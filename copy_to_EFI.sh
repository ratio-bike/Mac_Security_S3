
sudo mkdir /Volumes/efi
sudo mount -t msdos /dev/disk0s1 /Volumes/efi
cp /Users/jjh/Desktop/JJH/Mac_Hacking_S3/EDK2/edk2/Build/MdeModule/DEBUG_GCC46/X64/MdeModulePkg/Application/HelloWorld/HelloWorld/DEBUG/HelloWorld.efi /Volumes/efi/EFI/efi_test/
sudo diskutil umount /Volumes/efi/
