cmake_minimum_required(VERSION 3.15)
project(raytracingSDL)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_image REQUIRED CONFIG)

file(GLOB sources src/*.cpp src/*.h src/rt-headers/*.h)

add_executable(raytracingSDL ${sources})
target_link_libraries(raytracingSDL SDL2::SDL2main)
target_link_libraries(raytracingSDL SDL2::SDL2)
target_link_libraries(raytracingSDL SDL2_image::SDL2_image)