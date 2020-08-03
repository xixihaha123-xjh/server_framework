#!/bin/bash
if [ $# == 1 ]; then
    cd /home/Nihaowa123/server_framework/build
    make clean
    exit 0
fi

cd /home/Nihaowa123/server_framework/build
cmake ..

make -j4
cd /home/Nihaowa123/server_framework/bin


echo "#####################################"
echo "#####################################"
echo
echo
./test_config

if [ $# == 2 ]; then
    cd /home/Nihaowa123/server_framework/bin
    gdb ./test_config
fi
