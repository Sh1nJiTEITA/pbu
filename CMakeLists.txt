cmake_minimum_required(VERSION 3.28)
project(pbu)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_library(pbu SHARED src/pbu.cpp include/pbu.h src/bar_string.cpp
                       include/bar_string.h src/str.cpp include/str.h)

target_include_directories(pbu PUBLIC include)

install(TARGETS pbu DESTINATION lib)
install(FILES include/pbu.h DESTINATION include)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(
  uninstall COMMAND ${CMAKE_COMMAND} -P
                    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

set(BUILD_TESTS ON)

if (${BUILD_TESTS})
add_executable(pbu_test test.cpp src/pbu.cpp include/pbu.h src/str.cpp
                        include/str.h)
target_include_directories(pbu_test PUBLIC include)
find_package(catch2)
target_link_libraries(pbu_test PRIVATE Catch2::Catch2WithMain)
endif (${BUILD_TESTS})

