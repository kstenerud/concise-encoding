cmake_minimum_required(VERSION 3.5)
project(libcbe VERSION 1.0.0 LANGUAGES C CXX)

#Make sure that custom modules are found
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)


##############################################
# Create target and set properties

add_library(cbe
    src/decoder.c
    src/encoder.c
)

#Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(CBE::cbe ALIAS cbe)

#Set target properties
target_include_directories(cbe
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(cbe PRIVATE cxx_auto_type)
target_compile_options(cbe PRIVATE $<$<CXX_COMPILER_ID:GNU>:
    -Wall
    -Wextra
#    -Wduplicated-cond
#    -Wduplicated-branches
    -Wlogical-op
#    -Wrestrict
#    -Wnull-dereference
    -Wjump-misses-init
    -Wdouble-promotion
    -Wshadow
    -fstrict-aliasing
    -Wformat=2
>)

##############################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/CBE)

install(TARGETS cbe
    EXPORT cbe-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

#This is required so that the exported target has the name CBE and not cbe
set_target_properties(cbe PROPERTIES EXPORT_NAME CBE)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#Export the targets to a script
install(EXPORT cbe-targets
  FILE
    CBETargets.cmake
  NAMESPACE
    CBE::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/CBEConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/CBEConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CBEConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/CBEConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/CBEConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

export(EXPORT cbe-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/CBETargets.cmake NAMESPACE CBE::)

#Register package in user's package registry
export(PACKAGE CBE)

add_subdirectory(test)
