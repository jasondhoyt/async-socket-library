# Copyright (c) 2025-present, Jason Hoyt
# Distributed under the MIT License (http://opensource.org/licenses/MIT)

add_library(asl_mocks)

target_include_directories(asl_mocks PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        include
)

target_sources(asl_mocks PRIVATE
        src/mock_poller.cpp
        "${PROJECT_SOURCE_DIR}/src/address.cpp"
        "${PROJECT_SOURCE_DIR}/src/raw_address.cpp"
)

add_library(jhoyt::asl_mocks ALIAS asl_mocks)
