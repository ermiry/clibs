#!/bin/bash

# ensure a clean build
make clean

# main sources
printf "gcc make\n\n"
make TYPE=production -j8
