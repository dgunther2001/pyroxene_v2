#!/usr/bin/env bash

cd backend_llvm_cpp
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cd ..
ln -s build/compile_commands.json compile_commands.json

chmod u+x run.sh

cd frontend-haskell
chmod u+x frontend_hs.sh
cd ..

cd middle_end_rs
chmod u+x middle_end_run.sh
cd ..

cd backend_llvm_cpp
chmod u+x backend_run.sh
cd ..
