# Copyright (c) 2025-present, Jason Hoyt
# Distributed under the MIT License (http://opensource.org/licenses/MIT)

set(BASE_PROJECT_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")

#
# Address
#

add_executable(asl_test_address
        test_address.cpp
        "${BASE_PROJECT_DIR}/src/address.cpp"
)

target_include_directories(asl_test_address PRIVATE "${BASE_PROJECT_DIR}/include")

target_link_libraries(asl_test_address PRIVATE Catch2::Catch2WithMain)

add_test(NAME asl_test_address COMMAND asl_test_address)

#
# Raw Address
#

add_executable(asl_test_raw_address
        test_raw_address.cpp
        "${BASE_PROJECT_DIR}/src/raw_address.cpp"
)

target_include_directories(asl_test_raw_address PRIVATE "${BASE_PROJECT_DIR}/include")

target_link_libraries(asl_test_raw_address PRIVATE Catch2::Catch2WithMain)

add_test(NAME asl_test_raw_address COMMAND asl_test_raw_address)