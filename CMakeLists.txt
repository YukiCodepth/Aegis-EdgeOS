cmake_minimum_required(VERSION 3.10)
project(AegisCore C)

# Enforce strict C standards and memory warnings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

include_directories(include)

# Compile main, monitor, AND our new serial driver into the executable
add_executable(aegis_controller src/main.c src/monitor.c src/serial.c)
