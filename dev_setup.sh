#!/usr/bin/env bash

cd backend_llvm_cpp
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cd ..
ln -s build/compile_commands.json compile_commands.json
