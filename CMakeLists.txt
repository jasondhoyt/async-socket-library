# Copyright (c) 2025-present, Jason Hoyt
# Distributed under the MIT License (http://opensource.org/licenses/MIT)

cmake_minimum_required(VERSION 3.10...3.21)

include(cmake/utils.cmake)
include(cmake/ide.cmake)

asl_extract_version()

project(asl VERSION ${ASL_VERSION} LANGUAGES CXX)
message(STATUS "Build asl: ${ASL_VERSION}")

#
# Set default build to release
#

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    # Set CMAKE_BUILD_TYPE only if this project is top-level
    if ((DEFINED PROJECT_IS_TOP_LEVEL AND PROJECT_IS_TOP_LEVEL)
            OR (NOT DEFINED PROJECT_IS_TOP_LEVEL AND CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR))
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
    endif ()
endif ()

#
# Compiler configuration
#

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_EXTENSIONS OFF)

#
# Setup primary project if this library is being built
#

if (NOT DEFINED ASL_PRIMARY_PROJECT)
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(ASL_PRIMARY_PROJECT ON)
    else ()
        set(ASL_PRIMARY_PROJECT OFF)
    endif ()
endif ()

#
# Setup various build options
#

option(ASL_BUILD_SHARED "Build shared library" OFF)
option(ASL_BUILD_PIC "Build position independent code (-fPIC)" OFF)
set(ASL_DEBUG_POSTFIX "d" CACHE STRING "Filename postfix for libraries in debug builds")
option(ASL_BUILD_TESTS "Build tests" OFF)
option(ASL_BUILD_WARNINGS "Enable compiler warnings" OFF)

# TODO : install options

if (ASL_BUILD_PIC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif ()

#
# The library itself
#

set(ASL_SOURCES
        src/raw_address.cpp
        src/socket.cpp
)

if (ASL_BUILD_SHARED OR BUILD_SHARED_LIBS)
    if (WIN32)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)
        list(APPEND ASL_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
    endif ()

    add_library(asl SHARED ${ASL_SOURCES} ${ASL_ALL_HEADERS})
else ()
    add_library(asl STATIC ${ASL_SOURCES} ${ASL_ALL_HEADERS})
endif ()

target_include_directories(asl PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>")

set_target_properties(asl PROPERTIES VERSION ${ASL_VERSION} SOVERSION ${ASL_VERSION_MAJOR}.${ASL_VERSION_MINOR})

set_target_properties(asl PROPERTIES DEBUG_POSTFIX ${ASL_DEBUG_POSTFIX})

#
# Unit tests
#

if (ASL_BUILD_TESTS OR ASL_BUILD_ALL)
    message(STATUS "Generating tests")
    enable_testing()
    # TODO
endif ()

