#!/bin/bash

# ensure a clean build
make clean

# gcc
printf "gcc make\n\n"
make -j8
printf "\n\ngcc test\n\n"
make test -j8

# g++
printf "\n\ng++ test\n\n"
make CC=g++ test -j8

# clean up
make clean