@echo off
if exist \freedos\packages\src_game.1 goto setlargest
if exist \freedos\packages\games.1 goto setlarge
if exist \freedos\packages\src_base.1 goto setsmall
set INSTMAX=INSTALL.MIN
set INSTDAT=INSTALL.MIN
set INSTMIN=INSTALL.MIN
goto first

:setlargest
set INSTMAX=INSTALL.MAX
set INSTDAT=INSTALL.DAT
set INSTMIN=INSTALL.MIN
goto first

:setlarge
set INSTMAX=INSTALL.CUS
set INSTDAT=INSTALL.LRG
set INSTMIN=INSTALL.MIN
goto first

:setsmall
set INSTMAX=INSTALL.SML
set INSTDAT=INSTALL.SML
set INSTMIN=INSTALL.MIN

:first
:start
CDD %destdrv%\
:ready
if exist %destdrv%\bootsect.bin del %destdrv%\bootsect.bin
set opt=install
goto setupDOS

:quit
SET TEXTMENU=QUIT
goto end

:begin
for %%x in ( 0 1 2 3 ) do localize 2.%%x
for %%x in ( 4 5 ) do if not "%opt%"=="fdisk" localize 2.%%x
for %%x in ( install format ) do if "%opt%"=="%%x" localize 2.6
for %%x in ( 7 8 9 10 ) do localize 2.%%x

:setupDOS

:defaults
cpulevel > NUL
set cputype=80%errorlevel%86
if "%cputype%"=="80086" set cputype=80186
if "%lang%"=="" set lang=EN
if "%color%"=="" set color=color
if not "%color%"=="" goto defal
goto defaults
:defal
if "%instmode%"=="" set instmode=Full
goto options

:options
CDD %fdosroot%\install\
set instparm=/src %cdrom%\PACKAGES /dst %destdrv%\FDOS /DF
if "%color%"=="Mono" set instparm=/mono %instparm%
if "%instmode%"=="Maxi" set instparm=%instparm% %_CWD%\%INSTMAX%
if "%instmode%"=="Full" set instparm=%instparm% %_CWD%\%INSTDAT%
if "%instmode%"=="Mini" set instparm=%instparm% %_CWD%\%INSTMIN%
if "%os%"=="37"   iniadd C:\BOOT.INI FREEDOS.BSS %destdrv%\FREEDOS.BSS="FreeDOS"
pushd
cdd %fdosroot%\odin
if not exist %destdrv%\FREEDOS.BSS SYS %destdrv% %destdrv% %destdrv%\FREEDOS.BSS >NUL
popd
if "%os%"=="37" goto skipsys
goto setup

:setup
if exist %destdrv%\fdconfig.sys ren %destdrv%\fdconfig.sys fdconfig.old
if exist %destdrv%\fdauto.bat ren %destdrv%\fdauto.bat fdauto.old
pushd
cdd %fdosroot%\odin
if not exist %destdrv%\metakern.sys SYS %destdrv%>NUL
popd
if exist %destdrv%\metakern.sys SYS %destdrv% /BOOTONLY /K metakern.sys>NUL
:skipsys
xmssize 5 >NUL
set xms=%errorlevel%
if errorlevel 31 set xms=30
for %%x in ( Mono ) do if "%color%"=="%%x" goto setuptxt
for %%x in ( 8086 80186 80286 80386 80486 ) do if "%cputype%"=="%%x" goto setuptxt
goto setupTXT

:setupTXT
for %%x in ( 4 5 6 ) do if "%cputype%"=="80%%x86" set cputype=80386
textinst.exe %instparm%
if not "%errorlevel%"=="0" pause Warning: error %errorlevel% occurred! Press a key to continue
goto end

:end
