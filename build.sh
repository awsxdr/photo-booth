#! /bin/bash

clear

build_type="${1:-Debug}"

cmake -B ./build/$build_type -DCMAKE_BUILD_TYPE=$build_type
cmake --build ./build/$build_type --config $build_type

sudo chown root:root ./build/$build_type/photo_booth
sudo chmod 4755 ./build/$build_type/photo_booth

cp -r ./textures ./build/$build_type/textures/