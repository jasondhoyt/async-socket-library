// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <cassert>
#include <format>

#include "jhoyt/asl/address.hpp"

namespace jhoyt::asl
{
    std::string to_string(const ipv4_address& addr)
    {
        return std::format("{}:{}", addr.host, addr.port);
    }

    std::string to_string(const ipv6_address& addr)
    {
        return std::format("{}:{}", addr.host, addr.port);
    }

    std::string to_string(const file_address& addr)
    {
        return addr.path;
    }

    std::string to_string(const address& addr)
    {
        switch (get_address_type(addr))
        {

        case address_type::ipv4: {
            const auto& [host, port] = *std::get_if<ipv4_address>(&addr);
            return std::format("ipv4://{}:{}", host, port);
        }

        case address_type::ipv6: {
            const auto& [host, port] = *std::get_if<ipv6_address>(&addr);
            return std::format("ipv6://{}:{}", host, port);
        }

        case address_type::file: {
            const auto& [path] = *std::get_if<file_address>(&addr);
            return std::format("file://{}", path);
        }

        default:
            assert(false);
        }
    }

} // namespace jhoyt::asl