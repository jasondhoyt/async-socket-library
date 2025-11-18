// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include <jhoyt/asl/address.hpp>

TEST_CASE("Address to::string")
{
    const auto ipv4_addr = jhoyt::asl::ipv4_address{.host = "127.0.0.1", .port = 5555};
    CHECK(jhoyt::asl::to_string(ipv4_addr) == "127.0.0.1:5555");

    const auto ipv6_addr = jhoyt::asl::ipv6_address{.host = "::1", .port = 5555};
    CHECK(jhoyt::asl::to_string(ipv6_addr) == "::1:5555");

    const auto file_addr = jhoyt::asl::file_address{.path = "./test.sock"};
    CHECK(jhoyt::asl::to_string(file_addr) == "./test.sock");

    SECTION("ipv4 generic address")
    {
        auto addr = jhoyt::asl::address{ipv4_addr};
        CHECK(jhoyt::asl::to_string(addr) == "ipv4://127.0.0.1:5555");
    }

    SECTION("ipv6 generic address")
    {
        auto addr = jhoyt::asl::address{ipv6_addr};
        CHECK(jhoyt::asl::to_string(addr) == "ipv6://::1:5555");
    }

    SECTION("file generic address")
    {
        auto addr = jhoyt::asl::address{file_addr};
        CHECK(jhoyt::asl::to_string(addr) == "file://./test.sock");
    }
}