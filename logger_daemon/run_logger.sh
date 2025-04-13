#!/usr/bin/env bash

mkdir -p build
cd build
cmake .. > /dev/null
make > /dev/null
./driver
cd ..