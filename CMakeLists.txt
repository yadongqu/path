cmake_minimum_required(VERSION 3.20)

project(flow CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE Flow_Source_Files src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${Flow_Source_Files})

include_directories(include)

add_subdirectory(tests)

# add_executable(${PROJECT_NAME} main.cpp flow_math.cpp scene_parser.cpp)
# add_subdirectory(extern/fast_float)
# target_include_directories(${PROJECT_NAME} PRIVATE extern/fast_float/include)

# add_subdirectory(extern/json)
# target_include_directories(${PROJECT_NAME} PRIVATE extern/json/include)

# target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
# add_subdirectory(extern/pugixml)
# target_link_libraries(${PROJECT_NAME} PRIVATE pugixml)
