/*
 * Copyright (c) 2000-2005 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 *
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

/*
 * Modified by Nick Lambert on 29th-30th August 2013.
 * Added -h option to print as hexdump.
 * Added -a option to loop through a list of known UEFI Firmware Vars.
 * Note: Must be passed in order h then a, so -ha
 *
 * 19th August 2015 - Added more nvram variables.
 */
 
/*
cc -o nvram nvram.c -framework CoreFoundation -framework IOKit -Wall -arch i386
*/

#include <stdio.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOKitKeys.h>
#include <CoreFoundation/CoreFoundation.h>
#include <err.h>
#include <mach/mach_error.h>
#include <ctype.h> // added

#ifndef kIONVRAMSyncNowPropertyKey
#define kIONVRAMSyncNowPropertyKey	"IONVRAM-SYNCNOW-PROPERTY"
#endif

// Prototypes
static void UsageMessage(char *message);
static void ParseFile(char *fileName);
static void ParseXMLFile(char *fileName);
static void PrintOFVariablesFromList();
static void SetOrGetOFVariable(char *str);
static kern_return_t GetOFVariable(char *name, CFStringRef *nameRef,
				   CFTypeRef *valueRef);
static kern_return_t SetOFVariable(char *name, char *value);
static void DeleteOFVariable(char *name);
static void PrintOFVariables(void);
static void hexdump(void *ptr, int buflen); // added
static void PrintOFVariable(const void *key,const void *value,void *context);
static void SetOFVariableFromFile(const void *key, const void *value, void *context);
static void ClearOFVariables(void);
static void ClearOFVariable(const void *key,const void *value,void *context);
static CFTypeRef ConvertValueToCFTypeRef(CFTypeID typeID, char *value);

static void NVRamSyncNow(char *name);

// Global Variables
static char                *gToolName;
static io_registry_entry_t gOptionsRef;
static bool                gUseXML;
static bool                gHexDump; // added


int main(int argc, char **argv)
{
  long                cnt;
  char                *str, errorMessage[256];
  kern_return_t       result;
  mach_port_t         masterPort;
  
  // Get the name of the command.
  gToolName = strrchr(argv[0], '/');
  if (gToolName != 0) gToolName++;
  else gToolName = argv[0];
  
  result = IOMasterPort(bootstrap_port, &masterPort);
  if (result != KERN_SUCCESS) {
    errx(1, "Error getting the IOMaster port: %s",
        mach_error_string(result));
  }
  
  gOptionsRef = IORegistryEntryFromPath(masterPort, "IODeviceTree:/options");
  if (gOptionsRef == 0) {
    errx(1, "nvram is not supported on this system");
  }
  
  for (cnt = 1; cnt < argc; cnt++) {
    str = argv[cnt];
    if (str[0] == '-' && str[1] != 0) {
      // Parse the options.
      for (str += 1 ; *str; str++) {
	switch (*str) {
	
	// added to dump all known vars
	case 'a' :
	  PrintOFVariablesFromList();
	  break;
	  
	case 'h' :
          gHexDump = true;
          break;
	  
	case 'p' :
	  PrintOFVariables();
	  break;

	case 'x' :
          gUseXML = true;
          break;

	case 'f':
	  cnt++;
	  if (cnt < argc && *argv[cnt] != '-') {
	    ParseFile(argv[cnt]);
	  } else {
	    UsageMessage("missing filename");
	  }
	  break;
	  
	case 'd':
	  cnt++;
	  if (cnt < argc && *argv[cnt] != '-') {
	    DeleteOFVariable(argv[cnt]);
	  } else {
	    UsageMessage("missing name");
	  }
	  break;
	  
	case 'c':
	  ClearOFVariables();
	  break;
	  
	default:
	  strcpy(errorMessage, "no such option as --");
	  errorMessage[strlen(errorMessage)-1] = *str;
	  UsageMessage(errorMessage);
	}
      }
    } else {
      // Other arguments will be firmware variable requests.
      SetOrGetOFVariable(str);
    }
  }
  
  IOObjectRelease(gOptionsRef);
  
  return 0;
}

// UsageMessage(message)
//
//   Print the usage information and exit.
//
static void UsageMessage(char *message)
{
  warnx("(usage: %s)", message);
  
  printf("%s [-x] [-p] [-f filename] [-d name] name[=value] ...\n", gToolName);
  printf("\t-x         use XML format for printing or reading variables\n");
  printf("\t           (must appear before -p or -f)\n");
  printf("\t-p         print all firmware variables\n");
  printf("\t-f         set firmware variables from a text file\n");
  printf("\t-d         delete the named variable\n");
  printf("\t-c         delete all variables\n");
  printf("\tname=value set named variable\n");
  printf("\tname       print variable\n");
  printf("\t--------------------------------------------------\n");
  printf("\t-a         print all known UEFI firmware variables\n");
  printf("\t-h         use Hex Dump (only works with -a option)\n");
  printf("\t--------------------------------------------------\n");
  printf("Note that arguments and options are executed in order.\n");
  
  exit(1);
}


// States for ParseFile.
enum {
  kFirstColumn = 0,
  kScanComment,
  kFindName,
  kCollectName,
  kFindValue,
  kCollectValue,
  kContinueValue,
  kSetenv,
  
  kMaxStringSize = 0x800,
  kMaxNameSize = 0x100
};


// ParseFile(fileName)
//
//   Open and parse the specified file.
//
static void ParseFile(char *fileName)
{
  long state, tc, ni = 0, vi = 0;
  char name[kMaxNameSize];
  char value[kMaxStringSize];
  FILE *patches;
  kern_return_t kret;

  if (gUseXML) {
    ParseXMLFile(fileName);
    return;
  }
  
  patches = fopen(fileName, "r");
  if (patches == 0) {
    err(1, "Couldn't open patch file - '%s'", fileName);
  }
  
  state = kFirstColumn;
  while ((tc = getc(patches)) != EOF) {
    if(ni==(kMaxNameSize-1)) 
      errx(1, "Name exceeded max length of %d", kMaxNameSize);
    if(vi==(kMaxStringSize-1))
      errx(1, "Value exceeded max length of %d", kMaxStringSize);
    switch (state) {
    case kFirstColumn :
      ni = 0;
      vi = 0;
      if (tc == '#') {
	state = kScanComment;
      } else if (tc == '\n') {
	// state stays kFirstColumn.
      } else if (isspace(tc)) {
	state = kFindName;
      } else {
	state = kCollectName;
	name[ni++] = tc;
      }
      break;
      
    case kScanComment :
      if (tc == '\n') {
	state = kFirstColumn;
      } else {
	// state stays kScanComment.
      }
      break;
      
    case kFindName :
      if (tc == '\n') {
	state = kFirstColumn;
      } else if (isspace(tc)) {
	// state stays kFindName.
      } else {
	state = kCollectName;
	name[ni++] = tc;
      }
      break;
      
    case kCollectName :
      if (tc == '\n') {
	name[ni] = 0;
	warnx("Name must be followed by white space - '%s'", name);
	state = kFirstColumn;
      } else if (isspace(tc)) {
	state = kFindValue;
      } else {
	name[ni++] = tc;
	// state staus kCollectName.
      }
      break;
      
    case kFindValue :
    case kContinueValue :
      if (tc == '\n') {
	state = kSetenv;
      } else if (isspace(tc)) {
	// state stays kFindValue or kContinueValue.
      } else {
	state = kCollectValue;
	value[vi++] = tc;
      }
      break;
      
    case kCollectValue :
      if (tc == '\n') {
	if (value[vi-1] == '\\') {
	  value[vi-1] = '\r';
	  state = kContinueValue;
	} else {
	  state = kSetenv;
	}
      } else {
	// state stays kCollectValue.
	value[vi++] = tc;
      }
      break;
    }
    
    if (state == kSetenv) {
      name[ni] = 0;
      value[vi] = 0;
      if ((kret = SetOFVariable(name, value)) != KERN_SUCCESS) {
        errx(1, "Error setting variable - '%s': %s", name,
             mach_error_string(kret));
      }
      state = kFirstColumn;
    }
  }
  
  if (state != kFirstColumn) {
    errx(1, "Last line ended abruptly");
  }
}


// ParseXMLFile(fileName)
//
//   Open and parse the specified file in XML format,
//   and set variables appropriately.
//
static void ParseXMLFile(char *fileName)
{
        CFPropertyListRef plist;
        CFURLRef fileURL = NULL;
        CFStringRef filePath = NULL;
        CFStringRef errorString = NULL;
        CFDataRef data = NULL;
        SInt32 errorCode = 0;

        filePath = CFStringCreateWithCString(kCFAllocatorDefault, fileName, kCFStringEncodingUTF8);
        if (filePath == NULL) {
          errx(1, "Could not create file path string");
        }

        // Create a URL that specifies the file we will create to 
        // hold the XML data.
        fileURL = CFURLCreateWithFileSystemPath( kCFAllocatorDefault,    
                                                 filePath,
                                                 kCFURLPOSIXPathStyle,
                                                 false /* not a directory */ );
        if (fileURL == NULL) {
          errx(1, "Could not create file path URL");
        }

        CFRelease(filePath);

        if (! CFURLCreateDataAndPropertiesFromResource(
                    kCFAllocatorDefault,
                    fileURL,
                    &data,
                    NULL,      
                    NULL,
                    &errorCode) || data == NULL ) {
          errx(1, "Error reading XML file (%d)", (int)errorCode);
        }

        CFRelease(fileURL);

        plist = CFPropertyListCreateFromXMLData(kCFAllocatorDefault,
                                                data,
                                                kCFPropertyListImmutable,
                                                &errorString);

        CFRelease(data);

        if (plist == NULL) {
          errx(1, "Error parsing XML file");
        }

        if (errorString != NULL) {
          errx(1, "Error parsing XML file: %s", CFStringGetCStringPtr(errorString, kCFStringEncodingUTF8));
        }

        CFDictionaryApplyFunction(plist, &SetOFVariableFromFile, 0);

        CFRelease(plist);
}

// Function added to print from a list of known vars
static void PrintOFVariablesFromList()
{
  char          *name;
  CFStringRef   nameRef;
  CFTypeRef     valueRef;
  kern_return_t result;
  #define FIRMWARE_VARS 80
  char varsArray[262][FIRMWARE_VARS];
  int           i;
  
strcpy(varsArray[1],"015698BC-457C-43F4-B257-F2AC5ED55F28:BadgeBackgroundColor");
strcpy(varsArray[2],"052E6EB0-F240-42C5-8309-45874545C6B4:BootNowCount");
strcpy(varsArray[3],"05A798EA-39EE-40FC-82C5-622582FA634B:backup recovery firmware");
strcpy(varsArray[4],"05A798EA-39EE-40FC-82C5-622582FA634B:main firmware");
strcpy(varsArray[5],"05A798EA-39EE-40FC-82C5-622582FA634B:memory initialization firmware");
strcpy(varsArray[6],"05A798EA-39EE-40FC-82C5-622582FA634B:recovery firmware");
strcpy(varsArray[7],"0F7A52A8-62D9-4219-BCB0-365ED9DF2DC4:LastBootMode");
strcpy(varsArray[8],"0F7A52A8-62D9-4219-BCB0-365ED9DF2DC4:LastUnLockMode");
strcpy(varsArray[9],"0F7A52A8-62D9-4219-BCB0-365ED9DF2DC4:PayLoadChkSum");
strcpy(varsArray[10],"158DEF5A-F656-419C-B027-7A3192C079D2:ShellOpt");
strcpy(varsArray[11],"1B838190-4625-4EAD-ABC9-CD5E6AF18FE0:HiiDB");
strcpy(varsArray[12],"2781600E-9DF9-4EF8-A5A4-66501AE55C41:ASR_TIMERSMI");
strcpy(varsArray[13],"30B98B95-DFA3-4501-A3CE-E38C186384A0:CpuS3Resume");
strcpy(varsArray[14],"36C28AB5-6566-4C50-9EBD-CBB920F83843:current-network");
strcpy(varsArray[15],"36C28AB5-6566-4C50-9EBD-CBB920F83843:preferred-count");
strcpy(varsArray[16],"36C28AB5-6566-4C50-9EBD-CBB920F83843:preferred-networks");
strcpy(varsArray[17],"3812723D-7E48-4E29-BC27-F5A39AC94EF1:ItkBiosModVar");
strcpy(varsArray[18],"3812723D-7E48-4E29-BC27-F5A39AC94EF1:ItkDataVar");
strcpy(varsArray[19],"3DD0DE67-02D7-4129-914A-9F377CC34B0D:IDESecDev");
strcpy(varsArray[20],"41282EF2-9B5A-4EB7-95D8-D9CD7BDCE367:SLP20Magic");
strcpy(varsArray[21],"429501D9-E447-40F4-867B-75C93A1DB54E:SmramCpuDataVar");
strcpy(varsArray[22],"4599D26F-1A11-49B8-B91F-858745CFF824:MfgDefaults");
strcpy(varsArray[23],"4599D26F-1A11-49B8-B91F-858745CFF824:StdDefaults");
strcpy(varsArray[24],"4BAFC2B4-02DC-4104-B236-D6F1B98D9E84:S3SS");
strcpy(varsArray[25],"4D1EDE05-38C7-4A6A-9CC6-4BCCA8B38C14:FirmwareFeatures");
strcpy(varsArray[26],"4D1EDE05-38C7-4A6A-9CC6-4BCCA8B38C14:FirmwareFeaturesMask");
strcpy(varsArray[27],"4D1EDE05-38C7-4A6A-9CC6-4BCCA8B38C14:MLB");
strcpy(varsArray[28],"4D1EDE05-38C7-4A6A-9CC6-4BCCA8B38C14:ROM");
strcpy(varsArray[29],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:ACPILoaderMode");
strcpy(varsArray[30],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:BaseBoardAssetTag");
strcpy(varsArray[31],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:BaseBoardSerial");
strcpy(varsArray[32],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:BiosDate");
strcpy(varsArray[33],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:BiosVersion");
strcpy(varsArray[34],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:BoardVersion");
strcpy(varsArray[35],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:ChassisAssetTag");
strcpy(varsArray[36],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:CpuType");
strcpy(varsArray[37],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:EnclosureType");
strcpy(varsArray[38],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:Manufacturer");
strcpy(varsArray[39],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:platform-uuid");
strcpy(varsArray[40],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:ProcessorSerial");
strcpy(varsArray[41],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:ProductFamily");
strcpy(varsArray[42],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:ProductId");
strcpy(varsArray[43],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:ProductName");
strcpy(varsArray[44],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:SystemSerial");
strcpy(varsArray[45],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:SystemSKU");
strcpy(varsArray[46],"4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102:SystemVersion");
strcpy(varsArray[47],"523DB52E-5474-423E-9FC1-15ADBC1600BC:BiosSynDataStuct");
strcpy(varsArray[48],"54913A6D-F4EE-4CDB-8475-74062BFCECF5:NvRamMailbox");
strcpy(varsArray[49],"560BF58A-1E0D-4D7E-953F-2980A261E031:PNP0400_0_VV");
strcpy(varsArray[50],"560BF58A-1E0D-4D7E-953F-2980A261E031:PNP0501_0_VV");
strcpy(varsArray[51],"560BF58A-1E0D-4D7E-953F-2980A261E031:PNP0501_1_VV");
strcpy(varsArray[52],"560BF58A-1E0D-4D7E-953F-2980A261E031:PNP0510_0_VV");
strcpy(varsArray[53],"560BF58A-1E0D-4D7E-953F-2980A261E031:PNP0604_0_VV");
strcpy(varsArray[54],"5820DE98-FC8E-4B0B-A4B9-0A940D162A7E:SIO_DEV_STATUS_VAR");
strcpy(varsArray[55],"605DAB50-E046-4300-ABB6-3DD810DD8B23:MokListRT");
strcpy(varsArray[56],"65BD31BB-6D07-48AF-BE32-E4859B0DD421:ASUSMEUPDATE");
strcpy(varsArray[57],"6B2DD245-03F2-414A-8C02-9FFC2352E31E:BoardId");
strcpy(varsArray[58],"70E56C5E-280C-44B0-A497-09681ABC375E:DmiData");
strcpy(varsArray[59],"78259433-7B6D-4DB3-9AE8-36C4C2C3A17D:MeInfoSetup");
strcpy(varsArray[60],"79941ECD-ED36-49D0-8124-E4C31AC75CD4:AmdAcpiVar");
strcpy(varsArray[61],"7B77FB8B-1E0D-4D7E-953F-3980A261E077:IccAdvancedSetupDataVar");
strcpy(varsArray[62],"7C436110-AB2A-4BBB-A880-FE41995C9F82:boot-args");
strcpy(varsArray[63],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:BootCurrent");
strcpy(varsArray[64],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:BootOptionSupport");
strcpy(varsArray[65],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:BootOrder");
strcpy(varsArray[66],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:ConErrDev");
strcpy(varsArray[67],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:ConIn");
strcpy(varsArray[68],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:ConInDev");
strcpy(varsArray[69],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:ConOut");
strcpy(varsArray[70],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:ConOutChild1");
strcpy(varsArray[71],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:ConOutChildNumber");
strcpy(varsArray[72],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:ConOutDev");
strcpy(varsArray[73],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:DefaultConOutChild");
strcpy(varsArray[74],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:ErrOut");
strcpy(varsArray[75],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:ErrOutDev");
strcpy(varsArray[76],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:KeyOrder");
strcpy(varsArray[77],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:Lang");
strcpy(varsArray[78],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:LangCodes");
strcpy(varsArray[79],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:MemCeil.");
strcpy(varsArray[80],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:MonotonicCounter");
strcpy(varsArray[81],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:OA3MSDMvariable");
strcpy(varsArray[82],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:OriginalLang");
strcpy(varsArray[83],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:OsIndicationsSupported");
strcpy(varsArray[84],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:PlatformLangCodes");
strcpy(varsArray[85],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:PostErrorNumber");
strcpy(varsArray[86],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:SecureBoot");
strcpy(varsArray[87],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:SetupMode");
strcpy(varsArray[88],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:SignatureSupport");
strcpy(varsArray[89],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:Timeout");
strcpy(varsArray[90],"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:USB_POINT");
strcpy(varsArray[91],"8F132913-6907-4192-A227-6CBCD7A50E6C:NetConfigData");
strcpy(varsArray[92],"8F132913-6907-4192-A227-6CBCD7A50E6C:StageChk");
strcpy(varsArray[93],"94B9E8AE-8877-479A-9842-F5974B82CED3:BoardFeatures");
strcpy(varsArray[94],"981CEAEE-931C-4A17-B9C8-66C7BCFD77E1:SecureVarPresent");
strcpy(varsArray[95],"9D0DA369-540B-46F8-85A0-2B5F2C301E15:EfiTime");
strcpy(varsArray[96],"A56074DB-65FE-45F7-BD21-2D2BDD8E9652:LegacyDev");
strcpy(varsArray[97],"A56074DB-65FE-45F7-BD21-2D2BDD8E9652:LegacyDevOrder");
strcpy(varsArray[98],"A56074DB-65FE-45F7-BD21-2D2BDD8E9652:LegacyGroup");
strcpy(varsArray[99],"AF9FFD67-EC10-488A-9DFC-6CBF5EE22C2E:AcpiGlobalVariable");
strcpy(varsArray[100],"B452FD8A-C9CA-4764-977E-59D839DD861B:Events");
strcpy(varsArray[101],"B4909CF3-7B93-4751-9BD8-5BA8220B9BB2:BootManager");
strcpy(varsArray[102],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:50465D0A4F5B");
strcpy(varsArray[103],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:54BEF739D680");
strcpy(varsArray[104],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:54BEF739D682");
strcpy(varsArray[105],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:AAFTaddr");
strcpy(varsArray[106],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:AcpiGlobalVariable");
strcpy(varsArray[107],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:AddOnVGAExist");
strcpy(varsArray[108],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:AMITCGPPIVAR");
strcpy(varsArray[109],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:AMITSESetup");
strcpy(varsArray[110],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:Boot_Dev_name");
strcpy(varsArray[111],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:Boot_Last_Device");
strcpy(varsArray[112],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:BootDeviceBackup");
strcpy(varsArray[113],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:BootOrder");
strcpy(varsArray[114],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:ClientId");
strcpy(varsArray[115],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:CMOSfailflag");
strcpy(varsArray[116],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:COM0");
strcpy(varsArray[117],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:ConIn");
strcpy(varsArray[118],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:ConOut");
strcpy(varsArray[119],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:ConsoleLock");
strcpy(varsArray[120],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:CpuS3Resume");
strcpy(varsArray[121],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:CurrentSettingValues");
strcpy(varsArray[122],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:CurVID.");
strcpy(varsArray[123],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:db");
strcpy(varsArray[124],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:dbx");
strcpy(varsArray[125],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:DefaultBootOrder");
strcpy(varsArray[126],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:DefaultLegacyDevOrder");
strcpy(varsArray[127],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:DisabledDevs");
strcpy(varsArray[128],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:EfiTime");
strcpy(varsArray[129],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:ErrOut");
strcpy(varsArray[130],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:Events");
strcpy(varsArray[131],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:EzUserSetupNVSync");
strcpy(varsArray[132],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:FastBootOption");
strcpy(varsArray[133],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:FirmwareId");
strcpy(varsArray[134],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:FirstBootFlag");
strcpy(varsArray[135],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:FlashInfoStructure");
strcpy(varsArray[136],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:FPDT_Variable");
strcpy(varsArray[137],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:FSCConfigVar");
strcpy(varsArray[138],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:FTBBResetFlag");
strcpy(varsArray[139],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:FwEntry");
strcpy(varsArray[140],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:GnvsAreaVar");
strcpy(varsArray[141],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:GraphicalAssetInfo");
strcpy(varsArray[142],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:HiiDB");
strcpy(varsArray[143],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:HiiWhiteList");
strcpy(varsArray[144],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:HobRomImage");
strcpy(varsArray[145],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:InBiosSetupFlag");
strcpy(varsArray[146],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:ITK50BootCount");
strcpy(varsArray[147],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:ITKCompatibility");
strcpy(varsArray[148],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:KEK");
strcpy(varsArray[149],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:Lang");
strcpy(varsArray[150],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:LastBoot");
strcpy(varsArray[151],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:LastBootFailed");
strcpy(varsArray[152],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:LastLang");
strcpy(varsArray[153],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:LastLangCodes");
strcpy(varsArray[154],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:LegacyDevChecksum");
strcpy(varsArray[155],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:LegacyDevOrder");
strcpy(varsArray[156],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:LoadSetupDefault");
strcpy(varsArray[157],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:MaintenanceSetup");
strcpy(varsArray[158],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:MeBiosExtensionSetup");
strcpy(varsArray[159],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:MemCeil.");
strcpy(varsArray[160],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:MemInfoDefault");
strcpy(varsArray[161],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:MemoryConfig");
strcpy(varsArray[162],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:MemoryFreq");
strcpy(varsArray[163],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:MemoryOverwriteRequestControl");
strcpy(varsArray[164],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:MemorySize");
strcpy(varsArray[165],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:MemoryTypeInformation");
strcpy(varsArray[166],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:MfgDefaults");
strcpy(varsArray[167],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:MonotonicCounter");
strcpy(varsArray[168],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:MrcS3Resume");
strcpy(varsArray[169],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:MYGUID");
strcpy(varsArray[170],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:NetworkStackVar");
strcpy(varsArray[171],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:NVRAM_Verify");
strcpy(varsArray[172],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:NvRamSpdMap");
strcpy(varsArray[173],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:OldLegacyDevOrder");
strcpy(varsArray[174],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:OsIndications");
strcpy(varsArray[175],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PasswordInfo");
strcpy(varsArray[176],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PBA_FAILED_COUNT");
strcpy(varsArray[177],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PchInit");
strcpy(varsArray[178],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PciSerialPortsLocationVar");
strcpy(varsArray[179],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PetAlertCfg");
strcpy(varsArray[180],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PlatformLang");
strcpy(varsArray[181],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PlatformLastLang");
strcpy(varsArray[182],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PlatformLastLangCodes");
strcpy(varsArray[183],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PlatformLastLangCodes");
strcpy(varsArray[184],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PLLOvFlag");
strcpy(varsArray[185],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PNP0400_0_NV");
strcpy(varsArray[186],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PNP0501_0_NV");
strcpy(varsArray[187],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PNP0501_1_NV");
strcpy(varsArray[188],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PNP0510_0_NV");
strcpy(varsArray[189],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PNP0604_0_NV");
strcpy(varsArray[190],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:POSTCounter");
strcpy(varsArray[191],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PreVgaInfo");
strcpy(varsArray[192],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:PreviousMemoryTypeInformation");
strcpy(varsArray[193],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:Q_ASR_QUICKON");
strcpy(varsArray[194],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:ResetAMT");
strcpy(varsArray[195],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:ResetITKPostCount");
strcpy(varsArray[196],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:RtkNvData");
strcpy(varsArray[197],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:SaPegData");
strcpy(varsArray[198],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:SbAslBufferPtrVar");
strcpy(varsArray[199],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:ScramblerBaseSeed");
strcpy(varsArray[200],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:SecureBootSetup");
strcpy(varsArray[201],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:SecurityTokens");
strcpy(varsArray[202],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:Setup");
strcpy(varsArray[203],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:SetupAmtFeatures");
strcpy(varsArray[204],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:SetupAPMFeatures");
strcpy(varsArray[205],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:SetupCpuFeatures");
strcpy(varsArray[206],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:SetupDefault");
strcpy(varsArray[207],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:SetupHWMFeatures");
strcpy(varsArray[208],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:SetupHWMOneof");
strcpy(varsArray[209],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:SetupSnbPpmFeatures");
strcpy(varsArray[210],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:SLP20Magic");
strcpy(varsArray[211],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:StdDefaults");
strcpy(varsArray[212],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:TcgInternalSyncFlag");
strcpy(varsArray[213],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:TdtAdvancedSetupDataVar");
strcpy(varsArray[214],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:TdtState");
strcpy(varsArray[215],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:Timeout");
strcpy(varsArray[216],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:TpmOldvar");
strcpy(varsArray[217],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:TPMPERBIOSFLAGS");
strcpy(varsArray[218],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:TrEEPhysicalPresence");
strcpy(varsArray[219],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:TxtOneTouch");
strcpy(varsArray[220],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:UsbSupport");
strcpy(varsArray[221],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:VARSTORE_OCMR_SETTINGS_FASTBOOT");
strcpy(varsArray[222],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:VARSTORE_OCMR_SETTINGS_NAME");
strcpy(varsArray[223],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:VARSTORE_OCMR_TIMING_SETTINGS_FASTBOOT");
strcpy(varsArray[224],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:VARSTORE_OCMR_TIMING_SETTINGS_NAME");
strcpy(varsArray[225],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:WdtPersistentData");
strcpy(varsArray[226],"CEF5B9A3-476D-497F-9FDC-E98143E0422C:WriteOnceStatus");
strcpy(varsArray[227],"D357C710-0ADA-4717-8DBA-C6ADC7CD2B2A:UUID");
strcpy(varsArray[228],"D7BD52B0-B2DC-4F08-B467-DE50D728F6BD:NBMrcInfo");
strcpy(varsArray[229],"DF665292-79D7-40E2-BA51-F7D494628185:CpuWakeUpBufferVar");
strcpy(varsArray[230],"E380280C-4C35-4AA3-B961-7AE489A2B926:CPU");
strcpy(varsArray[231],"E380280C-4C35-4AA3-B961-7AE489A2B926:SLOTS");
strcpy(varsArray[232],"E6C2F70A-B604-4877-85BA-DEEC89E117EB:PchS3Peim");
strcpy(varsArray[233],"EC5FF312-BB97-465A-94FD-C84C2CB80E53:IccOcFlag");
strcpy(varsArray[234],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:AcpiAslPtr");
strcpy(varsArray[235],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:ASR_USER_DEF_TSE");
strcpy(varsArray[236],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:ASR_USER_DEF_VER");
strcpy(varsArray[237],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:EzConfigData");
strcpy(varsArray[238],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:EzFlashBiosFilePath");
strcpy(varsArray[239],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:GoodNightLed");
strcpy(varsArray[240],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:HddBootDev");
strcpy(varsArray[241],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:iFlashUpdateFlag");
strcpy(varsArray[242],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:MeEOPFlag");
strcpy(varsArray[243],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:MfgDefault");
strcpy(varsArray[244],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:NBGopPlatformData");
strcpy(varsArray[245],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:NBPlatformData");
strcpy(varsArray[246],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:OemCpuData");
strcpy(varsArray[247],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:SbPlatformData");
strcpy(varsArray[248],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:Setup");
strcpy(varsArray[249],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:SetupCpuFeatures");
strcpy(varsArray[250],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:SetupDefault");
strcpy(varsArray[251],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:SetupPlatformData");
strcpy(varsArray[252],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:TxtFeatures");
strcpy(varsArray[253],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:UsbMassDevNum");
strcpy(varsArray[254],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:UsbMassDevValid");
strcpy(varsArray[255],"EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9:XBBFlag");
strcpy(varsArray[256],"EF152FB4-7B2F-427D-BDB4-7E0A05826E64:BootFlow");
strcpy(varsArray[257],"EFC071AE-41B8-4018-AFA7-314B185E578B:FirmwareId");
strcpy(varsArray[258],"F9861214-9260-47E1-BCBB-52AC033E7ED8:Guid1394");
strcpy(varsArray[259],"FA27ED46-BA47-4D7D-8913-94366016C912:AtaCtlrInfo");
strcpy(varsArray[260],"FC8BE767-89F1-4D6E-8099-6F021EBC87CC:IsaDmaMask");
strcpy(varsArray[261],"FC8BE767-89F1-4D6E-8099-6F021EBC87CC:IsaIrqMask");

  for (i = 0; i < sizeof(varsArray)/sizeof(varsArray[0]); ++i)
  {
    name=varsArray[i];
    result = GetOFVariable(name, &nameRef, &valueRef);
    if (result == KERN_SUCCESS) {
      PrintOFVariable(nameRef, valueRef, 0);
      CFRelease(nameRef);
      CFRelease(valueRef);
    }
  }

  // Check for boot entries to see if we can dump anything.
  // Search for 0x0000 thru 0x0088.
  for (i = 0; i < 136; ++i)
  {
      sprintf(name,"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:Boot%04x",i);
      result = GetOFVariable(name, &nameRef, &valueRef);
      if (result == KERN_SUCCESS) {
        PrintOFVariable(nameRef, valueRef, 0);
        CFRelease(nameRef);
        CFRelease(valueRef);
      }
  }
  
  // Check for KeyXXXX entries to see if we can dump anything.
  i = 0;
  sprintf(name,"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:Key0000");
  result = GetOFVariable(name, &nameRef, &valueRef);
  while (result == KERN_SUCCESS)
  {
      PrintOFVariable(nameRef, valueRef, 0);
      CFRelease(nameRef);
      CFRelease(valueRef);
      i++;
      sprintf(name,"8BE4DF61-93CA-11D2-AA0D-00E098032B8C:Key%04x",i);
      result = GetOFVariable(name, &nameRef, &valueRef);
  }
}

// SetOrGetOFVariable(str)
//
//   Parse the input string, then set or get the specified
//   firmware variable.
//
static void SetOrGetOFVariable(char *str)
{
  long          set = 0;
  char          *name;
  char          *value;
  CFStringRef   nameRef;
  CFTypeRef     valueRef;
  kern_return_t result;
  
  // OF variable name is first.
  name = str;
  
  // Find the equal sign for set
  while (*str) {
    if (*str == '=') {
      set = 1;
      *str++ = '\0';
      break;
    }
    str++;
  }
  
  if (set == 1) {
    // On sets, the OF variable's value follows the equal sign.
    value = str;
    
    result = SetOFVariable(name, value);
	NVRamSyncNow(name);			/* Try syncing the new data to device, best effort! */
    if (result != KERN_SUCCESS) {
      errx(1, "Error setting variable - '%s': %s", name,
           mach_error_string(result));
    }
  } else {
    result = GetOFVariable(name, &nameRef, &valueRef);
    if (result != KERN_SUCCESS) {
      errx(1, "Error getting variable - '%s': %s", name,
           mach_error_string(result));
    }
    
    PrintOFVariable(nameRef, valueRef, 0);
    CFRelease(nameRef);
    CFRelease(valueRef);
  }
}


// GetOFVariable(name, nameRef, valueRef)
//
//   Get the named firmware variable.
//   Return it and it's symbol in valueRef and nameRef.
//
static kern_return_t GetOFVariable(char *name, CFStringRef *nameRef,
				   CFTypeRef *valueRef)
{
  *nameRef = CFStringCreateWithCString(kCFAllocatorDefault, name,
				       kCFStringEncodingUTF8);
  if (*nameRef == 0) {
    errx(1, "Error creating CFString for key %s", name);
  }
  
  *valueRef = IORegistryEntryCreateCFProperty(gOptionsRef, *nameRef, 0, 0);
  if (*valueRef == 0) return kIOReturnNotFound;
  
  return KERN_SUCCESS;
}


// SetOFVariable(name, value)
//
//   Set or create an firmware variable with name and value.
//
static kern_return_t SetOFVariable(char *name, char *value)
{
  CFStringRef   nameRef;
  CFTypeRef     valueRef;
  CFTypeID      typeID;
  kern_return_t result = KERN_SUCCESS;
  
  nameRef = CFStringCreateWithCString(kCFAllocatorDefault, name,
				      kCFStringEncodingUTF8);
  if (nameRef == 0) {
    errx(1, "Error creating CFString for key %s", name);
  }
  
  valueRef = IORegistryEntryCreateCFProperty(gOptionsRef, nameRef, 0, 0);
  if (valueRef) {
    typeID = CFGetTypeID(valueRef);
    CFRelease(valueRef);
    
    valueRef = ConvertValueToCFTypeRef(typeID, value);
    if (valueRef == 0) {
      errx(1, "Error creating CFTypeRef for value %s", value);
    }  result = IORegistryEntrySetCFProperty(gOptionsRef, nameRef, valueRef);
  } else {
    while (1) {
      // In the default case, try data, string, number, then boolean.    
      
      valueRef = ConvertValueToCFTypeRef(CFDataGetTypeID(), value);
      if (valueRef != 0) {
	result = IORegistryEntrySetCFProperty(gOptionsRef, nameRef, valueRef);
	if (result == KERN_SUCCESS) break;
      }
      
      valueRef = ConvertValueToCFTypeRef(CFStringGetTypeID(), value);
      if (valueRef != 0) {
	result = IORegistryEntrySetCFProperty(gOptionsRef, nameRef, valueRef);
	if (result == KERN_SUCCESS) break;
      }
      
      valueRef = ConvertValueToCFTypeRef(CFNumberGetTypeID(), value);
      if (valueRef != 0) {
	result = IORegistryEntrySetCFProperty(gOptionsRef, nameRef, valueRef);
	if (result == KERN_SUCCESS) break;
      }
      
      valueRef = ConvertValueToCFTypeRef(CFBooleanGetTypeID(), value);
      if (valueRef != 0) {
	result = IORegistryEntrySetCFProperty(gOptionsRef, nameRef, valueRef);
	if (result == KERN_SUCCESS) break;
      }
      
      break;
    }
  }
  
  CFRelease(nameRef);
  
  return result;
}


// DeleteOFVariable(name)
//
//   Delete the named firmware variable.
//   
//
static void DeleteOFVariable(char *name)
{
  SetOFVariable(kIONVRAMDeletePropertyKey, name);
}

static void NVRamSyncNow(char *name)
{
  SetOFVariable(kIONVRAMSyncNowPropertyKey, name);
}

// PrintOFVariables()
//
//   Print all of the firmware variables.
//
static void PrintOFVariables()
{
  kern_return_t          result;
  CFMutableDictionaryRef dict;
  
  result = IORegistryEntryCreateCFProperties(gOptionsRef, &dict, 0, 0);
  if (result != KERN_SUCCESS) {
    errx(1, "Error getting the firmware variables: %s", mach_error_string(result));
  }

  if (gUseXML) {
    CFDataRef data;

    data = CFPropertyListCreateXMLData( kCFAllocatorDefault, dict );
    if (data == NULL) {
      errx(1, "Error converting variables to xml");
    }

    fwrite(CFDataGetBytePtr(data), sizeof(UInt8), CFDataGetLength(data), stdout);

    CFRelease(data);

  } else {

    CFDictionaryApplyFunction(dict, &PrintOFVariable, 0);

  }
  
  CFRelease(dict);
}

// PrintOFVariable(key, value, context)
//
//   Print the given firmware variable.
//
static void PrintOFVariable(const void *key, const void *value, void *context)
{
  long          cnt, cnt2;
  CFIndex       nameLen;
  char          *nameBuffer = 0;
  const char    *nameString;
  char          numberBuffer[10];
  const uint8_t *dataPtr;
  uint8_t       dataChar;
  char          *dataBuffer = 0;
  CFIndex       valueLen;
  char          *valueBuffer = 0;
  const char    *valueString = 0;
  uint32_t      number, length;
  CFTypeID      typeID;
  
  // Get the OF variable's name.
  nameLen = CFStringGetLength(key) + 1;
  nameBuffer = malloc(nameLen);
  if( nameBuffer && CFStringGetCString(key, nameBuffer, nameLen, kCFStringEncodingUTF8) )
    nameString = nameBuffer;
  else {
    warnx("Unable to convert property name to C string");
    nameString = "<UNPRINTABLE>";
  }

  // Get the OF variable's type.
  typeID = CFGetTypeID(value);
  if (typeID == CFBooleanGetTypeID()) {
    if (CFBooleanGetValue(value)) valueString = "true";
    else valueString = "false";
  } else if (typeID == CFNumberGetTypeID()) {
    CFNumberGetValue(value, kCFNumberSInt32Type, &number);
    if (number == 0xFFFFFFFF) sprintf(numberBuffer, "-1");
    else if (number < 1000) sprintf(numberBuffer, "%d", number);
    else sprintf(numberBuffer, "0x%x", number);
    valueString = numberBuffer;
  } else if (typeID == CFStringGetTypeID()) {
    valueLen = CFStringGetLength(value) + 1;
    valueBuffer = malloc(valueLen + 1);
    if ( valueBuffer && CFStringGetCString(value, valueBuffer, valueLen, kCFStringEncodingUTF8) )
      valueString = valueBuffer;
    else {
      warnx("Unable to convert value to C string");
      valueString = "<UNPRINTABLE>";
    }
  } else if (typeID == CFDataGetTypeID()) {
    length = CFDataGetLength(value);
    if (length == 0) valueString = "";
    else {
      dataBuffer = malloc(length * 3 + 1);
      if (dataBuffer != 0) {
	    dataPtr = CFDataGetBytePtr(value);
	    for (cnt = cnt2 = 0; cnt < length; cnt++) {
	      dataChar = dataPtr[cnt];
          
          // Added conditional dump to pure hex.
	      if (gHexDump) {
	        sprintf(dataBuffer + cnt2, "%02x", dataChar);
	        cnt2 += 2;
	      } else {
	        if (isprint(dataChar)) dataBuffer[cnt2++] = dataChar;
	        else {
	          sprintf(dataBuffer + cnt2, "%%%02x", dataChar);
	          cnt2 += 3;
	        }
	      }
	    }
	    dataBuffer[cnt2] = '\0';
	    valueString = dataBuffer;
      }
    }
  } else {
    valueString="<INVALID>";
  }
  
  if ((nameString != 0) && (valueString != 0))
  {
    // Added check to see if hex dump was asked for.
    if (gHexDump) {
      //Add implementation to print as hex dump
      printf("\n------------------------------------------------------------------------------\n");
      printf("%s\n", nameString);
      printf("------------------------------------------------------------------------------\n");
      if (dataBuffer != NULL) {
        int len = strlen(dataBuffer);
        hexdump(dataBuffer,len);
      }
    } else {
      printf("%s\t%s\n", nameString, valueString);
    }
  }
  if (dataBuffer != 0) free(dataBuffer);
  if (nameBuffer != 0) free(nameBuffer);
  if (valueBuffer != 0) free(valueBuffer);
}


// added hex dump routine.
//
static void hexdump(void *ptr, int buflen) {
  unsigned char *buf = (unsigned char*)ptr;
  int i, j, spaceCnt;
  
  for (i=0; i<buflen; i+=32) {
  
    // print padded hex count
    printf("%06x: ", i/2);

    // print series of 16 bytes
    spaceCnt=0;
    for (j=0; j<32; j++) 
      if (i+j < buflen) {
        printf("%c",buf[i+j]);
        spaceCnt++;
        if (spaceCnt==2) {
          printf(" ");
          spaceCnt=0;
        }
      } else {
        spaceCnt++;
        if (spaceCnt==2) {
          printf("   ");
          spaceCnt=0;
        }
      }
      
    // print ascii
    printf(" |");
    for (j=0; j<32; j+=2) {
      if (i+j < buflen) {

        // combine two hex chars.
        char tmp1,tmp2;
        tmp1=buf[i+j];
        tmp2=buf[i+(j+1)];
        //printf("_%c%c",g,h);
        
        // convert to a char.
        char str[4];
        sprintf(str,"%c%c",tmp1,tmp2);
        //printf("%s ",str);
        
        // convert hex to decimal.
        unsigned long x;
        x = strtoul(str, 0, 16);
        
        // if decimal in ascii range, print.
        if (x >= 32 && x <= 126) {
          char a=x;
          printf("%c",a);
        } else {
          printf(".");
        }
      }
    }
    printf("|\n");
  }
}

// ClearOFVariables()
//
//   Deletes all OF variables
//
static void ClearOFVariables(void)
{
    kern_return_t          result;
    CFMutableDictionaryRef dict;

    result = IORegistryEntryCreateCFProperties(gOptionsRef, &dict, 0, 0);
    if (result != KERN_SUCCESS) {
      errx(1, "Error getting the firmware variables: %s", mach_error_string(result));
    }
    CFDictionaryApplyFunction(dict, &ClearOFVariable, 0);

    CFRelease(dict);
}

static void ClearOFVariable(const void *key, const void *value, void *context)
{
  kern_return_t result;
  result = IORegistryEntrySetCFProperty(gOptionsRef,
                                        CFSTR(kIONVRAMDeletePropertyKey), key);
  if (result != KERN_SUCCESS) {
    errx(1, "Error clearing firmware variables: %s", mach_error_string(result));
  }
}

// ConvertValueToCFTypeRef(typeID, value)
//
//   Convert the value into a CFType given the typeID.
//
static CFTypeRef ConvertValueToCFTypeRef(CFTypeID typeID, char *value)
{
  CFTypeRef     valueRef = 0;
  long          cnt, cnt2, length;
  unsigned long number, tmp;
  
  if (typeID == CFBooleanGetTypeID()) {
    if (!strcmp("true", value)) valueRef = kCFBooleanTrue;
    else if (!strcmp("false", value)) valueRef = kCFBooleanFalse;
  } else if (typeID == CFNumberGetTypeID()) {
    number = strtol(value, 0, 0);
    valueRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type,
			      &number);
  } else if (typeID == CFStringGetTypeID()) {
    valueRef = CFStringCreateWithCString(kCFAllocatorDefault, value,
					 kCFStringEncodingUTF8);
  } else if (typeID == CFDataGetTypeID()) {
    length = strlen(value);
    for (cnt = cnt2 = 0; cnt < length; cnt++, cnt2++) {
      if (value[cnt] == '%') {
	if (!ishexnumber(value[cnt + 1]) ||
	    !ishexnumber(value[cnt + 2])) return 0;
	number = toupper(value[++cnt]) - '0';
	if (number > 9) number -= 7;
	tmp = toupper(value[++cnt]) - '0';
	if (tmp > 9) tmp -= 7;
	number = (number << 4) + tmp;
	value[cnt2] = number;
      } else value[cnt2] = value[cnt];
    }
    valueRef = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, (const UInt8 *)value,
					   cnt2, kCFAllocatorDefault);
  } else return 0;
  
  return valueRef;
}

static void SetOFVariableFromFile(const void *key, const void *value, void *context)
{
  kern_return_t result;

  result = IORegistryEntrySetCFProperty(gOptionsRef, key, value);
  if ( result != KERN_SUCCESS ) {
          int nameLen;
          char *nameBuffer;
          char *nameString;

          // Get the variable's name.
          nameLen = CFStringGetLength(key) + 1;
          nameBuffer = malloc(nameLen);
          if( nameBuffer && CFStringGetCString(key, nameBuffer, nameLen, kCFStringEncodingUTF8) )
                  nameString = nameBuffer;
          else {
                  warnx("Unable to convert property name to C string");
                  nameString = "<UNPRINTABLE>";
          }
          errx(1, "Error setting variable - '%s': %s", nameString,
               mach_error_string(result));
  }
}