#!/bin/bash

$CXX --version && $CXX tests/unit.cpp -std=c++11 -lm -lstdc++ && ./a.out