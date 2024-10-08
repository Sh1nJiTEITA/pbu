cmake_minimum_required(VERSION 3.28)
project(pbu)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES src/pbu.cpp
            # src/str.cpp
)

set(HEADERS
    include/pbu.h
    # include/str.h
    include/vec.hpp include/allocator.hpp include/flist.hpp include/strv.hpp)

add_library(pbu SHARED ${SOURCES} ${HEADERS})

target_include_directories(pbu PUBLIC include)

install(TARGETS pbu DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(
  uninstall COMMAND ${CMAKE_COMMAND} -P
                    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

set(BUILD_TESTS ON)

if(${BUILD_TESTS})

  set(TEST_SOURCES # test/test.cpp
      test/test_allocator.cpp test/test_vec.cpp
      test/test_presentAllocationInfo.cpp test/test_flist.cpp
      test/test_strv.cpp)

  add_executable(pbu_test ${TEST_SOURCES} ${SOURCES} ${HEADERS})
  target_include_directories(pbu_test PUBLIC include)
  find_package(Catch2)
  target_link_libraries(pbu_test PRIVATE Catch2::Catch2WithMain)
endif(${BUILD_TESTS})
