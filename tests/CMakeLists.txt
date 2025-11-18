# Copyright (c) 2025-present, Jason Hoyt
# Distributed under the MIT License (http://opensource.org/licenses/MIT)

cmake_minimum_required(VERSION 3.11)

project(asl_tests CXX)

include(../cmake/utils.cmake)

find_package(Catch2 3 QUIET)
if (Catch2_FOUND)
    message(STATUS "Packaged version of Catch will be used.")
else ()
    message(STATUS "Bundled version of Catch will be downloaded and used.")
    include(FetchContent)
    FetchContent_Declare(
            Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG 53d0d913a422d356b23dd927547febdf69ee9081 # v3.5.0
    )
    FetchContent_MakeAvailable(Catch2)
endif ()

enable_testing()

add_executable(asl_system_tests system_tests.cpp)

target_link_libraries(asl_system_tests PRIVATE
        jhoyt::asl
        Catch2::Catch2WithMain
)

add_test(NAME asl_system_tests COMMAND asl_system_tests)

add_subdirectory(unit)