#!/bin/bash
printf "\033c"
g++ -std=c++11 -I/Users/det/Documents/Arduino/Libraries/PJON/src  MockSender.cpp -o MockSender
./MockSender
