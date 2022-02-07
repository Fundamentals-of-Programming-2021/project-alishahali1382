#!/bin/bash
cd state.io
cmake .
cmake --build .
rm -r CMakeFiles
rm cmake_install.cmake
rm CMakeCache.txt
mv state ../state
cp -r assets/ ../assets/
