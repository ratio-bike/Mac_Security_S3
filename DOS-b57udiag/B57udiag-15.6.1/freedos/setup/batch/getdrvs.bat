@echo off

rem if it is a non-CD-ROM hard drive, drive:\nul will exist.  Do not check
rem for floppy drives at this point (would installing to a floppy even work??)

for %%x in (C D E F G H I J K L M N O P Q R S T U V W Y) do if exist %%x:\NUL echo %%x:>>%1
