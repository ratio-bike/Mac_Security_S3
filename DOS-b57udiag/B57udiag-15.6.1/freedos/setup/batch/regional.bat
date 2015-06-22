@echo off
if "%1"=="setNLS" goto DoNLS
rem called by AUTORUN.BAT as: "regional.bat 21" , then 21 is looked up,
rem and regional.bat is called again, this time as 
rem "regional.bat setNLS HU 348 852 EGA  hu hu208.kl /id:208"
rem then we get: language=HU; country_ID=348 ; codepage=852, display table=EGA
rem and finally: keyboard layout = Hungary with some special layout


if "%1"== "1" call %0 setNLS  51 HY   899 EGA6 HY
if "%1"== "2" call %0 setNLS 112 BE   849 EGA4 BL
if "%1"== "3" call %0 setNLS  56 NL   858 EGA  BE
if "%1"== "4" call %0 setNLS 191 HR   852 EGA  HR
if "%1"== "5" call %0 setNLS 208 DA   858 EGA  DK
if "%1"== "6" call %0 setNLS 233 ET   775 EGA2 ET
if "%1"== "7" call %0 setNLS 246 FI   858 EGA  SU
if "%1"== "8" call %0 setNLS 250 FR   858 EGA  FR
if "%1"== "9" call %0 setNLS 268 KA 60853 EGA6 KA
if "%1"=="10" call %0 setNLS 276 DE   858 EGA  GR

if "%1"=="11" call %0 setNLS 372 GA   858 EGA  IR
if "%1"=="12" call %0 setNLS 380 IT   858 EGA  IT
if "%1"=="13" call %0 setNLS 392 JA   932 ---  JP
if "%1"=="14" call %0 setNLS 428 LV   775 EGA2 LV
if "%1"=="15" call %0 setNLS 578 NO   858 EGA  NO
if "%1"=="16" call %0 setNLS 642 RO   852 EGA  RO
if "%1"=="17" call %0 setNLS 724 ES   858 EGA  SP
if "%1"=="18" call %0 setNLS 752 SV   858 EGA  SV
if "%1"=="19" call %0 setNLS 792 TR   857 EGA  TR TR440.KL /ID:440
if "%1"=="20" call %0 setNLS 826 EN   858 EGA  UK

if "%1"=="21" call %0 setNLS 840 EN   858 EGA  US
if "%1"=="22" call %0 setNLS  31 AZ 60258 EGA7 AZ
if "%1"=="23" call %0 setNLS  76 PT   858 EGA  BR
if "%1"=="24" call %0 setNLS  76 PT   858 EGA  BR BR274.KL /ID:274
if "%1"=="25" call %0 setNLS 124 FR   858 EGA  CF
if "%1"=="26" call %0 setNLS 203 CS   852 EGA  CZ CZ243.KL /ID:243
if "%1"=="27" call %0 setNLS 234 FO   858 EGA  FO
if "%1"=="28" call %0 setNLS 300 EL   869 EGA5 GK GK220.KL /ID:220
if "%1"=="29" call %0 setNLS 300 EL   869 EGA5 GK
if "%1"=="30" call %0 setNLS 398 KK 58152 EGA7 KK

if "%1"=="31" call %0 setNLS 417 KY 58152 EGA7 KY
if "%1"=="32" call %0 setNLS --- ES   858 EGA  LA
if "%1"=="33" call %0 setNLS 496 MN 58152 EGA7 MN
if "%1"=="34" call %0 setNLS 528 NL   858 EGA  US
if "%1"=="35" call %0 setNLS 608 TL   858 EGA  TL
if "%1"=="36" call %0 setNLS 620 PT   858 EGA  PO
if "%1"=="37" call %0 setNLS 643 RU   808 EGA3 RU
if "%1"=="38" call %0 setNLS 703 SK   852 EGA  SK
if "%1"=="39" call %0 setNLS 705 SL   852 EGA  SI
if "%1"=="40" call %0 setNLS 840 EN   850 EGA  DV

if "%1"=="41" call %0 setNLS 840 EN   858 EGA  UX
if "%1"=="42" call %0 setNLS 860 UZ 62306 EGA7 UZ
if "%1"=="43" call %0 setNLS  70 BS   852 EGA  BA
if "%1"=="44" call %0 setNLS 100 BG   872 EGA3 BG
if "%1"=="45" call %0 setNLS 100 BG   872 EGA3 BG BG241.KL /ID:241
if "%1"=="46" call %0 setNLS 124 FR   858 EGA  CF CF501.KL /ID:501
if "%1"=="47" call %0 setNLS 300 EL   869 EGA5 GK GK459.KL /ID:459
if "%1"=="48" call %0 setNLS 348 HU   852 EGA  HU
if "%1"=="49" call %0 setNLS 348 HU   852 EGA  HU HU208.KL /ID:208
if "%1"=="50" call %0 setNLS 352 IS   858 EGA  IS

if "%1"=="51" call %0 setNLS 352 IS   858 EGA  IS IS161.KL /ID:161
if "%1"=="52" call %0 setNLS 440 LT   775 EGA2 LT LT210.KL /ID:210
if "%1"=="53" call %0 setNLS 440 LT   775 EGA2 LT
if "%1"=="54" call %0 setNLS 470 MT   853 EGA  MT MT166.KL /ID:166
if "%1"=="55" call %0 setNLS 470 MT   853 EGA  MT
if "%1"=="56" call %0 setNLS 616 PL   852 EGA  PL
if "%1"=="57" call %0 setNLS 616 PL   852 EGA  PL PL214.KL /ID:214
if "%1"=="58" call %0 setNLS 642 RO   852 EGA  RO RO446.KL /ID:446
if "%1"=="59" call %0 setNLS 792 TR   857 EGA  TR
if "%1"=="60" call %0 setNLS 826 EN   858 EGA  UK UK168.KL /ID:168

if "%1"=="61" call %0 setNLS 804 UK   848 EGA4 UR UR465.KL /ID:465
if "%1"=="62" call %0 setNLS 804 UK   848 EGA4 UR
if "%1"=="63" call %0 setNLS 804 TT 59234 EGA7 TT
if "%1"=="64" call %0 setNLS   8 SQ   852 EGA  SQ SQ448.KL /ID:448
if "%1"=="65" call %0 setNLS   8 SQ   852 EGA  SQ
if "%1"=="66" call %0 setNLS 124 FR   858 EGA  CF CF445.KL /ID:445
if "%1"=="67" call %0 setNLS 203 CS   852 EGA  CZ
if "%1"=="68" call %0 setNLS --- EO   853 EGA  EO EO166.KL /ID:166
if "%1"=="69" call %0 setNLS --- EO   853 EGA  EO
if "%1"=="70" call %0 setNLS 250 FR   858 EGA  FR FR120.KL /ID:120

if "%1"=="71" call %0 setNLS 276 DE   858 EGA  GR GR453.KL /ID:453
if "%1"=="72" call %0 setNLS 380 IT   858 EGA  IT IT142.KL /ID:142
if "%1"=="73" call %0 setNLS 428 LV   775 EGA2 LV LV455.KL /ID:455
if "%1"=="74" call %0 setNLS 440 LT   775 EGA2 LT LT211.KL /ID:211
if "%1"=="75" call %0 setNLS 440 LT   775 EGA2 LT LT212.KL /ID:212
if "%1"=="76" call %0 setNLS 440 LT   775 EGA2 LT LT456.KL /ID:456
if "%1"=="77" call %0 setNLS 807 MK   872 EGA3 MK
if "%1"=="78" call %0 setNLS 643 RU   808 EGA3 RU RU443.KL /ID:443
if "%1"=="79" call %0 setNLS 891 SR   872 EGA3 SR
if "%1"=="80" call %0 setNLS 891 SR   872 EGA3 SR SR450.KL /ID:450

if "%1"=="81" call %0 setNLS 756 FR   858 EGA  SF
if "%1"=="82" call %0 setNLS 756 DE   858 EGA  SG
if "%1"=="83" call %0 setNLS 826 GD   858 EGA  GA
if "%1"=="84" call %0 setNLS 840 EN   858 EGA  LH
if "%1"=="85" call %0 setNLS 840 EN   858 EGA  RH

goto finalend

:DoNLS
set choice=
cls
goto setkeyb
goto codepage

:setkeyb
rem if exist %fdosroot%\NLS\LOCALIZE.%3 set lang=%3
set lang=%3
set uniqueID=%2 
if "%6"=="" goto codepage
set kblayout=%6
set keybrest=%7 %8 %9
set keybfile=KEYBOARD.SYS
if "%keybfile%"=="%7" set keybrest=%9
goto codepage

:codepage
rem Latin America has no codepage number; Japan has no CPI/CPX file
if "%4"=="---" goto loadkeyb
if "%5"=="---" goto loadkeyb
set cpxfile=%5
set cp=%4
if not exist %fdosroot%\ODIN\CPI\%5.CPX goto end
MODE CON CP /STATUS
if "%errorlevel%"=="1" LH DISPLAY CON=(EGA,,1)
MODE CON CP PREP=((%4) %fdosroot%\ODIN\cpi\%5.CPX)
MODE CON CP SEL=%4
goto loadkeyb

:loadkeyb
if "%kblayout%"=="" goto finalend
rem KEYB;DISPLAY;KEYB/U;KEYB;DISPLAY goes wrong, load DISPLAY first, then KEYB(/U)
KEYB /U
LH KEYB %kblayout% %keybrest%
if not errorlevel 1 set keybfile=keyboard.sys
if errorlevel 1 set keybfile=keybrd2.sys
if errorlevel 1 LH KEYB %kblayout%,,keybrd2.sys %keybrest%
if errorlevel 1 set keybfile=keybrd3.sys
if errorlevel 1 LH KEYB %kblayout%,,keybrd3.sys %keybrest%
:end
:finalend
