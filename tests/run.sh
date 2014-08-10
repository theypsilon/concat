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

echo "LINKER TEST"
echo "-----------"

$CXX file1.cpp file2.cpp -std=c++11 -lm -lstdc++
./a.out
if [ $? -eq 0 ]; then
	echo -e "\e[1;32mPassed.\e[0m"
fi
rm a.out

echo ""
echo "COMPILER TESTS"
echo "--------------"

./compile_tester.sh