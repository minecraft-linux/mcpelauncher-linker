cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
enable_language(CXX)

project(android)

SET(TESTING OFF CACHE BOOL "Enable Tests")
if(TESTING)
  enable_testing()
  find_package(GTest)
  set(GTEST GTest::GTest GTest::Main)
  if (NOT GTest_FOUND)
      # Download and unpack googletest at configure time
      configure_file(GTest/CMakeLists.txt.in googletest-download/CMakeLists.txt)
      execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
              RESULT_VARIABLE result
              WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
      if(result)
          message(FATAL_ERROR "CMake step for googletest failed: ${result}")
      endif()
      execute_process(COMMAND ${CMAKE_COMMAND} --build .
              RESULT_VARIABLE result
              WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
      if(result)
          message(FATAL_ERROR "Build step for googletest failed: ${result}")
      endif()

      # Prevent overriding the parent project's compiler/linker
      # settings on Windows
      set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

      # Add googletest directly to our build. This defines
      # the gtest and gtest_main targets.
      add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
              ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
              EXCLUDE_FROM_ALL)

      # The gtest/gtest_main targets carry header search path
      # dependencies automatically when using CMake 2.8.11 or
      # later. Otherwise we have to add them here ourselves.
      if (CMAKE_VERSION VERSION_LESS 2.8.11)
          include_directories("${gtest_SOURCE_DIR}/include")
      endif()
      set(GTEST gtest gtest_main --coverage)

  endif()

include(GoogleTest)
include_directories(include)
include_directories(core/base/include)
add_executable(linked_list_test bionic/linker/linked_list_test.cpp)
target_link_libraries(linked_list_test ${GTEST})
gtest_discover_tests(linked_list_test)
add_executable(linker_block_allocator_test bionic/linker/linker_block_allocator_test.cpp bionic/linker/linker_block_allocator.cpp)
target_compile_definitions(linker_block_allocator_test PRIVATE PAGE_SIZE=4096 "prctl(a,b,c,d,e)=''" PR_SET_VMA=0 PR_SET_VMA_ANON_NAME=0)
target_link_libraries(linker_block_allocator_test ${GTEST})
gtest_discover_tests(linker_block_allocator_test)
endif()