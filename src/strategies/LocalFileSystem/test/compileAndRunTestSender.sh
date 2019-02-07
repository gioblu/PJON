#!/bin/bash
printf "\033c"
g++ -std=c++11 -I/Users/det/Documents/Arduino/Libraries/PJON/src  TestSender.cpp -o TestSender
TestSender $1
