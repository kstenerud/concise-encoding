###############
# Google Test #
###############

# https://github.com/google/googletest/blob/master/googletest/README.md

# Use a local copy so it doesn't re-download every time the project files change.

# Prevent overriding the parent project's compiler/linker settings on Windows.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(googletest-src EXCLUDE_FROM_ALL)


#########
# Setup #
#########

# Give access to the internal header files
include_directories(${PROJECT_SOURCE_DIR}/src)

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

package_add_test(cte_tests
    src/helpers/cte_decoder.cpp
    src/helpers/cte_encoder.cpp
    src/helpers/encoding.cpp
    src/helpers/test_helpers.cpp
    src/helpers/test_utils.cpp

    src/comment.cpp
    src/general.cpp
)
