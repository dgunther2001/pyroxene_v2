#!/usr/bin/env bash

rm -rf build
mkdir -p build
cd build

cmake ..  -DCMAKE_PREFIX_PATH=$INCLUDED_LIBRARY_PATH -DCMAKE_INSTALL_PREFIX=$PYROXENE_ROOT_PATH -DBUILD_SHARED_LIBS=ON  > /dev/null
make > /dev/null
#make install > /dev/null

./driver