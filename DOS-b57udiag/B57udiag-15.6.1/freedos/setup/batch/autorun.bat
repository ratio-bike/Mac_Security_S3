@echo off
set /E FC=echo hello world
if not "%FC%"=="" goto init
if not "%_CWD%"=="" goto init
echo Fatal error: FreeDOS-specific shell required (FreeCOM v0.84pre2)
exit
goto end

:init
set FC=
set drivesrc=
set validsrc=
if not "%fdosroot%"=="" CDD %fdosroot%\batch
if not exist autorun.bat goto errordir
pushd
cd \freedos
set cdrom=%_CWD%
set fdosroot=%cdrom%\setup
popd
rem define your own location for the freedos bootdisk FREEDOS directory
set path=%fdosroot%\odin;%fdosroot%\install
cls
for %%x in ( [%lang%] [%key%] ) do if %%x==[] goto defaults

:optmenu1
call %fdosroot%\batch\menu1.bat
echo.
if "%menu1_nr%"=="INSTFDOS" goto optmenu2
if "%menu1_nr%"=="BOOTDISK" goto bootdisk
if "%menu1_nr%"=="QUIT" LOCALIZE 0.17
if "%menu1_nr%"=="QUIT" goto end
if "%menu1_nr%"=="VIEW" PG \freedos\docs\install3.txt
if "%menu1_nr%"=="REBOOT" FDAPM WARMBOOT
if "%menu1_nr%"=="XFDISK" XFDISK
if "%menu1_nr%"=="SPFDISK" SPFDISK
if "%menu1_nr%"=="FDISK" FDISK
if "%menu1_nr%"=="FORMAT" Format %destdrv% /v:FreeDOS
if "%menu1_nr%"=="FORMAT" pause
goto optmenu1

:bootdisk
if !%nlspath%==! set nlspath=a:\freedos
set oldnls=%nlspath%
set nlspath=a:\freedos
localize 0.4 @@@ This creates a FreeDOS boot disk on your first floppy drive
localize 0.5 @@@ Please insert a formatted floppy disk into that drive
set nlspath=%oldnls%
pause
if not !%initrd%==! set destdsk=b:
if !%initrd%==! set destdsk=a:
%cddrv%FreeDOS\3rdParty\extract %cddrv%ISOLinux\Data\FDBoot.img -x x:%destdsk%\
sys %destdsk% /BOOTONLY
pause

:defaults
alias menu=%fdosroot%\batch\autorun.bat
set nlspath=%fdosroot%\nls
set helppath=%fdosroot%\odin\help
if "%lang%"=="" set lang=EN
if "%cputype%"=="" set cputype=80386
for %%x in ( 8086 80186 80286 ) do if "%cputype%"=="%%x" set color=Mono
if not "%display%"=="yes" set display=
set nlspath=%fdosroot%\nls
goto optmenu1

:optmenu2
set menu2_nr=
call %fdosroot%\batch\menu2.bat
CLS
if "%menu2_nr%"=="QUIT"     goto optmenu1
if "%menu2_nr%"=="SETKEYB"  goto sel_keyb
if "%menu2_nr%"=="INSTFDOS" goto installr
if "%menu2_nr%"=="SETLANG"  goto sel_lang
if "%menu2_nr%"=="SETSCRN"  goto sel_scrn
if "%menu2_nr%"=="SETSIZE"  goto sel_mode
goto optmenu2

:installr
call %fdosroot%\batch\textmenu.bat
if "%TEXTMENU%"=="QUIT" goto optmenu2
rem Do not CALL this, but let it be the end-option.
rem But what about the INSTALLER not being run? how to solve that?
if exist postinst.bat postinst.bat
goto end

:sel_lang
set LANG=EN
cls
goto optmenu2

:sel_scrn
if !%lang%==! set lang=EN
set oldlang=%lang%
if not exist %nlspath%\color.%lang% set lang=EN
cls
pbatch %nlspath%\backgrou.men -fh7 -ft7 -x1 -y1 -r0 -s0
pbatch %nlspath%\title.%lang% -fh1 -ft15 -x2 -y2
pbatch %nlspath%\color.%lang% -fh1 -ft15 -m1
cls
if !%errorlevel%==!1 set color=Color
if !%errorlevel%==!2 set color=Mono
set lang=%oldlang%
goto optmenu2

:sel_keyb
cls
pbatch %nlspath%\backgrou.men -fh7 -ft7 -x1 -y1 -r0 -s0
pbatch %nlspath%\lang.txt -fh1 -ft15 -x2 -y2
pbatch %nlspath%\langsel.txt -fh1 -ft15 -m1
cls
set choice=%errorlevel%
if !%choice%==!1 call %fdosroot%\batch\regional.bat 21
if !%choice%==!2 call %fdosroot%\batch\regional.bat 10
if !%choice%==!3 call %fdosroot%\batch\regional.bat 8
if !%choice%==!4 call %fdosroot%\batch\regional.bat 56
if !%choice%==!5 call %fdosroot%\batch\regional.bat 34
if !%choice%==!6 call %fdosroot%\batch\regional.bat 12
if !%choice%==!7 call %fdosroot%\batch\regional.bat 18
if !%choice%==!8 call %fdosroot%\batch\regional.bat 36
if !%choice%==!9 call %fdosroot%\batch\regional.bat 17
if !%choice%==!10 call %fdosroot%\batch\regional.bat 37
if not !%choice%==!11 goto optmenu2
type %fdosroot%\language\regional.txt
echo.
echo.
echo Please enter the number corresponding to the name which best describes
set /P choice=your current location (confirm input by pressing ENTER):
if not "%choice%"=="" call %fdosroot%\batch\regional.bat %choice%
goto optmenu2

:sel_mode
if !%lang%==! set lang=EN
set oldlang=%lang%
if not exist %nlspath%\instmode.%lang% set lang=EN
pbatch %nlspath%\backgrou.men -fh7 -ft7 -x1 -y1 -r0 -s0
pbatch %nlspath%\title.%lang% -fh1 -ft15 -x2 -y2
pbatch %nlspath%\instmode.%lang% -fh1 -ft15 -m1
cls
if !%errorlevel%==!1 set instmode=Full
if !%errorlevel%==!2 set instmode=Mini
if !%errorlevel%==!3 set instmode=Maxi
set lang=%oldlang%
goto optmenu2

:errordir
echo You are in the wrong directory..
echo file AUTORUN.BAT needs to exist!
goto end

:end
