#!/bin/bash
cd state.io
cmake .
cmake --build .
rm -r CMakeFiles
rm cmake_install.cmake
rm CMakeCache.txt
cd ..
mv state.io/state state
cp -r state.io/assets assets
