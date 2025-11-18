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
    };

    struct ipv6_address
    {
        std::string host;
        uint16_t port;
    };

    struct file_address
    {
        std::string path;
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