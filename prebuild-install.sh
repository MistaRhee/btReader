#! /bin/sh

#This is an installer to use the pre-built version (may not be up-to-date, but doesn't require compiler)
#Installs in PWD

#Remove all source files and .git stuff
rm -rf .git/ src/ include/ makefile .gitignore install.sh
cp -r bin/* ./
rm -rf bin

echo "Should be all good! Remove prebuild-install.sh if you wish. To uninstall, just remove the folder!"
