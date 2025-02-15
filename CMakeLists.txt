cmake_minimum_required(VERSION 3.31)

project(harmidica)

add_executable(harmidica main.c tunnel.c)
target_link_libraries(harmidica PUBLIC asound)
