@echo off
cls
rem %config%: DOS
rem %errorlevel%: NT, FreeCOM
rem %CD%: NT
rem %_CWD%: FreeCOM, 4DOS.
rem Check for memdisk existence, errorlevel 1=found ; 0=notfound
set dest=B:
if "%OS%"=="Windows_NT" goto begin
if not exist a:\freedos\ifmemdsk.com goto begin
a:\freedos\ifmemdsk.com 
if errorlevel 3 goto begin
if not errorlevel 1 set dest=A:
goto begin

:begin
title FreeDOS Bootdisk Builder
cls
echo Welcome to the bootdisk builder. this will create a bootdisk for you.
echo.
if not "%OS%"=="Windows_NT" goto begin2
echo Please insert a (non)empty formatted diskette in diskdrive and press a key to continue
pause
CD /D %dest%
goto emptydsk

:begin2
if not "%cdrom%"=="" CDD %cdrom%
cls
if "%dest%"=="B:" echo Please insert a (non)empty formatted diskette in your diskette drive.
if "%dest%"=="A:" echo please remove bootdisk and insert a (non)empty formatted diskette in your diskette drive
goto emptydsk

:emptydsk
pause Press a key to continue or Control-C to abort.
CDD %dest%
rem kernel files and cdromdrivers (as long as atapicdd.sys isn't performing good/stable enough)
for %%x in ( io.sys msdos.sys oakcdrom.sys ) do if exist %%x attrib +r %%x
for %%x in ( \*.* ) do del %%x
rem all files in root removed, except for kernel files.
goto docopy

:docopy
if "%dest%"=="A:" \freedos\3rdparty\extract -ox \isolinux\data\fdboot.img %dest% *.* 
if "%dest%"=="B:" xcopy /s /y A:\*.* %dest%
rem files are now present on diskette!
rem setup stable cdromdriver, delete recreatable files
if exist %dest%\oakcdrom.sys copy /y %dest%\oakcdrom.sys %dest%\driver\vide-cdd.sys
if exist %dest%\metakern.sys del %dest%\metakern.sys
if exist %dest%\freedos\bootsect.bin del %dest%\freedos\bootsect.bin
set kernel=kernel.sys

rem Assuming non-freedos bootsector currently installed on diskette
%dest%\freedos\oscheck.com %dest% %dest%\freedos\bootsect.bin
if not exist %dest%\bootsect.bin goto end
if not exist %dest%\freedos\meta-all.bin goto end
if not exist %dest%\freedos\metaboot.bot goto end
copy /b %dest%\freedos\meta-all.bin + %dest%\freedos\metaboot.bot + %dest%\bootsect.bin %dest%\metakern.sys
dir %dest%
pause
if exist %dest%\metakern.sys set kernel=metakern.sys
echo SHELL=A:\COMMAND.COM A:\ /E:1024 /D /K set path=a:\freedos> %dest%\config.sys
goto end

:end
echo Kernel file: %kernel%
%dest%\freedos\SYS %dest% %dest% BOOTONLY /K %kernel%
set kernel=
goto end2
:end2
