FreeDOS 1.0 Final (2006-08-11)

Readme file

Table of Contents:
---------------------------------

I)    How to view this document
II)   What to get, platform dependant
III)  How to boot up
IV)   Installation
V)    Post-installation disk/RAM layout and configuration files
VI)   Using FreeDOS
VII)  Updates and help

---------------------------------

I      - How to view this document

This document is posted online and thus may incidentally not comply with the 
DOS specification of keeping line width below 80 characters. We recommend 
using a HTML viewer or the FreeDOS program PG, which supports line wrapping,
found at http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/util/file/pg/

---------------------------------

II     - What to get, platform dependant

The FreeDOS distribution is available in several ways. Users only wanting to 
use the kernel don't need to download the entire distribution. Owners of a 
computer older than 80386 often don't have a cdrom drive in their systems.
FreeDOS 1.0 Final is offered for:

Platform:            what to download:
---------------------------------------------------------------------
8086/80186/80286 : - download ODIN (One Disk INstaller)
                     odin.fdos.org

80386 or newer   : - download the bootable freedos installer cdrom
                     http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/distributions/1.0/fd??????.iso
                     
                     If you cannot boot from cdrom, please download bootdisk
                     http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/distributions/1.0/fdos1440.img

                     The bootdisk can access the FreeDOS cdrom or an ISO
                     version of it placed on harddisk, (C:\FDBOOTCD.ISO), in
                     order to continue the normal installation process.

You may need to write the bootdisk image to a diskette using one of several
available programs found at: http://www.fdos.org/ripcord/rawrite/
(RAwrite downloadable from :
http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/distributions/old/beta9sr1/rawrite.exe )
If the target diskette contains information, it will be erased when writing
the image contents to the diskette.

The .iso file (ISO9660 filesystem) can be written to a blank recordable medium 
[CD-R(W) or DVD+/-R(W)] using a cd writing program that can write a disk image.
Most used is Ahead's Nero Burning Rom, and the Unix/Linux program CDrecord
Make sure contents of the fd??????.iso file is stored on the written medium,
not the file itself. In other words, you should see a file called autorun.inf 
and the directories called FREEDOS and ISOLINUX, and *not* a file with the
extension ISO (example: FDBOOTCD.ISO).  In Nero, you may select to burn the
CD-ROM using an image file, not simply burining the ISO file onto the drive.

---------------------------------

III    - How to boot up

Virtually all the computer systems that FreeDOS can be installed on allow
booting from another medium than the harddisk. This is required for installing
FreeDOS to your harddisk.
Old computers support diskette booting, recent computers also support starting
up from cdrom (the so called El Torito standard dating from 1995)

If you cannot boot from cdrom or do not know how to set up your BIOS
settings (most systems use DEL, ESC, F2 or F10 keys to access the BIOS) 
to allow booting from cdrom, please boot from diskette.

If you are able to get your system to allow booting from cdrom, that's the 
easiest and best way of installing FreeDOS. Type "ENTER" at the BOOT: prompt and
then you'll see FreeDOS starting. You should now see another menu. Select
option 1 to load memory and cdrom drivers. the CDROM is accessed and then you
can select what to do next (Install FreeDOS, Run FreeDOS, Create bootdisk), 
but more about that later. First the floppy explanation.

For booting from diskette, insert the diskette to which you have written the
contents of FDOS1440.IMG using WinImage or Rawrite for example.

You should now see a boot menu with 2 options:
*Select 1 (recommended)
 Boot FreeDOS and begin the installation process after locating the FreeDOS
 CD-ROM or ISO file (at C:\FDBOOTCD.ISO).
*Select 2 (safe mode)
 Boot a bare FreeDOS bootdisk without any drivers loaded.

Working with FreeDOS: 
-if the cdrom is found, you'll be presented with a series of menus to guide
 you through the installation process..
-if the cdrom is not found, you'll be presented with an error message
 (cdrom driver not loaded, cdrom not found).

Installation through using the harddisk (an ISO file) is the fastest and most
stable way of installing FreeDOS. As we allow dualbooting with popular
operating systems, you might want to place the FreeDOS ISO on your harddisk
first to experience a smooth installation process.

see chapter IV - Installation for further info.

---------------------------------

IV     - Installation

Regardless of which way you booted, installing FreeDOS is made up of several
steps. If you want (or need) to change your disk layout (using a partitioning
program) , you will need to reboot at least once in this process.

These are the steps you need to follow:
1) Partition your harddisk (not recommended unless necessary!)
2) Create a filesystem     (not recommended unless necessary!)
3) Install and configure FreeDOS core files and additional files.

Step1: (not recommended unless necessary!)
You can change your disk layout by using any partition program.
most known ones are Fdisk ("program for Fixed Disks") and the Partition Magic
product from the company Symantec (was PowerQuest Partition Magic).
Other often used programs are FIPS (DOS, freeware) and PARTED (Linux, GPL).
A program called NTFSRESIZE might also be usefull if your harddisk was 
partitioned as a single primary partition of the NTFS filesystem type.
The options usually given to the user for partitioning consist of XFdisk
(recommended), SPFdisk (recommended, but 386+ only), and Fdisk (MS-like
interface, but has been known to have bugs).

DOS *requires* a primary partition with the FAT filesystem that it recognizes
as C:.  Having a C: partition under your modern Windows system using NTFS is
not suitable, as DOS won't recognize that partition, let alone assign a
driveletter (C:) to it!

KNOW WHAT YOU ARE DOING - ALTERING DISK PARTITION LAYOUT CAN CAUSE DATA LOSS

FreeDOS provides its own version of FDISK, called FreeFdisk.
Here's what you need to do (if using FreeFdisk):
*Answer yes to the first question (do you want support for FAT32 (big disks?))
*Create some free (actually unpartitioned) disk space by making sure
 not all disk space is allocated.
*Create a primary partition in the unallocated space, at least 20 MegaBytes
 (option 1, then option 1), though we recommend at least 40MB
*Set it active (option 2 from the main menu)
*Exit FDISK. You will probably have to reboot. Be sure to boot from the same
 boot device again (diskette/cdrom)

The other Fdisk variants available have similar interfaces, and integrated
help.

Step2: (not recommended unless necessary)
Once an (active primary) FAT partition exists, FreeDOS detects this and
assigns a driveletter to it (starting with C: for the first FAT partition on
the first physical harddisk).
However it still allows to change disk layout by calling the supplied FDISK.
and offers you to format the partition. If the partition was not yet formatted
then FreeDOS auto-formats (after the user confirms) it. If it was formatted,
we recommend not to format the partition again, as it may contain programs and
data you wish to keep (creating the FAT [File Allocation Table] filesystem
using the FORMAT program erases all previous contents)


Step3:
Once the active primary partition actually contains a filesystem, be it
already present or just created through the FORMAT program, it will allow
you to install FreeDOS using the installation program.
The installation program is intuitive to use, though you should type in
a pathname (C:\FDOS for example) instead of a drive/driveletter (C:).
If you specify the latter, all files end up in the root of the drive,
instead of its own directory.
We recommend to opt for a destination directory on drive C:, to have
the FreeDOS system files and the rest of the files on one single partition.
Be aware that DOS numbers partitions in a different order than other 
operating systems. Also only FAT(12/16/32) filesystems are recognized and
accessible through an assigned driveletter, not NTFS/HPFS/EXT2, etc..
If you choose to install to a drive other than C:, you may have to configure
your boot manager manually to make it able to boot (sometimes involving
hiding/switching partitions).

All selected files are copied to the selected installation directory, 
the core components installed (kernel - c:\kernel.sys ; shell - c:\command.com)
and several startup files are created. See following chapter for that.

All is done now, you have installed FreeDOS to harddisk and can start using it.
We recommend to reboot your system though for best use. Also remove any
installation discs to make sure you boot from harddisk.

If booting from harddisk fails, then a critical piece of program code called
the 'boot sector' was not (properly) written to harddisk.
In that case, boot from your installation disc, select a clean boot,
and enter the following command (followed by pressing ENTER):
A:\FREEDOS\SYS C: C: /BOOTONLY

---------------------------------

V      - Post-installation disk/RAM layout and configuration files

This chapter references the layout of both files and memory layout,
on diskette, cdrom and the harddisk installed FreeDOS distribution.

Feel free to skip this chapter immediately or after the harddisk 
disk layout and conventional memory layout.

* Memory layout (below 1MB) when maximizing free conventional memory,
  using the FreeDOS EMM386 driver. Layout obtained from "MEM /C /P"

  Name           Total           Conventional       Upper Memory
  --------  ----------------   ----------------   ----------------
  SYSTEM      16,192   (16K)      9,888   (10K)      6,304    (6K)
  HIMEM        2,480    (2K)      2,480    (2K)          0    (0K)
  EMM386       3,200    (3K)      3,200    (3K)          0    (0K)
  COMMAND      3,264    (3K)      2,944    (3K)        320    (0K)
  DISPLAY     11,648   (11K)          0    (0K)     11,648   (11K)
  KEYB         1,536    (2K)          0    (0K)      1,536    (2K)
  MOUSE        3,328    (3K)          0    (0K)      3,328    (3K)
  Free       693,296  (677K)    634,512  (620K)     58,784   (57K)


* Harddisk layout for a fresh installed FreeDOS distribution:

C:\                  Root of active FreeDOS FAT partition
| AUTOEXEC.BAT       Bootup script, contents see below.
| COMMAND.COM        Shell (FreeCOM), translated if possible
| CONFIG.SYS         FreeDOS configuration file
| FREEDOS.BSS        Bootsector of drive C: stored in file
| KERNEL.SYS         FreeDOS kernel, version 2.0.35 or newer
|
\--FDOS              FreeDOS directory
|   +--TEMP          Directory for storing temporary files
|   +--APPINFO       Contains the Linux Software Maps (LSM files). See below
|   +--BIN           Contains FreeDOS programs and drivers
|   |  +--CTMOUSE    CuteMouse available in several languages
|   +--DOC           Contains documentation for each program, in own directory
|   +--HELP          Stores the help-files for freedos support program
|   |  \--HHSTNDRD   called HELP. Files support a subset of the HTML format
|   +--NLS           contains localized help text for some programs
|   \--CPI           International CodePages for FreeDOS, in compressed format

Many DOS operating systems use the files c:\autoexec.bat and c:\config.sys
Therefore FreeDOS will, if another DOS-based operating system is detected,
use other system files instead:
c:\fdconfig.sys instead of c:\config.sys
fdauto.bat in your FreeDOS installation directory, instead of c:\autoexec.bat

If FreeDOS SETUP detected WindowsNT/2K/XP/2K3, then C:\BOOT.INI will contain
a menu entry to startup FreeDOS.
If a DOS based operating system was detected, then file METAKERN.SYS will be
in your root directory of C:. It contains the entry for booting your other
DOS, and an entry to boot FreeDOS.

The LSM files can be found through http://www.freedos.org/freedos/software/
example: http://www.freedos.org/freedos/software/lsm.cgi?q=v&a=base/kernel.lsm

---------------------------------

VI      - Using FreeDOS

We must assume you are familiar with the DOS operating system.
This README document is not the place to explain that.

You can get help in the following ways:
*HELP
*programname /?
*HELP programname
*FASTHELP programname
*read documents located in the DOC subdirectory of your installation directory

if that's not enough to help you, feel free to submit your problem or question
to the FreeDOS user's list. See end of this document for the email address.


---------------------------------
VII    - Updates and help

-If you would like to see what software parts need improvements,
 have a look at http://wiki.fdos.org/Main/Post_1_0_Todo

-A newer FreeDOS distribution may be found at www.freedos.org/

-Updated software is available from http://www.freedos.org/freedos/software/

-The FreeDOS kernel may be updated by replacing C:\KERNEL.SYS with a newer
 version, which can be obtained from:
 [official release] http://sourceforge.net/project/showfiles.php?group_id=5109
 [weekly from CVS ] http://fdos.org/bootdisks/ 
 [daily from CVS  ] http://fdos.org/kernel kwc8632.dev.zip

or compiled (use OpenWatCom 1.3 for example) from sources:
[daily CVS source ] www.fdos.org/kernel ( kernel.UNSTABLE.zip )


-Bugs may be reported at the FreeDOS bug database
 [ http://www.freedos.org/freedos/bugs/ ], or at the developer's mailinglist
 [ read: http://sourceforge.net/mailarchive/forum.php?forum=freedos-devel ]
 [ subscribe to: http://www.freedos.org/freedos/lists/  ]
 [ send message: freedos-devel@lists.sourceforge.net ]

 for general help please consult the FreeDOS user mailinglist,
 [ read: http://sourceforge.net/mailarchive/forum.php?forum=freedos-user ]
 [ subscribe to: http://www.freedos.org/freedos/lists/  ]
 [ send message: freedos-user@lists.sourceforge.net ]


IRC chat is also provided (irc.i7c.org , channels #fd-user and #freedos ),
use a IRC client (Mozilla, mIRC, etc) to connect to the IRC chat.

thank you for reading this lengthy document, and enjoy FreeDOS :)

Bernd Blaauw,
2004-11-30

Blair Campbell,
2006-08-11
