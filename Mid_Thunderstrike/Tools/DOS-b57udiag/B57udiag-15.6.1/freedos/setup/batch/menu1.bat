@echo off
set oldlang=%lang%
if "%lang%"=="" set lang=EN
set oldlang=%lang%
if not exist %nlspath%\menu1.%lang% set lang=EN
cls
pbatch %nlspath%\backgrou.men -fh7 -ft7 -x1 -y1 -r0 -s0
pbatch %nlspath%\title.%lang% -fh1 -ft15 -x2 -y2
pbatch %nlspath%\menu1.%lang% -fh1 -ft15 -m1
cls
if errorlevel 1 set menu1_nr=INSTFDOS
if errorlevel 2 set menu1_nr=VIEW
if errorlevel 3 set menu1_nr=FORMAT
if errorlevel 4 set menu1_nr=XFDISK
if errorlevel 5 set menu1_nr=QUIT
if errorlevel 6 set menu1_nr=BOOTDISK
if errorlevel 7 set menu1_nr=REBOOT
set lang=%oldlang%
