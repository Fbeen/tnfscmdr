#!/bin/bash

make

if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

cp build/tnfscmdr.exe ~C:\DOS

dosbox-x
