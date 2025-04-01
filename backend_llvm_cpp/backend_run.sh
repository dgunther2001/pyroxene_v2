#!/usr/bin/env bash

mkdir -p build
cd build
cmake .. 
make 
./driver
cd ..
#rm -rf build
