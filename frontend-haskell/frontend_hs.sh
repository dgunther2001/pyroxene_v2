#!/usr/bin/env bash

stack build --verbosity warn > /dev/null
stack exec frontend-haskell