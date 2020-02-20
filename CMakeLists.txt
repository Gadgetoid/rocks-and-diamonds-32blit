cmake_minimum_required(VERSION 3.1)
project (rocks-and-diamonds)
include (../32blit-beta/32blit.cmake)

# Get the tool from https://github.com/pimoroni/32blit-tools
find_program(
    32BLIT
    NAMES 32blit.exe 32blit
    HINTS
    $ENV{LOCALAPPDATA}/Programs/Python/Python38/Scripts/
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/assets.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/assets.hpp
   DEPENDS
        ${CMAKE_SOURCE_DIR}/assets.yml
        ${CMAKE_SOURCE_DIR}/assets/sprites.png
        ${CMAKE_SOURCE_DIR}/assets/level.tmx
    COMMAND cd ${CMAKE_SOURCE_DIR} && ${32BLIT} pack --force --config assets.yml --output ${CMAKE_CURRENT_BINARY_DIR}
)

blit_executable (rocks-and-diamonds rocks-and-diamonds.cpp assets.cpp)

target_include_directories(rocks-and-diamonds PRIVATE ${CMAKE_CURRENT_BINARY_DIR})