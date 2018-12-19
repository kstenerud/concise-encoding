# Download and unpack googletest at configure time
configure_file(${CMAKE_SOURCE_DIR}/cmake/GoogleTest-CMakeLists.txt.in ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(cbe_test
                   src/main.cpp
                   src/cbe_decoder.cpp
                   src/decode_encode.c
                   src/test_helpers.cpp
                   src/library.cpp
                   src/support.cpp
                   src/bool.cpp
                   src/int_8.cpp
                   src/int_16.cpp
                   src/int_32.cpp
                   src/int_64.cpp
                   src/int_128.cpp
                   src/float_32.cpp
                   src/float_64.cpp
                   src/float_128.cpp
                   src/decimal_32.cpp
                   src/decimal_64.cpp
                   src/decimal_128.cpp
                   src/time.cpp
                   src/string.cpp
                   src/list.cpp
                   src/map.cpp
                   src/binary_data.cpp
                   src/empty.cpp
                   src/encode_piecemeal.cpp
                   src/decode_piecemeal.cpp
                   src/invalid_document.cpp
                   src/stop_in_callback.cpp
               )

target_compile_features(cbe_test PRIVATE cxx_auto_type)
target_link_libraries(cbe_test gtest_main quadmath CBE::cbe)

# Gain access to internal headers
include_directories(${CMAKE_SOURCE_DIR}/src)

target_compile_options(cbe_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:
    -Wall
    -Wextra
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wrestrict
    -Wnull-dereference
    -Wdouble-promotion
    -Wshadow
    -fstrict-aliasing
    -Wformat=2
#    -pedantic
>)
