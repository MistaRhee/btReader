#! /bin/sh
if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root" 1>&2
    exit 1
fi

#Copy over the .so files
cp lib/* /usr/local/lib
ldconfig

make
make clean
