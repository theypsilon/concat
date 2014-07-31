#!/bin/bash
echo ""
echo "RUNNING TEST SUITE"
echo "=================="

cd tests

$CXX --version

echo ""
echo "UNIT TESTS"
echo "----------"

$CXX unit.cpp -std=c++11 -lm -lstdc++
./a.out

echo "COMPILATION TEST"
echo "----------------"

$CXX file1.cpp file2.cpp -std=c++11 -lm -lstdc++
./a.out