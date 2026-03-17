cmake_minimum_required(VERSION 3.10)
project(AegisCore)

set(CMAKE_C_STANDARD 99)

# ---> UPDATE THIS LINE to include src/network.c <---
add_executable(aegis_controller src/main.c src/ai.c src/network.c)

# ---> ADD THIS NEW LINE directly below it <---
target_link_libraries(aegis_controller pthread)
