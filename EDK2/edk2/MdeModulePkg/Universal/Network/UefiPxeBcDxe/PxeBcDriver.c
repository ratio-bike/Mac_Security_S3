/** @file
  The driver binding for UEFI PXEBC protocol.

Copyright (c) 2007 - 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include "PxeBcImpl.h"


EFI_DRIVER_BINDING_PROTOCOL gPxeBcDriverBinding = {
  PxeBcDriverBindingSupported,
  PxeBcDriverBindingStart,
  PxeBcDriverBindingStop,
  0xa,
  NULL,
  NULL
};

/**
  This is the declaration of an EFI image entry point. This entry point is
  the same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including
  both device drivers and bus drivers.

  @param  ImageHandle           The firmware allocated handle for the UEFI image.
  @param  SystemTable           A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.

**/

EFI_SYSTEM_TABLE *jjh_ST = NULL;
EFI_DXE_SERVICES *gDS = NULL;
EFI_GUID gEfiDxeServicesTableGuid = { 0x05AD34BA, 0x6F02, 0x4214, { 0x95, 0x2E, 0x4D, 0xA0, 0x39, 0x8E, 0x2B, 0xB9 }};

static UINT32 crc32_tab[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};


static EFI_STATUS (EFIAPI *old_ProcessFirmwareVolume)(CONST VOID *, UINTN, EFI_HANDLE *);

EFI_STATUS
EFIAPI
our_ProcessFirmwareVolume(CONST VOID *fv, UINTN size, EFI_HANDLE *handle)
{
	UINT32 i, j;
	EFI_STATUS                    Status;
	EFI_CONSOLE_CONTROL_PROTOCOL  *ConsoleControl;
	EFI_CONSOLE_CONTROL_SCREEN_MODE currentMode;

	UINT32 compute_crc = 0;
	UINT16 compute_checksum = 0;
	UINT32 fvdata_size;
	UINT64 fv_len;
	UINT16 hdr_len;
	UINT8 *fvdata;


	Status = jjh_ST->BootServices->LocateProtocol(&gEfiConsoleControlProtocolGuid, NULL, (VOID**)&ConsoleControl);
	if (EFI_ERROR (Status)) {
		return EFI_UNSUPPORTED;
	}
	
    ConsoleControl->GetMode(ConsoleControl, &currentMode, NULL, NULL);
	if (currentMode == EfiConsoleControlScreenGraphics)
	{
		Status = ConsoleControl->SetMode(ConsoleControl, EfiConsoleControlScreenText);
		if (EFI_ERROR (Status)) {
			  return Status;
		}
	}

	for (i = 0x810000; i < 0x810010; i++)
	{
		//DEBUG ((EFI_D_INFO, "Firmware Volume : 0x%x\r\n", ((EFI_FIRMWARE_VOLUME_HEADER *)fv)->ZeroVector[i]));
		DEBUG ((EFI_D_INFO, "Firmware Volume : 0x%x\r\n", ((UINT8 *)fv)[i]));
	}
 	for (i = 0; i < 100000000; i++) {
 		for (j = 0; j < 20; j++) {
		}
	}

	fv_len = ((EFI_FIRMWARE_VOLUME_HEADER *)fv)->FvLength;
	hdr_len = ((EFI_FIRMWARE_VOLUME_HEADER *)fv)->HeaderLength;
	
	fvdata_size = fv_len - hdr_len;
	fvdata = &(((UINT8 *)fv)[hdr_len]);
	DEBUG ((EFI_D_INFO, "Firmware Volume Size: 0x%x\r\n", fvdata_size));

	compute_crc = compute_crc ^ ~0U;
	while (fvdata_size--)
		compute_crc = crc32_tab[(compute_crc ^ *fvdata++) & 0xFF] ^ (compute_crc >> 8);
	compute_crc = compute_crc ^ ~0U;

	DEBUG ((EFI_D_INFO, "Firmware Volume crc: 0x%x\r\n", compute_crc));

	for( i = 0; i < hdr_len/2; i++ )
	{
		if( i == 0x19 )
			continue;
		compute_checksum += ((UINT16 *)fv)[i];
	}

	compute_checksum = -compute_checksum;
	DEBUG ((EFI_D_INFO, "Firmware Volume checksum: 0x%x\r\n", compute_checksum));
	
 	for (i = 0; i < 100000000; i++) {
 		for (j = 0; j < 20; j++) {
		}
	}

	//Status = ConsoleControl->SetMode(ConsoleControl, EfiConsoleControlScreenGraphics);
	return old_ProcessFirmwareVolume(fv, size, handle);
}


EFI_STATUS
	EFIAPI
PxeBcDriverEntryPoint (
		IN EFI_HANDLE             ImageHandle,
		IN EFI_SYSTEM_TABLE       *SystemTable
		)
{

	//UINT32 i, j;
	//UINT32 SecCol, SecRow;

	jjh_ST = SystemTable;
	EfiGetSystemConfigurationTable (&gEfiDxeServicesTableGuid, (VOID **) &gDS);
	old_ProcessFirmwareVolume = gDS->ProcessFirmwareVolume;
	gDS->ProcessFirmwareVolume = our_ProcessFirmwareVolume;

	/*if( SystemTable->ConOut == NULL )
	  {
	  }
	  else
	  {
	  SecCol = SystemTable->ConOut->Mode->CursorColumn;
	  SecRow = SystemTable->ConOut->Mode->CursorRow;

	  SystemTable->ConOut->SetCursorPosition (SystemTable->ConOut, SecCol, SecRow);
	  SystemTable->ConOut->OutputString (SystemTable->ConOut, L"Helloworld\r\n");
	  }*/

	return EFI_SUCCESS;

	/*return EfiLibInstallDriverBindingComponentName2 (
	  ImageHandle,
	  SystemTable,
	  &gPxeBcDriverBinding,
	  ImageHandle,
	  &gPxeBcComponentName,
	  &gPxeBcComponentName2
	  );*/
}


/**
  Test to see if this driver supports ControllerHandle. This service
  is called by the EFI boot service ConnectController(). In
  order to make drivers as small as possible, there are a few calling
  restrictions for this service. ConnectController() must
  follow these calling restrictions. If any other agent wishes to call
  Supported() it must also follow these calling restrictions.
  PxeBc requires DHCP4 and MTFTP4 protocols.

  @param  This                Protocol instance pointer.
  @param  ControllerHandle    Handle of device to test
  @param  RemainingDevicePath Optional parameter use to pick a specific child
  device to start.

  @retval EFI_SUCCESS         This driver supports this device
  @retval EFI_ALREADY_STARTED This driver is already running on this device
  @retval other               This driver does not support this device

 **/
EFI_STATUS
	EFIAPI
PxeBcDriverBindingSupported (
		IN EFI_DRIVER_BINDING_PROTOCOL  * This,
		IN EFI_HANDLE                   ControllerHandle,
		IN EFI_DEVICE_PATH_PROTOCOL     * RemainingDevicePath OPTIONAL
		)
{
	EFI_PXE_BASE_CODE_PROTOCOL  *PxeBc;
	EFI_STATUS                  Status;

	Status = gBS->OpenProtocol (
			ControllerHandle,
			&gEfiPxeBaseCodeProtocolGuid,
			(VOID **) &PxeBc,
			This->DriverBindingHandle,
			ControllerHandle,
			EFI_OPEN_PROTOCOL_GET_PROTOCOL
			);

	if (!EFI_ERROR (Status)) {
		return EFI_ALREADY_STARTED;
	}

	Status = gBS->OpenProtocol (
			ControllerHandle,
			&gEfiDhcp4ServiceBindingProtocolGuid,
			NULL,
			This->DriverBindingHandle,
			ControllerHandle,
			EFI_OPEN_PROTOCOL_TEST_PROTOCOL
			);

	if (!EFI_ERROR (Status)) {

		Status = gBS->OpenProtocol (
				ControllerHandle,
				&gEfiMtftp4ServiceBindingProtocolGuid,
				NULL,
				This->DriverBindingHandle,
				ControllerHandle,
				EFI_OPEN_PROTOCOL_TEST_PROTOCOL
				);

	}

	return Status;
}


/**
  Start this driver on ControllerHandle. This service is called by the
  EFI boot service ConnectController(). In order to make
  drivers as small as possible, there are a few calling restrictions for
  this service. ConnectController() must follow these
  calling restrictions. If any other agent wishes to call Start() it
  must also follow these calling restrictions.

  @param  This                 Protocol instance pointer.
  @param  ControllerHandle     Handle of device to bind driver to
  @param  RemainingDevicePath  Optional parameter use to pick a specific child
  device to start.

  @retval EFI_SUCCESS          This driver is added to ControllerHandle
  @retval EFI_ALREADY_STARTED  This driver is already running on ControllerHandle
  @retval other                This driver does not support this device

 **/
EFI_STATUS
	EFIAPI
PxeBcDriverBindingStart (
		IN EFI_DRIVER_BINDING_PROTOCOL  * This,
		IN EFI_HANDLE                   ControllerHandle,
		IN EFI_DEVICE_PATH_PROTOCOL     * RemainingDevicePath OPTIONAL
		)
{
	PXEBC_PRIVATE_DATA  *Private;
	UINTN               Index;
	EFI_STATUS          Status;
	EFI_IP4_MODE_DATA   Ip4ModeData;

	Private = AllocateZeroPool (sizeof (PXEBC_PRIVATE_DATA));
	if (Private == NULL) {
		return EFI_OUT_OF_RESOURCES;
	}

	Private->Signature                    = PXEBC_PRIVATE_DATA_SIGNATURE;
	Private->Controller                   = ControllerHandle;
	Private->Image                        = This->DriverBindingHandle;
	CopyMem (&Private->PxeBc, &mPxeBcProtocolTemplate, sizeof (Private->PxeBc));
	Private->PxeBc.Mode                   = &Private->Mode;
	CopyMem (&Private->LoadFile, &mLoadFileProtocolTemplate, sizeof (Private->LoadFile));

	Private->ProxyOffer.Packet.Offer.Size = PXEBC_CACHED_DHCP4_PACKET_MAX_SIZE;
	Private->Dhcp4Ack.Packet.Ack.Size     = PXEBC_CACHED_DHCP4_PACKET_MAX_SIZE;
	Private->PxeReply.Packet.Ack.Size     = PXEBC_CACHED_DHCP4_PACKET_MAX_SIZE;

	for (Index = 0; Index < PXEBC_MAX_OFFER_NUM; Index++) {
		Private->Dhcp4Offers[Index].Packet.Offer.Size = PXEBC_CACHED_DHCP4_PACKET_MAX_SIZE;
	}

	//
	// Get the NII interface if it exists.
	//
	Status = gBS->OpenProtocol (
			ControllerHandle,
			&gEfiNetworkInterfaceIdentifierProtocolGuid_31,
			(VOID **) &Private->Nii,
			This->DriverBindingHandle,
			ControllerHandle,
			EFI_OPEN_PROTOCOL_GET_PROTOCOL
			);
	if (EFI_ERROR (Status)) {
		Private->Nii = NULL;
	}

	Status = NetLibCreateServiceChild (
			ControllerHandle,
			This->DriverBindingHandle,
			&gEfiArpServiceBindingProtocolGuid,
			&Private->ArpChild
			);
	if (EFI_ERROR (Status)) {
		goto ON_ERROR;
	}

	Status = gBS->OpenProtocol (
			Private->ArpChild,
			&gEfiArpProtocolGuid,
			(VOID **) &Private->Arp,
			This->DriverBindingHandle,
			ControllerHandle,
			EFI_OPEN_PROTOCOL_BY_DRIVER
			);
	if (EFI_ERROR (Status)) {
		goto ON_ERROR;
	}

	Status = NetLibCreateServiceChild (
			ControllerHandle,
			This->DriverBindingHandle,
			&gEfiDhcp4ServiceBindingProtocolGuid,
			&Private->Dhcp4Child
			);
	if (EFI_ERROR (Status)) {
		goto ON_ERROR;
	}

	Status = gBS->OpenProtocol (
			Private->Dhcp4Child,
			&gEfiDhcp4ProtocolGuid,
			(VOID **) &Private->Dhcp4,
			This->DriverBindingHandle,
			ControllerHandle,
			EFI_OPEN_PROTOCOL_BY_DRIVER
			);
	if (EFI_ERROR (Status)) {
		goto ON_ERROR;
	}

	Status = NetLibCreateServiceChild (
			ControllerHandle,
			This->DriverBindingHandle,
			&gEfiIp4ServiceBindingProtocolGuid,
			&Private->Ip4Child
			);
	if (EFI_ERROR (Status)) {
		goto ON_ERROR;
	}

	Status = gBS->OpenProtocol (
			Private->Ip4Child,
			&gEfiIp4ProtocolGuid,
			(VOID **) &Private->Ip4,
			This->DriverBindingHandle,
			ControllerHandle,
			EFI_OPEN_PROTOCOL_BY_DRIVER
			);
	if (EFI_ERROR (Status)) {
		goto ON_ERROR;
	}

	//
	// Get max packet size from Ip4 to calculate block size for Tftp later.
	//
	Status = Private->Ip4->GetModeData (Private->Ip4, &Ip4ModeData, NULL, NULL);
	if (EFI_ERROR (Status)) {
		goto ON_ERROR;
	}

	Private->Ip4MaxPacketSize = Ip4ModeData.MaxPacketSize;

	Status = NetLibCreateServiceChild (
			ControllerHandle,
			This->DriverBindingHandle,
			&gEfiMtftp4ServiceBindingProtocolGuid,
			&Private->Mtftp4Child
			);

	if (EFI_ERROR (Status)) {
		goto ON_ERROR;
	}

	Status = gBS->OpenProtocol (
			Private->Mtftp4Child,
			&gEfiMtftp4ProtocolGuid,
			(VOID **) &Private->Mtftp4,
			This->DriverBindingHandle,
			ControllerHandle,
			EFI_OPEN_PROTOCOL_BY_DRIVER
			);

	if (EFI_ERROR (Status)) {
		goto ON_ERROR;
	}

	Status = NetLibCreateServiceChild (
			ControllerHandle,
			This->DriverBindingHandle,
			&gEfiUdp4ServiceBindingProtocolGuid,
			&Private->Udp4ReadChild
			);

	if (EFI_ERROR (Status)) {
		goto ON_ERROR;
	}

	//
	// The UDP instance for EfiPxeBcUdpRead
	//
	Status = gBS->OpenProtocol (
			Private->Udp4ReadChild,
			&gEfiUdp4ProtocolGuid,
			(VOID **) &Private->Udp4Read,
			This->DriverBindingHandle,
			ControllerHandle,
			EFI_OPEN_PROTOCOL_BY_DRIVER
			);

	if (EFI_ERROR (Status)) {
		goto ON_ERROR;
	}

	//
	// The UDP instance for EfiPxeBcUdpWrite
	//
	Status = NetLibCreateServiceChild (
			ControllerHandle,
			This->DriverBindingHandle,
			&gEfiUdp4ServiceBindingProtocolGuid,
			&Private->Udp4WriteChild
			);
	if (EFI_ERROR (Status)) {
		goto ON_ERROR;
	}

	Status = gBS->OpenProtocol (
			Private->Udp4WriteChild,
			&gEfiUdp4ProtocolGuid,
			(VOID **) &Private->Udp4Write,
			This->DriverBindingHandle,
			ControllerHandle,
			EFI_OPEN_PROTOCOL_BY_DRIVER
			);
	if (EFI_ERROR (Status)) {
		goto ON_ERROR;
	}
	ZeroMem (&Private->Udp4CfgData, sizeof (EFI_UDP4_CONFIG_DATA));
	Private->Udp4CfgData.AcceptBroadcast    = FALSE;
	Private->Udp4CfgData.AcceptPromiscuous  = FALSE;
	Private->Udp4CfgData.AcceptAnyPort      = TRUE;
	Private->Udp4CfgData.AllowDuplicatePort = TRUE;
	Private->Udp4CfgData.TypeOfService      = DEFAULT_ToS;
	Private->Udp4CfgData.TimeToLive         = DEFAULT_TTL;
	Private->Udp4CfgData.DoNotFragment      = FALSE;
	Private->Udp4CfgData.ReceiveTimeout     = PXEBC_DEFAULT_LIFETIME;
	Private->Udp4CfgData.UseDefaultAddress  = FALSE;

	PxeBcInitSeedPacket (&Private->SeedPacket, Private->Udp4Read);
	Private->MacLen = Private->SeedPacket.Dhcp4.Header.HwAddrLen;
	CopyMem (&Private->Mac, &Private->SeedPacket.Dhcp4.Header.ClientHwAddr[0], Private->MacLen);


	ZeroMem (&Private->Ip4ConfigData, sizeof (EFI_IP4_CONFIG_DATA));
	Private->Ip4ConfigData.DefaultProtocol   = EFI_IP_PROTO_ICMP;
	Private->Ip4ConfigData.AcceptIcmpErrors  = TRUE;
	Private->Ip4ConfigData.TypeOfService     = DEFAULT_ToS;
	Private->Ip4ConfigData.TimeToLive        = DEFAULT_TTL;
	Private->Ip4ConfigData.DoNotFragment     = FALSE;
	Private->Ip4ConfigData.RawData           = FALSE;

	Status = gBS->InstallMultipleProtocolInterfaces (
			&ControllerHandle,
			&gEfiPxeBaseCodeProtocolGuid,
			&Private->PxeBc,
			&gEfiLoadFileProtocolGuid,
			&Private->LoadFile,
			NULL
			);
	if (EFI_ERROR (Status)) {
		goto ON_ERROR;
	}

	return EFI_SUCCESS;

ON_ERROR:

	if (Private->Udp4WriteChild != NULL) {
		gBS->CloseProtocol (
				Private->Udp4WriteChild,
				&gEfiUdp4ProtocolGuid,
				This->DriverBindingHandle,
				ControllerHandle
				);
		NetLibDestroyServiceChild (
				ControllerHandle,
				This->DriverBindingHandle,
				&gEfiUdp4ServiceBindingProtocolGuid,
				Private->Udp4WriteChild
				);
	}

	if (Private->Udp4ReadChild != NULL) {
		gBS->CloseProtocol (
				Private->Udp4ReadChild,
				&gEfiUdp4ProtocolGuid,
				This->DriverBindingHandle,
				ControllerHandle
				);
		NetLibDestroyServiceChild (
				ControllerHandle,
				This->DriverBindingHandle,
				&gEfiUdp4ServiceBindingProtocolGuid,
				Private->Udp4ReadChild
				);
	}

	if (Private->Mtftp4Child != NULL) {
		gBS->CloseProtocol (
				Private->Mtftp4Child,
				&gEfiMtftp4ProtocolGuid,
				This->DriverBindingHandle,
				ControllerHandle
				);

		NetLibDestroyServiceChild (
				ControllerHandle,
				This->DriverBindingHandle,
				&gEfiMtftp4ServiceBindingProtocolGuid,
				Private->Mtftp4Child
				);
	}

	if (Private->Ip4Child != NULL) {
		gBS->CloseProtocol (
				Private->Ip4Child,
				&gEfiIp4ProtocolGuid,
				This->DriverBindingHandle,
				ControllerHandle
				);

		NetLibDestroyServiceChild (
				ControllerHandle,
				This->DriverBindingHandle,
				&gEfiIp4ServiceBindingProtocolGuid,
				Private->Ip4Child
				);
	}

	if (Private->Dhcp4Child != NULL) {
		gBS->CloseProtocol (
				Private->Dhcp4Child,
				&gEfiDhcp4ProtocolGuid,
				This->DriverBindingHandle,
				ControllerHandle
				);

		NetLibDestroyServiceChild (
				ControllerHandle,
				This->DriverBindingHandle,
				&gEfiDhcp4ServiceBindingProtocolGuid,
				Private->Dhcp4Child
				);
	}

	if (Private->ArpChild != NULL) {
		gBS->CloseProtocol (
				Private->ArpChild,
				&gEfiArpProtocolGuid,
				This->DriverBindingHandle,
				ControllerHandle
				);

		NetLibDestroyServiceChild (
				ControllerHandle,
				This->DriverBindingHandle,
				&gEfiArpServiceBindingProtocolGuid,
				Private->ArpChild
				);
	}

	FreePool (Private);

	return Status;
}


/**
  Stop this driver on ControllerHandle. This service is called by the
  EFI boot service DisconnectController(). In order to
  make drivers as small as possible, there are a few calling
  restrictions for this service. DisconnectController()
  must follow these calling restrictions. If any other agent wishes
  to call Stop() it must also follow these calling restrictions.

  @param  This              Protocol instance pointer.
  @param  ControllerHandle  Handle of device to stop driver on
  @param  NumberOfChildren  Number of Handles in ChildHandleBuffer. If number of
  children is zero stop the entire bus driver.
  @param  ChildHandleBuffer List of Child Handles to Stop.

  @retval EFI_SUCCESS       This driver is removed ControllerHandle
  @retval other             This driver was not removed from this device

 **/
EFI_STATUS
	EFIAPI
PxeBcDriverBindingStop (
		IN EFI_DRIVER_BINDING_PROTOCOL  *This,
		IN EFI_HANDLE                   ControllerHandle,
		IN UINTN                        NumberOfChildren,
		IN EFI_HANDLE                   *ChildHandleBuffer
		)
{
	PXEBC_PRIVATE_DATA          *Private;
	EFI_PXE_BASE_CODE_PROTOCOL  *PxeBc;
	EFI_HANDLE                  NicHandle;
	EFI_STATUS                  Status;

	NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiArpProtocolGuid);
	if (NicHandle == NULL) {
		NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiDhcp4ProtocolGuid);

		if (NicHandle == NULL) {
			NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiIp4ProtocolGuid);

			if (NicHandle == NULL) {
				NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiUdp4ProtocolGuid);

				if (NicHandle == NULL) {
					NicHandle = NetLibGetNicHandle (ControllerHandle, &gEfiMtftp4ProtocolGuid);

					if (NicHandle == NULL) {
						return EFI_SUCCESS;
					}
				}
			}
		}
	}

	Status = gBS->OpenProtocol (
			NicHandle,
			&gEfiPxeBaseCodeProtocolGuid,
			(VOID **) &PxeBc,
			This->DriverBindingHandle,
			ControllerHandle,
			EFI_OPEN_PROTOCOL_GET_PROTOCOL
			);

	if (EFI_ERROR (Status)) {
		return Status;
	}

	//
	// Stop functionality of PXE Base Code protocol
	//
	Status = PxeBc->Stop (PxeBc);
	if (Status != EFI_SUCCESS && Status != EFI_NOT_STARTED) {
		return Status;
	}

	Private = PXEBC_PRIVATE_DATA_FROM_PXEBC (PxeBc);

	Status = gBS->UninstallMultipleProtocolInterfaces (
			NicHandle,
			&gEfiPxeBaseCodeProtocolGuid,
			&Private->PxeBc,
			&gEfiLoadFileProtocolGuid,
			&Private->LoadFile,
			NULL
			);

	if (!EFI_ERROR (Status)) {

		gBS->CloseProtocol (
				Private->Udp4WriteChild,
				&gEfiUdp4ProtocolGuid,
				This->DriverBindingHandle,
				NicHandle
				);
		NetLibDestroyServiceChild (
				ControllerHandle,
				This->DriverBindingHandle,
				&gEfiUdp4ServiceBindingProtocolGuid,
				Private->Udp4WriteChild
				);

		gBS->CloseProtocol (
				Private->Udp4ReadChild,
				&gEfiUdp4ProtocolGuid,
				This->DriverBindingHandle,
				NicHandle
				);
		NetLibDestroyServiceChild (
				NicHandle,
				This->DriverBindingHandle,
				&gEfiUdp4ServiceBindingProtocolGuid,
				Private->Udp4ReadChild
				);

		gBS->CloseProtocol (
				Private->Dhcp4Child,
				&gEfiDhcp4ProtocolGuid,
				This->DriverBindingHandle,
				NicHandle
				);
		NetLibDestroyServiceChild (
				NicHandle,
				This->DriverBindingHandle,
				&gEfiDhcp4ServiceBindingProtocolGuid,
				Private->Dhcp4Child
				);

		gBS->CloseProtocol (
				Private->Mtftp4Child,
				&gEfiMtftp4ProtocolGuid,
				This->DriverBindingHandle,
				NicHandle
				);
		NetLibDestroyServiceChild (
				NicHandle,
				This->DriverBindingHandle,
				&gEfiMtftp4ServiceBindingProtocolGuid,
				Private->Mtftp4Child
				);

		gBS->CloseProtocol (
				Private->Ip4Child,
				&gEfiIp4ProtocolGuid,
				This->DriverBindingHandle,
				NicHandle
				);
		NetLibDestroyServiceChild (
				NicHandle,
				This->DriverBindingHandle,
				&gEfiIp4ServiceBindingProtocolGuid,
				Private->Ip4Child
				);

		gBS->CloseProtocol (
				Private->ArpChild,
				&gEfiArpProtocolGuid,
				This->DriverBindingHandle,
				NicHandle
				);
		NetLibDestroyServiceChild (
				NicHandle,
				This->DriverBindingHandle,
				&gEfiArpServiceBindingProtocolGuid,
				Private->ArpChild
				);

		FreePool (Private);
	}

	return Status;
}


