@echo off
ctty con
if not [%1]==[] goto %1
if not [%1]==[] echo El mensaje que deber¡a ver no pudo visualizarse.
if [%1]==[]     echo Este programa deber¡a llamarse con un argumento (etiqueta adonde saltar)
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
ECHO ÿ
ECHO ÿ       1)   Instalaci¢n de FreeDOS Ripcord Beta9
ECHO ÿ       2)   Ejecutar FreeDOS desde el CD-ROM (salir al int‚rprete)
ECHO ÿ       3)   Crear un disquete de arranque de FreeDOS (1'44MB)
ECHO ÿ 
if [%pqpart%]==[true]   ECHO ÿ       4)   Particionar sus discos con PartitionMagic
if [%pqbackup%]==[true] ECHO ÿ       5)   Hacer o restaurar discos/particiones con Drive Image
ECHO ÿ 
if [%oemcmd%]==[true]   ECHO ÿ       o)   Utilidades OEM
if not [%VM%]==[TRUE]   ECHO ÿ       x)   Instalar el gestor de arranque XOSL (486+, 4MB+, VESA+)
ECHO ÿ 
ECHO ÿ       v)   Ver las instrucciones de instalaci¢n desde este CD-ROM
ECHO ÿ            (Presione Esc para abandonar el visor)
ECHO ÿ 
ECHO ÿ       r)   Reiniciar
ECHO ÿ
goto end

:runoem
ECHO Ejecutar utilidades OEM desde aqu¡.
ECHO En la actualidad, no se distribuyen utilidades OEM en esta distribuci¢n.
ECHO ­A¤ada las suyas! Vea %cdrom%\FREEDOS\AUTORUN\AUTORUN.BAT
ECHO y A:\CONFIG\CFG.BAT
pause Presione cualquier tecla para volver al men£ de FreeDOS.
goto end

:runxosl
ECHO ­Atenci¢n! es recomendable ejecutar XOSL desde un disquete para poder
ECHO hacer copia de seguridad de la informaci¢n sobre particiones durante
ECHO la instalaci¢n.
ECHO ¨Desea continuar la instalaci¢n de XOSL?
goto end

:setHeadr
SET line1=Instalaci¢n de lka distribuci¢n FreeDOSÿRipcordÿBetaÿ9ÿ[Methusalem]
SET HEADER=for %%x in ( %line1% ) do echo %%x
goto end

:KeybEnd
if [%KEY%]==[FALSE] ECHO KeyB no cargado.
rem set KEY=
PAUSE
goto end

:STDPRMTS
ECHO Si no ha creado una partici¢n DOS, deber¡a ejecutar FDISK ahora. En
ECHO caso contrario, puede omitir este paso.           
ECHO ÿ  Siga las indicaciones para crear una partici¢n primara y m rquela
ECHO ÿ  como partici¢n activa. Tras ejecutar FDISK deber  reiniciar el PC.
ECHO ÿ  Reinicie de nuevo desde el disco de instalaci¢n y seleccione NO en
ECHO    esta pregunta.
ECHO.
goto end

:POSTFDSK
ECHO Si ha creado una partici¢n nueva para FreeDOS, deber  darle formato
ECHO ahora. Tambi‚n podr  darle formato si desea borrarla completamente.
ECHO Atenci¢n: Se perder  toda la informaci¢n de la unidad C:, as¡ que
ECHO           s¢lamente elija SÖ si est  seguro (tambi‚n deber  introducir
ECHO           'Y' cuando FORMAT lo pregunte).
ECHO.
goto end

:MAKEBOOT
ECHO Ahora deber  transferir el sistema a la unidad (copiar el n£cleo y
ECHO COMMAND y actualizar el sector de arranque).
ECHO.
goto end

:DO_INST
ECHO Ha seleccionado la instalaci¢n %DISTRO%.
ECHO.
REM ECHO Para el origen, especifique \ (o la ruta completa, p.ej. A:\)
ECHO Para el origen, puede especificar %cdrom%\FREEDOS\INSTALL\DISKSETS
ECHO Tambi‚n se le preguntar  d¢nde desea instalar FreeDOS. Generalmente,
ECHO ser  C:\FDOS (o C:\DOS).
ECHO.
pause
goto end

:DO_INST2
ECHO.
ECHO Su unidad deber¡a ahora poder iniciar FreeDOS
ECHO Opcionalmente, copie CONFIG.SYS y AUTOEXEC.BAT a C:\ , ed¡telos
ECHO seg£n necesite y reinicie el equipo.
goto end

:makebf
echo Creando un disco de arranque gen‚rico.
echo Por favor, introduzca un disquete en la unidad.
echo Cuando se le pregunte por la unidad de destino, especifique:
echo ÿ  si ha iniciado desde el CD, use B: para su primera unidad de disco
echo    en caso contrario, utilice las letras habituales (A: la primera)
goto end

:makebf2
echo.
goto end

:the_end
echo Las siguientes operaciones se han llevado a cabo en la configuraci¢n
echo posterior a la instalaci¢n:
echo.
echo * Establecer CONFIG.SYS alternativo
echo * Establecer AUTOEXEC.BAT alternativo
echo * Transferir los archivos base (su disco duro es casi de arranque).
echo * Traducir COMMAND.COM al idioma elegido en la instalaci¢n.
echo * Traducir el controlador de rat¢n CTMOUSE.EXE al lenguaje utilizado
echo   durtante la instalaci¢n
echo * Cargar algunos controladores y establecer opciones de arranque.
echo * Guardar el sector de arranque en C:\FDBTSECT.BIN para anadirlo
echo   f cilmente a un gestor de arranque
echo   - para el archivo c:\boot.ini de Microsoft Windows NT,2000 o XP,
echo     a¤ada la siguiente l¡nea:
echo   -    C:\FDBTSECT.BIN="FreeDOS beta 9"
echo.
echo Introduzca BOOT si desea hacer su disco duro de arranque
echo En caso contrario, reinicie su equipo ahora.
goto end

:makesys
echo Su equipo puede iniciar FreeDOS ahora. Se recomienda reiniciar su PC.
goto end

:makehelp
echo BOOT
echo.
echo Escribe el c¢digo del sector de arranque a un disco duro. Posibilita
echo a FreeDOS iniciarse. Si utiliza otro(s) sistema(s) operativo(s) en
echo este PC, utilice su gestor de arranque para a¤adir FreeDOS.
echo.
echo Atenci¢n: BOOT puede evitar que sistemas no DOS no se inicialicen.
goto end

:showbeta
PAUSE
goto end

:ABORTINST
ECHO La instalaci¢n se ha cancelado
GOTO FINI
:FINI
ECHO Escriba MENU para volver al men£
ECHO.
goto end

:showb2
ECHO La instalaci¢n se ha completado.
goto end

:seal2
Presione una tecla para comenzar la instalaci¢n manual del entorno Seal.
goto end

:mono_msg
choice /N /C:SN"¨Activar modo monocromo (/MONO)? [SN]?"
goto end

:fullmini
choice /C:CM "¨Desea una instalaci¢n [C]OMPELTA o [M]INI?"
goto end

:end
rem return to calling batchfile...
