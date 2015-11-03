@echo off
rem This batchfile should work on any MS operating system. 
rem Necessary shell is also provided if only MS COMMAND.COM is available.
if "%OS%"=="Windows_NT" goto settings
rem support for OS's without FreeCOM loaded as default shell. Load FreeCOM, and run.
if "%_CWD%"=="" buildcd\command.com /C %0
if not "%_CWD%"=="" goto settings
pause bigtime error! Abort!
exit

:settings
rem Win2000-only: switch to directory containing %0. Allows '\CDROM\ISOLINUX\MAKEISO.BAT'
rem a 'CD "full-qualified filename" would also work, if allowed, that is. CD C:\CONFIG.SYS for example
for %%I in ( %0 ) do CD /D %%~pI
set isofile=fdbootcd.iso
cls
if not exist makeiso.bat goto wrongdir
cd..
if "%OS%"=="Windows_NT" set basedir=%CD%
if not "%_CWD%"=="" set basedir=%_CWD%
if not exist %basedir%nul cd..
if not exist %basedir%nul set basedir=%basedir%\

rem When running makeiso.bat from cdrom you need to store it elsewhere.
rem Needed in case %isofile% or the drive it's on is readonly (cdrom, or C:\FDBOOTCD.ISO mounted by Daemon Tools)
if not exist %temp%\nul set temp=c:
if not "%OS%"=="Windows_NT" goto find_exe
rem ignore \storeiso.cmd on all but NT/2000/XP. It overrides %isofile% to NTFS partition directory
if exist \storeiso.cmd call \storeiso.cmd
goto find_exe

:find_exe 
rem search the path for mkisofs 2.01 executable. thus allow to exclude it from the compilation.
if exist %basedir%isolinux\buildcd\mkisofs.exe set mkisoexe=%basedir%isolinux\buildcd\mkisofs.exe
if "%mkisoexe%"=="" for %%d in (%path%) do if exist %%d\mkisofs.exe set mkisoexe=%%d\mkisofs.exe
if "%mkisoexe%"=="" for %%d in (%path%) do if exist %%dmkisofs.exe set mkisoexe=%%dmkisofs.exe
if "%mkisoexe%"=="" goto notool
goto make_ISO

:make_ISO
for %%x in ( %CD% %_CWD% ) do if not "%%x"=="" set currdir=%%x
if "%currdir%%isofile%"=="%basedir%%isofile%" del %basedir%%isofile%
cls
set msg=Creating cdrom imagefile %isofile% out of directory %basedir%
for %%x in ( title cls echo ) do %%x %msg%


if not "%isofile%"=="fdbootcd.iso" set params2=-force-uppercase
%mkisoexe% @%BASEDIR%isolinux\buildcd\params.txt %params2% -o %isofile% %BASEDIR%
if "%errorlevel%"=="22" set isofile=%temp%\fdbootcd.iso
if "%errorlevel%"=="22" %mkisoexe% @%BASEDIR%isolinux\buildcd\params.txt %isofile% %BASEDIR%
if "%errorlevel%"=="0" set msg=Created cdrom imagefile %isofile% out of directory %basedir%
if "%errorlevel%"=="0" goto end
set msg=Error %errorlevel% :
if "%errorlevel%"=="65535" set msg=%msg% Rename isolinux directory and isolinux.bin all lowercase!
if "%errorlevel%"=="22" set msg=%msg% Could not create ISO-image due to it or its drive being read-only
if "%errorlevel%"=="13" set msg=%msg% Please remove read-only attribute from file isolinux.bin 
if exist %isofile% del %isofile%
goto end

:wrongdir
echo Error: CDROM creation program was called from a wrong directory.
echo please switch to the directory where %0 is located, and run %0 again.
goto finalend

:notool
echo Error: the program MKISOFS.EXE was not found in your search path.
echo        It can be downloaded from http://cdrtools.bootcd.ru/
echo        (Official version: http://www.wolfgang-brinkmann.de/bcdw/index_e.htm )
echo        please place the program in one of the following directories (path):
for %%x in (%path%) do echo %%x
goto finalend

:end
for %%x in ( title cls echo ) do %%x %msg%
goto finalend
:finalend
rem skip pause in case of running from FreeDOS (Freecom loaded, Config.sys menu entry selected)
for %%x in ( 0 1 2 3 4 5 6 7 8 9 ) do if "%config%"=="%%x" goto real_end
pause
goto real_end
:real_end
