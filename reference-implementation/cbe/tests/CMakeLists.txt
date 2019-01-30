# Basic GTest Testing Setup

# This assumes that you've added google test as a submodule in ../extern/googletest
# From the parent directory:
#     git submodule add https://github.com/google/googletest.git extern/googletest



#########
# Setup #
#########

# Give access to the internal header files
include_directories(${PROJECT_SOURCE_DIR}/src)

add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "extern/googletest")

# Keep the CACHE cleaner
mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

# Mark directories so that IDEs can clean them.
set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)

# Macro to add a test executable
macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    target_link_libraries(${TESTNAME} quadmath gtest gmock gtest_main ${LOCAL_PROJECT_NAMESPACE}::${LOCAL_PROJECT_NAME})
    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
    target_compile_options(${TESTNAME} PRIVATE $<$<CXX_COMPILER_ID:GNU>:
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
endmacro()



#########
# Tests #
#########

package_add_test(CBE
    src/cbe_decoder.cpp
    src/cbe_encoder.cpp
    src/encoding.cpp
    src/general_tests.cpp
    src/library.cpp
    src/stop_in_callback.cpp
    src/test_helpers.cpp
    src/test_utils.cpp
)
