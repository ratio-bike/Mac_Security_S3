@echo off
if "%lang%"=="" set lang=EN
set oldlang=%lang%
if not exist %destdrv%\nul goto end
if not exist %nlspath%\menu2.%lang% set lang=EN
set menu2_nr=INSTFDOS
cls
pbatch %nlspath%\backgrou.men -fh7 -ft7 -x1 -y1 -r0 -s0
pbatch %nlspath%\title.%lang% -fh1 -ft15 -x2 -y2
pbatch %nlspath%\menu2.%lang% -fh1 -ft15 -m1
cls
if !%errorlevel%==!1 set menu2_nr=INSTFDOS
if !%errorlevel%==!2 set menu2_nr=SETSIZE
if !%errorlevel%==!3 set menu2_nr=SETLANG
if !%errorlevel%==!4 set menu2_nr=QUIT
set lang=%oldlang%
goto end

:end
rem Fdisk and then Quit must lead to menu 1
if "%TEXTMENU%"=="QUIT" set menu2_nr=QUIT
set TEXTMENU=

