// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <cassert>
#include <format>
#include <stdexcept>

#if !defined(_WIN32)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/un.h>
#endif

#include "jhoyt/asl/raw_address.hpp"

namespace
{
    using namespace jhoyt::asl;

    sockaddr_storage storage_from_address(const address& addr)
    {
        auto storage = sockaddr_storage{0};
        switch (get_address_type(addr))
        {

        case address_type::ipv4: {
            const auto& [host, port] = *std::get_if<ipv4_address>(&addr);
            auto& ipv4_out = *reinterpret_cast<sockaddr_in*>(&storage);
            if (const auto status = inet_pton(AF_INET, host.c_str(), &ipv4_out.sin_addr); status != 1)
            {
                assert(status != -1);
                throw std::runtime_error{"malformed ipv4 address host"};
            }

            ipv4_out.sin_family = AF_INET;
            ipv4_out.sin_port = htons(port);

            storage.ss_len = sizeof(sockaddr_in);
            break;
        }

        case address_type::ipv6: {
            const auto& [host, port] = *std::get_if<ipv6_address>(&addr);
            auto& ipv6_out = *reinterpret_cast<sockaddr_in6*>(&storage);
            if (const auto status = inet_pton(AF_INET6, host.c_str(), &ipv6_out.sin6_addr); status != 1)
            {
                assert(status != -1);
                throw std::runtime_error{"malformed ipv6 address host"};
            }

            ipv6_out.sin6_family = AF_INET6;
            ipv6_out.sin6_port = htons(port);

            storage.ss_len = sizeof(sockaddr_in6);
            break;
        }

        case address_type::file: {
            const auto& [path] = *std::get_if<file_address>(&addr);
            auto& file_out = *reinterpret_cast<sockaddr_un*>(&storage);
            if (path.empty())
            {
                throw std::runtime_error{"file address path is empty"};
            }

            if (path.length() > sizeof(file_out.sun_path) - 1)
            {
                throw std::runtime_error{"file address path is too long"};
            }

            memcpy(file_out.sun_path, path.data(), path.length());

            file_out.sun_family = AF_UNIX;

            storage.ss_len = sizeof(sockaddr_un);
            break;
        }

        default:
            throw std::runtime_error{std::format("unsupported address type: {}", addr.index())};
        }

        return storage;
    }

    sockaddr_storage storage_from_raw_data(const std::span<const char> data)
    {
        if (data.size() > sizeof(sockaddr_storage))
        {
            throw std::runtime_error{"malformed raw address data"};
        }

        const auto& data_storage = *reinterpret_cast<const sockaddr_storage*>(data.data());
        switch (data_storage.ss_family)
        {

        case AF_INET:
            if (data_storage.ss_len != sizeof(sockaddr_in))
            {
                throw std::runtime_error{"malformed ipv4 raw address data"};
            }
            break;

        case AF_INET6:
            if (data_storage.ss_len != sizeof(sockaddr_in6))
            {
                throw std::runtime_error{"malformed ipv6 raw address data"};
            }
            break;

        case AF_UNIX:
            if (data_storage.ss_len != sizeof(sockaddr_un))
            {
                throw std::runtime_error{"malformed file raw address data"};
            }
            break;

        default:
            throw std::runtime_error{std::format("unsupported raw address data: {}", data_storage.ss_family)};
        }

        return data_storage;
    }

} // namespace

namespace jhoyt::asl
{

    raw_address::raw_address()
    {
        memset(&data_, 0, sizeof(data_));
    }

    raw_address::raw_address(const address& addr) : data_(storage_from_address(addr))
    {
    }

    raw_address::raw_address(const std::span<const char> data) : data_(storage_from_raw_data(data))
    {
    }

    std::span<const char> raw_address::get_data() const
    {
        return {reinterpret_cast<const char*>(&data_), data_.ss_len};
    }

    address raw_address::get_address() const
    {
        switch (data_.ss_family)
        {

        case AF_INET: {
            assert(data_.ss_len == sizeof(sockaddr_in));
            const auto& ipv4_data = *reinterpret_cast<const sockaddr_in*>(&data_);
            auto buf = std::array<char, INET_ADDRSTRLEN>{};
            if (!inet_ntop(AF_INET, &ipv4_data.sin_addr, buf.data(), buf.size()))
            {
                throw std::runtime_error{"malformed ipv4 raw address host"};
            }

            return ipv4_address{.host = buf.data(), .port = ntohs(ipv4_data.sin_port)};
        }

        case AF_INET6: {
            assert(data_.ss_len == sizeof(sockaddr_in6));
            const auto& ipv6_data = *reinterpret_cast<const sockaddr_in6*>(&data_);
            auto buf = std::array<char, INET6_ADDRSTRLEN>{};
            if (!inet_ntop(AF_INET6, &ipv6_data.sin6_addr, buf.data(), buf.size()))
            {
                throw std::runtime_error{"malformed ipv6 raw address host"};
            }

            return ipv6_address{.host = buf.data(), .port = ntohs(ipv6_data.sin6_port)};
        }

        case AF_UNIX: {
            assert(data_.ss_len == sizeof(sockaddr_un));
            const auto& unix_data = *reinterpret_cast<const sockaddr_un*>(&data_);
            return file_address{.path = unix_data.sun_path};
        }

        default:
            throw std::runtime_error{std::format("unsupported raw address type: {}", data_.ss_family)};
        }
    }

    std::string to_string(const raw_address& addr)
    {
        return to_string(addr.get_address());
    }

} // namespace jhoyt::asl