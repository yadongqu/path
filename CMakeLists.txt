cmake_minimum_required(VERSION 3.20)

project(path C)

add_subdirectory(extern/minifb)

add_executable(MathTest math_test.c vec.h)
add_test(Name MathTest COMMAND MathTest)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE minifb)