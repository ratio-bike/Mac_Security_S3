Documentation for build - https://github.com/tianocore/tianocore.github.io/wiki/Using-EDK-II-with-Native-GCC

Some known problems are listed

1. If EDK path is in mounted volume with FAT file system(using vmware sharing),
error occurs because python script String.py is treated as string.py.
Following is Solution for the problem.

renamed String.py to MyString.py,
$ cd BaseTools/Source/Python
$ rm -f ./Common/String.pyc
$ grep -ir -l --include="*.py" "Common.String" * | xargs sed -i 's/Common.String/Common.MyString/g'
$ grep -r -l --include="*.py" "from String import" * | xargs sed -i 's/from String import/from MyString import/g'

2. nasm not found

$ sudo apt-get install nasm

3. ld: internal error ~
This is because of ld.gold bug
so change ld to ld.bfd
/usr/bin/ld is symlink to /usr/bin/ld.gold, so make ld link to /usr/bin/ld.bfd
