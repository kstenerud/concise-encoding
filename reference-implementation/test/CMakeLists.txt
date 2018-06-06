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
                   src/library.cpp
                   src/encode_support.cpp
                   src/encode_bool.cpp
                   src/encode_int_8.cpp
                   src/encode_int_16.cpp
                   src/encode_int_32.cpp
                   src/encode_int_64.cpp
                   src/encode_int_128.cpp
                   src/encode_float_32.cpp
                   src/encode_float_64.cpp
                   src/encode_float_128.cpp
                   src/encode_date.cpp
                   src/encode_string.cpp
                   src/encode_list.cpp
                   src/encode_map.cpp
                   src/encode_array.cpp
                   src/encode_empty.cpp
                   src/roundtrip_reencode.c
                   src/decode_int.cpp
               )

target_compile_features(cbe_test PRIVATE cxx_auto_type)
target_link_libraries(cbe_test gtest_main CBE::cbe)

# Gain access to internal headers
include_directories(${CMAKE_SOURCE_DIR}/src)
