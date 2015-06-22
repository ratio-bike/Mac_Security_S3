@echo off
ctty con
if not [%1]==[] goto %1
if not [%1]==[] echo Sorry, het bericht dat u nu zou moeten zien kon niet worden weergegeven.
if [%1]==[]     echo Dit bestand dient met een parameter te worden aangeroepen (labelnaam)
goto end

rem Keep for Dutch translation, I first want to work on English message-file
rem if not [%1]==[] echo 
rem if [%1]==[]     echo 
rem Here start the messages you want to display, each should end with GOTO END on last line
rem of the message. Message-label has a maximum of 8 characters!

:pickLANG
type %cdrom%\freedos\install\docs\instlang.txt
goto end

:showmenu
cls
if not [%header%]==[] %header%
ECHO ÿ
ECHO ÿ       1)   FreeDOS Ripcord Beta9 Installatie
ECHO ÿ       2)   voer FreeDOS uit vanaf CD-ROM (keer terug naar de commando-prompt)
ECHO ÿ       3)   Maak een FreeDOS opstartdiskette (1.44MB)
ECHO ÿ 
if [%pqpart%]==[true]   ECHO ÿ       4)   Partitioneer harddisk(s) mbv PartitionMagic
if [%pqbackup%]==[true] ECHO ÿ       5)   Backup or Restore harddisk(s)/partitie(s) mbv Drive Image
ECHO ÿ 
if [%oemcmd%]==[true]   ECHO ÿ       o)   OEM programma's
if not [%VM%]==[TRUE]   ECHO ÿ       x)   Installeer XOSL boot manager (486+, 4MB+, VESA+)
ECHO ÿ 
ECHO ÿ       v)   Bekijk de Installatietekst voor het gebruiken van van deze CD-ROM
ECHO ÿ            (Druk op de ESC toets om het bekijk-programma te verlaten)
ECHO ÿ 
ECHO ÿ       r)   Herstarten
ECHO ÿ
goto end

:runoem
ECHO Draai de OEM tools vanaf hier.
ECHO Op dit moment worden er programmma's met deze Distributie gebundeld.  
ECHO Het staat u vrij zelf programma's toe te voegen! Zie hiervoor
ECHO %cdrom%\FREEDOS\AUTORUN\AUTORUN.BAT en A:\CONFIG\CFG.BAT
pause Druk op een toets om terug te keren naar het FreeDOS menu
goto end

:runxosl
ECHO WAARSCHUWING! XOSL raadt aan dat u het vanaf een FLOPPY uitvoert
ECHO zodat u bestaande partitiegegevens kunt opslaan tijdens de installatie!
ECHO Wilt u verder gaan met de installatie van XOSL ?
goto end

:setHeadr
SET line1=FreeDOSÿinstallatie FreeDOSÿRipcordÿBetaÿ9ÿ[Methusalem]ÿDistributie ÿ
SET HEADER=for %%x in ( %line1% ) do echo %%x
goto end

:KeybEnd
if [%KEY%]==[FALSE] ECHO XKeyB niet geladen.
rem set KEY=
PAUSE Druk op een toets om verder te gaan . . .
goto end

:STDPRMTS
ECHO Als u nog geen DOS partitie heeft aangemaakt dan moet u nu FDISK uitvoeren,
ECHO indien u dit al wel heeft gedaan kunt u deze stap overslaan.
ECHO ÿ  Volg de aanwijzingen op het scherm om een nieuwe primaire partitie te
ECHO ÿ  maken en deze te merken als Actief.  Na het uitvoeren van FDISK zult u
ECHO ÿ  moeten herstarten.
ECHO    Start gewoon weer van de installatiediskette op en beantwoord deze
ECHO    prompt met NEE.
ECHO.
goto end

:POSTFDSK
ECHO Als u zojuist een nieuwe partitie voor FreeDOS hebt aangemaakt dan moet u
ECHO deze nu formatteren.  U kunt eventueel ook de partitie formatteren als u 
ECHO deze leeg wilt maken.
ECHO.
ECHO WAARSCHUWING: Alle gegevens op schijf C: zullen hierdoor verloren gaan.
ECHO ÿ             kies dus alleen JA (YES) als u het zeker weet. 
ECHO ÿ             (U moet ook 'Y' typen bij Format's prompt).
ECHO.
goto end

:MAKEBOOT
ECHO U dient nu de schijf opstartbaar te maken.
ECHO (wat inhoudt dat de KERNEL en COMMAND er naartoe worden 
ECHO  geschreven en dat het opstartrecord wordt beschreven).
ECHO.
goto end

:DO_INST
ECHO %DISTRO% installatie gekozen.
ECHO.
ECHO Als bron kunt u %cdrom%\FREEDOS\INSTALL\DISKSETS opgeven.
ECHO U zal gevraagd worden op welke locatie op de schijf u FreeDOS wilt instal-
ECHO leren. De meeste mensen kiezen hierbij gewoonlijk voor C:\FDOS (of C:\DOS)
ECHO.
pause Druk op een toets om verder te gaan . . .
goto end

:DO_INST2
ECHO.
ECHO Uw harde schijf is nu helemaal klaar om FreeDOS op te kunnen starten,
ECHO Voor meer informatie over het beinvloeden van de opstart-opties
ECHO (bv muis-driver toevoegen) kunt u de voorbeeldbestanden CONFIG.SYS
ECHO en AUTOEXEC.BAT in uw FreeDOS directory inzien met het programma EDIT.
ECHO voorbeeld:           CD DOS
ECHO ÿ                    EDIT CONFIG.SYS
 goto end

:makebf
echo Algemene CD opstartdiskette wordt aangemaakt.
echo Plaats alstublieft een diskette in de floppy drive.
echo Wanneer u wordt gevraagd naar het doel/target (schijf waarnaar
echo toe te schrijven), geef dan de volgende schijfletter op:
echo 
echo ÿ  Als u opstart van CD gebruik dan B: als schijfletter voor 
echo ÿ  uw eerste floppy drive;
echo ÿ  gebruik in alle andere gevallen de gewoonlijke schijfletters
echo ÿ  (bv. A: voor 1e floppy drive)
echo.
goto end

:makebf2
echo.
goto end

:the_end
echo De volgende dingen zijn zojuist gedaan en geregeld na afloop van installatie:
echo.
echo * Config.sys opstartbestand aanmaken en inrichten
echo * Autoexec.bat opstartbestand aanmaken en inrichten
echo * Het overbrengen van de FreeDOS kernbestanden, om uw harddisk klaar te maken
echo ÿ voor de mogelijkheid om FreeDOS op te starten.
echo * Het vertalen van command.com in de tijdens de installatie gebruikte taalkeuze.
echo * Het vertalen van het muis-stuurprogramma (CTMOUSE.EXE) in dezelfde taalkeuze.
echo * het laden van enkele stuurprogramma's en opstart-instellingen.
echo * Opslaan van bootsector in C:\FDBTSECT.BIN om het toevoegen van FreeDOS aan
echo ÿ een bootmanager te vergemakkelijken.
echo ÿ      voeg voor NT,2000 or XP's c:\boot.ini bestand de volgende regel toe:
echo ÿ      C:\FDBTSECT.BIN="FreeDOS beta 9"
echo.
echo !!! type alstublief BOOT als u uw harddisk opstartbaar wilt maken, !!!
echo !!! en anders herstart uw systeem nu                               !!!
goto end

:makesys
echo Uw systeem is nu opstartbaar. Wij raden u aan het systeem te herstarten
goto end

:makehelp
echo BOOT
echo.
echo Schrijft de bootsectorcode naar de harde schijf. Dit zorgt ervoor dat u FreeDOS
echo kunt opstarten. Als u nog een ander besturingssysteem geinstalleerd hebt, 
echo gebruik dan bij voorkeur de bootmanager die daarbij geleverd wordt.
echo.
echo Waarschuwing: BOOT kan niet-DOS besturingssystemen onstartbaar maken.
goto end

:showbeta
PAUSE Druk op een toets om verder te gaan . . .
goto end

:ABORTINST
ECHO Installatie afgebroken!
goto end

:fini
ECHO Typ MENU om terug te keren naar het basis-menu.
ECHO.
goto end

:SHOWB2
echo Installatie succesvol voltooid!
goto end

:SEAL2
Pause Druk op een toets om de handmatige intallatie van de Seal Desktop te starten
goto end

:mono_msg
choice /N /C:JN"Zwart-wit (monochroom) beeldscherm-mode gebruiken ipv kleur [J/N]?"
goto end

:fullmini
choice /C:VM "Wilt u een [V]olledige of [M]ini installatie uitvoeren?"
goto end

:end
rem return to calling batchfile...