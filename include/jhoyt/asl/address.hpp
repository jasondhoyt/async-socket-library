// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <string>
#include <variant>

namespace jhoyt::asl
{

    struct ipv4_address
    {
        std::string host;
        uint16_t port;

        auto operator==(const ipv4_address& other) const
        {
            return host == other.host && port == other.port;
        }

        auto operator!=(const ipv4_address& other) const
        {
            return !(*this == other);
        }
    };

    struct ipv6_address
    {
        std::string host;
        uint16_t port;

        auto operator==(const ipv6_address& other) const
        {
            return host == other.host && port == other.port;
        }

        auto operator!=(const ipv6_address& other) const
        {
            return !(*this == other);
        }
    };

    struct file_address
    {
        std::string path;

        auto operator==(const file_address& other) const
        {
            return path == other.path;
        }

        auto operator!=(const file_address& other) const
        {
            return !(*this == other);
        }
    };

    using address = std::variant<ipv4_address, ipv6_address, file_address>;

    enum class address_type
    {
        ipv4,
        ipv6,
        file
    };

    inline auto get_address_type(const address& addr)
    {
        return static_cast<address_type>(addr.index());
    }

    std::string to_string(const ipv4_address& addr);
    std::string to_string(const ipv6_address& addr);
    std::string to_string(const file_address& addr);

    std::string to_string(const address& addr);

} // namespace jhoyt::asl