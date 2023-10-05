#! /bin/bash

# g++ `pkg-config --cflags --libs glib-2.0` -o test main.cpp -std=c++20 -L/usr/lib/aarch64-linux-gnu -lX11

build_type="${1:-Debug}"

cmake -B ./build -DCMAKE_BUILD_TYPE=$build_type
cmake --build ./build --config $build_type

sudo chown root:root ./build/photo_booth
sudo chmod 4755 ./build/photo_booth