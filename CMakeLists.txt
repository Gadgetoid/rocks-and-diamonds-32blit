cmake_minimum_required(VERSION 3.1)
project (rocks-and-diamonds)
include (../32blit-beta/32blit.cmake)
blit_executable (rocks-and-diamonds rocks-and-diamonds.cpp)
blit_assets (rocks-and-diamonds
    MAP_TILED2BIN level.tmx
    SPRITE_PACKED sprites.png)