#!/usr/bin/env bash

mkdir tmp/
chmod 755 tmp/

cd backend_llvm_cpp
./backend_run.sh &
cd ..

sleep 0.1
cd frontend_rs
./frontend_run.sh
cd ..

wait

rm -rf tmp/
