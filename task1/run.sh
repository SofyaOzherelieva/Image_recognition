#! /bin/bash

g++ Code/main.cpp -o bin -std=c++11 -O2 -L/usr/X11R6/lib -lm -lpthread -lX11