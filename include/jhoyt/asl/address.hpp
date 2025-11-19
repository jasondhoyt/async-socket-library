// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <string>
#include <variant>

namespace jhoyt::asl
{

    /// @brief Type that represents a host:port pair for an IPv4 address.
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

    /// @brief Type that represents a host:port pair for an IPv6 address.
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

    /// @brief Type that represents a file path for a UNIX domain address.
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

    /// @brief Type that represents any of the supported address types.
    using address = std::variant<ipv4_address, ipv6_address, file_address>;

    /// @brief Enumeration that matches the index() of the address variant.
    enum class address_type
    {
        ipv4,
        ipv6,
        file
    };

    /// @brief Helper function to retrieve the type from an address value.
    /// @param addr The address value to get the type of.
    /// @returns The type of the address value.
    inline auto get_address_type(const address& addr)
    {
        return static_cast<address_type>(addr.index());
    }

    /// @brief Helper function to return a string representation of an IPv4 address.
    /// @param addr The IPv4 address value to get the string representation of.
    /// @returns The string representation (in the form HOST:PORT) of the IPv4 address.
    std::string to_string(const ipv4_address& addr);

    /// @brief Helper function to return a string representation of an IPv6 address.
    /// @param addr The IPv6 address value to get the string representation of.
    /// @returns The string representation (in the form HOST:PORT) of the IPv6 address.
    std::string to_string(const ipv6_address& addr);

    /// @brief Helper function to return a string representation of a file (UNIX domain) address.
    /// @param addr The file address value to get the string representation of.
    /// @returns The string representation (in the form of a path) of the file address.
    std::string to_string(const file_address& addr);

    /// @brief Helper function to return a string representation of a generic address.
    ///
    /// The returned value will have a prefix added in the form of 'ipv4://', 'ipv6://' or 'file://' which designates
    /// what the underlying address type is.
    ///
    /// @param addr The generic address value to get the string representation of.
    /// @returns The string representation of the generic address.
    std::string to_string(const address& addr);

} // namespace jhoyt::asl