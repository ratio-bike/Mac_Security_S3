@echo off
ctty con
if not [%1]==[] goto %1
if not [%1]==[] echo Sorry, die Nachricht die Sie sehen sollten konnte nicht angezeigt werden.
if [%1]==[]     echo Diese Datei sollte mit einem Argument aufgerufen werden (Sprungmarke)
goto end

rem Keep for Dutch translation, I first want to work on English message-file
rem if not [%1]==[] echo Sorry, het bericht dat u nu zou moeten zien kon niet worden weergegeven.
rem if [%1]==[]     echo Dit bestand dient met een parameter te worden aangeroepen (labelnaam)
rem Here start the messages you want to display, each should end with GOTO END on last line
rem of the message. Message-label has a maximum of 8 characters!

:pickLANG
type %cdrom%\freedos\install\docs\instlang.txt
goto end

:showmenu
cls
if not [%header%]==[] %header%
ECHO ò
ECHO ò       1)   FreeDOS Ripcord Beta9 Installation
ECHO ò       2)   FreeDOS von CD-ROM starten (zum Command Prompt zurÅckkehren)
ECHO ò       3)   FreeDOS Bootdiskette erstellen (1.44MB)
ECHO ò
if [%pqpart%]==[true]   ECHO ò       4)   Festplatte(n) mit PartitionMagic partitionieren
if [%pqbackup%]==[true] ECHO ò       5)   Festplatte(n)/Partition(en) mit Drive Image sichern oder wiederherstellen
ECHO ò 
if [%oemcmd%]==[true]   ECHO ò       o)   OEM Tools
if not [%VM%]==[TRUE]   ECHO ò       x)   XOSL Bootmanager installieren (486+, 4MB+, VESA+)
ECHO ò 
ECHO ò       v)   Installationstext zum Gebrauch dieser CD-ROM anzeigen
ECHO ò            (DrÅcken Sie ESC um das Anzeigeprogramm zu verlassen)
ECHO ò 
ECHO ò       r)   Neustart
ECHO ò
goto end

:runoem
ECHO OEM Tools von hier starten.
ECHO Momentan sind keine OEM Tools in dieser Distribution enthalten.
ECHO Sie sind willkommen Ihre eigenen hinzufÅgen!  Siehe %cdrom%\FREEDOS\AUTORUN\AUTORUN.BAT
ECHO und A:\CONFIG\CFG.BAT
pause DrÅcken Sie eine Taste um zum FreeDOS MenÅ zurÅckzukehren
goto end

:runxosl
ECHO WARNUNG! Es empfiehlt sich, XOSL von einer Diskette zu starten, sodass Sie
ECHO wÑhrend der Installation Informationen existierender Partitionen sichern kînnen!
ECHO Mîchten Sie die XOSL Installation fortsetzen?
goto end

:setHeadr
SET line1=FreeDOSòInstallation FreeDOSòRipcordòBetaò9ò[Methusalem]òDistribution ò
SET HEADER=for %%x in ( %line1% ) do echo %%x
goto end

:KeybEnd
if [%KEY%]==[FALSE] ECHO XKeyB nicht geladen.
rem set KEY=
PAUSE
goto end

:STDPRMTS
ECHO Falls Sie nicht schon eine DOS Partition erstellt haben, sollten Sie
ECHO jetzt FDISK starten, andernfalls kînnen Sie diesen Schritt auslassen.
ECHO ò  Folgen Sie den Anweisungen auf dem Bildschirm um eine neue PrimÑrpartition
ECHO ò  zu erstellen und sie als aktiv zu markieren.  Nach dem Gebrauch von FDISK
ECHO ò  werden Sie neustarten mÅssen.  Booten Sie einfach erneut von Bootdiskette,
ECHO ò  und beantworten Sie diese Frage mit Nein.
ECHO.
goto end

:POSTFDSK
ECHO Falls Sie gerade eine neue Partition fÅr FreeDOS erstellt haben, FORMATieren
ECHO Sie sie nun.  Sie kînnen die Partition auch formatieren um sie vîllig zu leeren.
ECHO WARNUNG: Alle Daten auf Laufwerk C: werden gelîscht, wÑhlen Sie nur JA (YES)
ECHO ò        wenn Sie sicher sind. (Sie mÅssen auch bei FORMAT 'Y' drÅcken).
ECHO.
goto end

:MAKEBOOT
ECHO Sie mÅssen das Laufwerk nun bootfÑhig machen (also den KERNEL und
ECHO COMMAND darauf kopieren und den Bootrecord setzen).
ECHO.
goto end

:DO_INST
ECHO %DISTRO% Installation gewÑhlt.
ECHO.
REM ECHO Als Quelle kînnen Sie \ angeben (oder den vollen Pfadnamen, z.B. A:\).
ECHO Als Quelle kînnen Sie %cdrom%\FREEDOS\INSTALL\DISKSETS angeben.
ECHO Au·erdem mÅssen Sie angeben, wo FreeDOS installiert werden soll,
ECHO meist ist dies C:\FDOS (oder C:\DOS).
ECHO.
pause
goto end

:DO_INST2
ECHO.
ECHO Ihre Festplatte sollte bereit sein FreeDOS zu booten,
ECHO (optional:) Bitte kopieren Sie die Beispieldateien CONFIG.SYS und
ECHO AUTOEXEC.BAT nach C:\ und editieren Sie sie je nach Bedarf, booten Sie dann.
goto end

:makebf
echo Erstelle allgemeine CD Bootdiskette.
echo Bitte stecken Sie eine Diskette in das Diskettenaufwerk.
echo Wenn Sie nach einem Ziel gefragt werden (Laufwerk auf das geschrieben
echo werden soll), geben Sie bitte an:
echo ò  Falls Sie von CD booten geben Sie B: als ihr erstes Diskettenlaufwerk an
echo ò  andernfalls geben Sie die gewohnten Laufwerkbuchstaben an
echo    (z.B. A: fÅr das erste Diskettenlaufwerk)
goto end

:makebf2
echo.
goto end

:the_end
echo Die folgenden Schritte wurden soeben wÑhrend der Nachkonfiguration ausgefÅhrt:
echo.
echo * Einrichten einer alternativen Config.sys Datei
echo * Einrichten einer alternativen Autoexec.bat Datei
echo * Kopieren der FreeDOS Kerndateien, sodass Ihre Festplatte annÑhernd bootfÑhig ist.
echo * öbersetzen von Command.com in die wÑhrend der Installation benutzte Sprache.
echo * öbersetzen des Maustreibers (CTMOUSE.EXE) in die wÑhrend der Installation benutzte Sprache.
echo * Laden verschiedener Treiber und Booteinstellungen.
echo * Speichern des Bootsektors in C:\FDBTSECT.BIN um das HinzufÅgen
echo   zu einem Bootmanager zu vereinfachen
echo   - erweitern Sie die Datei C:\boot.ini von NT, 2000 oder XP um folgende Zeile:
echo   -    C:\FDBTSECT.BIN="FreeDOS Beta 9"
echo.
echo !!! Bitte geben Sie BOOT ein falls Sie Ihre Festplatte bootfÑhig !!!
echo !!! machen wollen, andernfalls starten Sie Ihr System jetzt neu. !!!
goto end

:makesys
echo Ihr System ist jetzt bootfÑhig. Wir empfehlen das System neu zu starten.
goto end

:makehelp
echo BOOT
echo.
echo Schreibt Bootsektoren auf die Festplatte. Dies ermîglicht FreeDOS hochzufahren.
echo Falls Sie noch andere Betriebssysteme auf diesem Computer installiert haben,
echo benutzen Sie bitte deren Bootmanager um FreeDOS hinzuzufÅgen.
echo.
echo WARNUNG: BOOT kann nicht-DOS Betriebssysteme bootunfÑhig machen.
goto end

:showbeta
PAUSE
goto end

:ABORTINST
ECHO Installation abgebrochen!
GOTO FINI
:FINI
ECHO Geben Sie MENU ein um zum MenÅ zurÅckzukehren :)
ECHO.
goto end

:showb2
ECHO Installation abgeschlossen.
goto end

:seal2
Pause DrÅcken Sie eine Taste um die manuelle Seal Desktop Installation zu starten
goto end

:mono_msg
choice /N /C:JN"Monochrom (/MONO) Modus setzen [J/N]?"
goto end

:fullmini
choice /C:KM "Mîchten Sie eine [K]omplette oder eine [M]ini Installation?"
goto end

:end
rem return to calling batchfile...
