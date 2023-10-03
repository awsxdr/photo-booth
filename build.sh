#! /bin/bash

g++ `pkg-config --cflags --libs glib-2.0` -o test main.cpp -std=c++20 -L/usr/lib/aarch64-linux-gnu -lX11