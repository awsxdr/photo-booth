cmake_minimum_required(VERSION 3.13)

project(photo_booth LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES 
    button.cpp
    countdown_window.cpp
    input.cpp
    main.cpp
    photo_booth.cpp
    preview_window.cpp
    texture_loader.cpp
    upload_process.cpp
    window_manager.cpp)
add_executable(photo_booth ${SOURCE_FILES})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions("DEBUG")
endif()

target_link_directories(photo_booth PUBLIC "/usr/lib/aarch64-linux-gnu")

target_link_libraries(photo_booth "X11" "pigpio" "GL" "png" "jpeg" "uuid")