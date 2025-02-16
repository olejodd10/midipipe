cmake_minimum_required(VERSION 3.31)

project(midipipe)

add_executable(midipipe main.c midipipe.c)
target_link_libraries(midipipe PUBLIC asound)
