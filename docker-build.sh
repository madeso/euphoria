#!/bin/bash

# exit on error
# https://stackoverflow.com/questions/2870992/automatic-exit-from-bash-shell-script-on-error
set -euxo pipefail

echo "List all installed GCC"
which gcc
echo "*** gcc ***"
ls -l /usr/bin/gcc*

echo "List all installed clang"
echo "*** clang ***"
ls /usr/bin/clang-*

echo "====================== Create Build Environment"
cmake -E make_directory /euph/build

echo "========================= Configure"
cd /euph/build 
cmake \
    -GNinja \
    -DCMAKE_C_COMPILER=/usr/bin/clang-22 \
    -DCMAKE_CXX_COMPILER=/usr/bin/clang++-22 \
    -DCMAKE_BUILD_TYPE=Debug .. 

echo "========================= Build"
time cmake --build . --config Debug


echo "========================= setup clang titdy"
cd /euph 
clang-tidy-22 --version
dotnet run --project ./tools/buildtools/Workbench/ -- path clang-tidy list 
dotnet run --project ./tools/buildtools/Workbench/ -- path clang-tidy set /usr/bin/clang-tidy-22 
dotnet run --project ./tools/buildtools/Workbench/ -- path clang-tidy list

echo "========================= run titdy"
dotnet run --project ./tools/buildtools/Workbench/ -- clang tidy libs apps --html docs/generated/tidy/
