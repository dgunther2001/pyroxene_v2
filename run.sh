#!/usr/bin/env bash

rm -rf logs
mkdir logs
export PYROXENE_LOG_PATH=$(pwd)/logs/pyroxene.log

LOG_ENABLED=false
for arg in "$@"; do
    case $arg in
        --log)
            LOG_ENABLED=true
            shift
            ;;
        *)

            ;;
    esac
done

if $LOG_ENABLED; then 
    export PYROXENE_LOG="1"
    touch logs/pyroxene.log
fi

rm -rf tmp/

mkdir tmp/
chmod 755 tmp/

cd backend_llvm_cpp
./backend_run.sh &
cd ..

sleep 0.1
cd middle_end_rs
./middle_end_run.sh &
cd ..

sleep 0.1
cd frontend-haskell
./frontend_hs.sh
cd ..

wait

rm -rf tmp/
