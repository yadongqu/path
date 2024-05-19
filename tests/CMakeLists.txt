find_package(Catch2 3 REQUIRED)
add_executable(PBRTTest PBRTTest.cpp)
target_link_libraries(PBRTTest PRIVATE Catch2::Catch2WithMain)
