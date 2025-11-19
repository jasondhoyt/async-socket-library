// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(_WIN32)
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#endif

#include <catch.hpp>

#include <jhoyt/asl/address.hpp>

#include "jhoyt/asl/raw_address.hpp"

namespace
{

    constexpr auto k_too_long_of_path =
        "This is a string of text that will overflow the file address storage. It is intended to be used to validate "
        "that the raw address structure does not accept file paths that are too long.";

    auto is_zero(const std::span<const char> data)
    {
        for (const auto byte : data)
        {
            if (byte != 0)
            {
                return false;
            }
        }

        return true;
    }

    auto is_equal(const std::span<const char> first, const std::span<const char> second)
    {
        if (first.size() != second.size())
        {
            return false;
        }

        for (auto ix = size_t{0}; ix < first.size(); ++ix)
        {
            if (first[ix] != second[ix])
            {
                return false;
            }
        }

        return true;
    }
} // namespace

namespace jhoyt::asl
{
    std::string to_string(const jhoyt::asl::address&)
    {
        return "address string";
    }
} // namespace jhoyt::asl

TEST_CASE("Empty Raw Address")
{
    const auto addr = jhoyt::asl::raw_address{};
    CHECK(is_zero(addr.get_data()));
    CHECK_THROWS(jhoyt::asl::to_string(addr));
}

TEST_CASE("Raw Address from Address")
{
    CHECK_THROWS(jhoyt::asl::raw_address{jhoyt::asl::ipv4_address{.host = {}, .port = 0}});
    CHECK_THROWS(jhoyt::asl::raw_address{jhoyt::asl::ipv4_address{.host = "malformed", .port = 0}});
    CHECK_THROWS(jhoyt::asl::raw_address{jhoyt::asl::ipv4_address{.host = ".127.0.1.1", .port = 0}});
    CHECK_THROWS(jhoyt::asl::raw_address{jhoyt::asl::ipv4_address{.host = "::1", .port = 0}});

    CHECK_THROWS(jhoyt::asl::raw_address{jhoyt::asl::ipv6_address{.host = {}, .port = 0}});
    CHECK_THROWS(jhoyt::asl::raw_address{jhoyt::asl::ipv6_address{.host = "malformed", .port = 0}});
    CHECK_THROWS(jhoyt::asl::raw_address{jhoyt::asl::ipv6_address{.host = ".127.0.1.1", .port = 0}});
    CHECK_THROWS(jhoyt::asl::raw_address{jhoyt::asl::ipv6_address{.host = "127.0.0.1", .port = 0}});

    CHECK_THROWS(jhoyt::asl::raw_address{jhoyt::asl::file_address{.path = {}}});
    CHECK_THROWS(jhoyt::asl::raw_address{jhoyt::asl::file_address{.path = k_too_long_of_path}});

    const auto addr_ipv4 = jhoyt::asl::raw_address{jhoyt::asl::ipv4_address{.host = "127.0.0.1", .port = 5555}};
    CHECK(!is_zero(addr_ipv4.get_data()));
    CHECK_NOTHROW(jhoyt::asl::to_string(addr_ipv4));

    const auto addr_ipv6 = jhoyt::asl::raw_address{jhoyt::asl::ipv6_address{.host = "::1", .port = 5555}};
    CHECK(!is_zero(addr_ipv6.get_data()));
    CHECK_NOTHROW(jhoyt::asl::to_string(addr_ipv6));

    const auto addr_file = jhoyt::asl::raw_address{jhoyt::asl::file_address{.path = "./test.sock"}};
    CHECK(!is_zero(addr_file.get_data()));
    CHECK_NOTHROW(jhoyt::asl::to_string(addr_file));
}

TEST_CASE("Raw Address from Bytes")
{
    auto addr_storage = sockaddr_storage{0};
    CHECK_THROWS(
        jhoyt::asl::raw_address{std::span{reinterpret_cast<const char*>(&addr_storage), sizeof(addr_storage)}});

    addr_storage.ss_family = 255;
    CHECK_THROWS(
        jhoyt::asl::raw_address{std::span{reinterpret_cast<const char*>(&addr_storage), sizeof(addr_storage)}});

    addr_storage.ss_family = AF_INET;
    addr_storage.ss_len = 10;
    CHECK_THROWS(
        jhoyt::asl::raw_address{std::span{reinterpret_cast<const char*>(&addr_storage), sizeof(addr_storage)}});

    auto good_raw_addr = jhoyt::asl::raw_address{jhoyt::asl::ipv4_address{.host = "127.0.0.1", .port = 5555}};
    const auto good_raw_addr_data = good_raw_addr.get_data();
    auto raw_addr = jhoyt::asl::raw_address{good_raw_addr_data};
    const auto new_raw_addr_data = raw_addr.get_data();
    CHECK(new_raw_addr_data.size() == good_raw_addr_data.size());
    CHECK(is_equal(new_raw_addr_data, good_raw_addr_data));
    CHECK_NOTHROW(jhoyt::asl::to_string(raw_addr));
}

TEST_CASE("Raw Address Data")
{
    SECTION("Null Raw Address Data")
    {
        const auto addr = jhoyt::asl::raw_address{};
        CHECK(is_zero(addr.get_data()));
    }

    SECTION("IPv4 Raw Address Data")
    {
        const auto addr = jhoyt::asl::raw_address{jhoyt::asl::ipv4_address{.host = "127.0.0.1", .port = 5555}};
        const auto addr_data = addr.get_data();
        CHECK(!is_zero(addr_data));

        const auto& storage = *reinterpret_cast<const sockaddr_storage*>(addr_data.data());
        CHECK(storage.ss_family == AF_INET);
        CHECK(storage.ss_len == sizeof(sockaddr_in));
    }

    SECTION("IPv6 Raw Address Data")
    {
        const auto addr = jhoyt::asl::raw_address{jhoyt::asl::ipv6_address{.host = "::1", .port = 5555}};
        const auto addr_data = addr.get_data();
        CHECK(!is_zero(addr_data));

        const auto& storage = *reinterpret_cast<const sockaddr_storage*>(addr_data.data());
        CHECK(storage.ss_family == AF_INET6);
        CHECK(storage.ss_len == sizeof(sockaddr_in6));
    }

    SECTION("File Raw Address Data")
    {
        const auto addr = jhoyt::asl::raw_address{jhoyt::asl::file_address{.path = "./test.sock"}};
        const auto addr_data = addr.get_data();
        CHECK(!is_zero(addr_data));

        const auto& storage = *reinterpret_cast<const sockaddr_storage*>(addr_data.data());
        CHECK(storage.ss_family == AF_UNIX);
        CHECK(storage.ss_len == sizeof(sockaddr_un));
    }
}

TEST_CASE("Raw Address to Address")
{
    SECTION("Null Raw Address")
    {
        const auto raw_addr = jhoyt::asl::raw_address{};
        CHECK_THROWS(raw_addr.get_address());
    }

    SECTION("IPv4 Raw Address")
    {
        const auto ipv4_addr = jhoyt::asl::address{jhoyt::asl::ipv4_address{.host = "127.0.0.1", .port = 5555}};
        const auto raw_addr = jhoyt::asl::raw_address{ipv4_addr};
        const auto new_addr = raw_addr.get_address();
        CHECK(new_addr == ipv4_addr);
    }

    SECTION("IPv6 Raw Address")
    {
        const auto ipv6_addr = jhoyt::asl::address{jhoyt::asl::ipv6_address{.host = "::1", .port = 5555}};
        const auto raw_addr = jhoyt::asl::raw_address{ipv6_addr};
        const auto new_addr = raw_addr.get_address();
        CHECK(new_addr == ipv6_addr);
    }

    SECTION("File Raw Address")
    {
        const auto file_addr = jhoyt::asl::address{jhoyt::asl::file_address{.path = "./test.sock"}};
        const auto raw_addr = jhoyt::asl::raw_address{file_addr};
        const auto new_addr = raw_addr.get_address();
        CHECK(new_addr == file_addr);
    }
}

TEST_CASE("Raw Address to String")
{
    const auto addr_zero = jhoyt::asl::raw_address{};
    CHECK_THROWS(jhoyt::asl::to_string(addr_zero));

    const auto addr_ipv4 = jhoyt::asl::raw_address{jhoyt::asl::ipv4_address{.host = "127.0.0.1", .port = 5555}};
    CHECK(jhoyt::asl::to_string(addr_ipv4) == "address string");

    const auto addr_ipv6 = jhoyt::asl::raw_address{jhoyt::asl::ipv6_address{.host = "::1", .port = 5555}};
    CHECK(jhoyt::asl::to_string(addr_ipv6) == "address string");

    const auto addr_file = jhoyt::asl::raw_address{jhoyt::asl::file_address{.path = "./test.sock"}};
    CHECK(jhoyt::asl::to_string(addr_file) == "address string");
}