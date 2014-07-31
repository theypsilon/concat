#!/bin/bash

$CXX --version && $CXX unit.cpp -std=c++11 -lm -lstdc++ && ./a.out