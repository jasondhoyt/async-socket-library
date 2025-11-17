// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <cassert>
#include <cerrno>
#include <format>
#include <stdexcept>

#if !defined(_WIN32)
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "jhoyt/asl/socket.hpp"

namespace
{
    using namespace jhoyt::asl;

    constexpr auto k_strerror_cap = 256;

#if !defined(_WIN32)
    constexpr auto k_socket_error = -1;
#endif

    int map_socket_domain(const socket_domain domain)
    {
        switch (domain)
        {

        case socket_domain::ipv4:
            return AF_INET;

        case socket_domain::ipv6:
            return AF_INET6;

        case socket_domain::file:
            return AF_UNIX;

        default:
            assert(false);
        }

        return 0;
    }

    int map_socket_type(const socket_type type)
    {
        switch (type)
        {
        case socket_type::stream:
            return SOCK_STREAM;

        default:
            assert(false);
        }

        return 0;
    }

    int map_shutdown_type(const socket::shutdown_type type)
    {
        switch (type)
        {
        case socket::shutdown_type::read:
            return SHUT_RD;

        case socket::shutdown_type::write:
            return SHUT_WR;

        case socket::shutdown_type::read_write:
            return SHUT_RDWR;

        default:
            assert(false);
        }

        return 0;
    }

    std::string make_socket_error_string(std::string_view msg)
    {
#if !defined(_WIN32)
        auto buf = std::array<char, k_strerror_cap>{};
        strerror_r(errno, buf.data(), buf.size());
        return std::format("{}: {}", msg, buf.data());
#endif
    }

    bool would_block()
    {
#if !defined(_WIN32)
        return errno == EINPROGRESS || errno == EAGAIN;
#endif
    }

} // namespace

namespace jhoyt::asl
{

    socket::~socket()
    {
        close();
    }

    socket::socket(socket& other) noexcept : sock_(other.sock_)
    {
        other.sock_ = k_invalid_socket;
    }

    socket& socket::operator=(socket&& other) noexcept
    {
        if (this != &other)
        {
            close();
            std::swap(sock_, other.sock_);
        }

        return *this;
    }

    void socket::open(socket_domain domain, socket_type type)
    {
        close();

        const auto tmp_sock = ::socket(map_socket_domain(domain), map_socket_type(type), 0);
        if (tmp_sock == k_invalid_socket)
        {
            throw std::runtime_error{make_socket_error_string("failed to open socket")};
        }

        // TODO : make socket non-blocking
#if !defined(_WIN32)
        auto flags = fcntl(tmp_sock, F_GETFL);
        if (flags == -1)
        {
            ::close(tmp_sock);
            throw std::runtime_error{make_socket_error_string("failed to get socket flags")};
        }

        flags |= O_NONBLOCK;
        if (fcntl(tmp_sock, F_SETFL, flags) == -1)
        {
            ::close(tmp_sock);
            throw std::runtime_error{make_socket_error_string("failed to set socket flags")};
        }
#else
        assert(false);
#endif

        sock_ = tmp_sock;
    }

    void socket::close()
    {
        if (sock_ != k_invalid_socket)
        {
#if !defined(_WIN32)
            ::close(sock_);
#else
            assert(false);
#endif

            sock_ = k_invalid_socket;
        }
    }

    void socket::set_reuse_address_option(bool value)
    {
        if (sock_ == k_invalid_socket)
        {
            throw std::runtime_error{"invalid socket for reuse address call"};
        }

        auto opt_value = value ? 1 : 0;
        if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &opt_value, sizeof(opt_value)) == k_socket_error)
        {
            throw std::runtime_error{make_socket_error_string("failed to set socket option for address reuse")};
        }
    }

    void socket::shutdown(const shutdown_type type)
    {
        if (sock_ != k_invalid_socket)
        {
            throw std::runtime_error{"invalid socket for shutdown call"};
        }

        if (::shutdown(sock_, map_shutdown_type(type)) == k_socket_error)
        {
            throw std::runtime_error{make_socket_error_string("failed to shutdown socket")};
        }
    }

    void socket::bind(const raw_address& addr)
    {
        if (sock_ == k_invalid_socket)
        {
            throw std::runtime_error{"invalid socket for bind call"};
        }

        const auto& addr_data = addr.get_data();
        if (::bind(sock_, reinterpret_cast<const sockaddr*>(addr_data.data()), addr_data.size()) == k_socket_error)
        {
            throw std::runtime_error{make_socket_error_string("failed to bind socket")};
        }
    }

    void socket::listen(int backlog)
    {
        if (sock_ == k_invalid_socket)
        {
            throw std::runtime_error{"invalid socket for listen call"};
        }

        if (::listen(sock_, backlog) == k_socket_error)
        {
            throw std::runtime_error{make_socket_error_string("failed to listen on socket")};
        }
    }

    socket::connect_status socket::connect(const raw_address& addr)
    {
        if (sock_ == k_invalid_socket)
        {
            throw std::runtime_error{"invalid socket for connect call"};
        }

        const auto& addr_data = addr.get_data();
        if (::connect(sock_, reinterpret_cast<const sockaddr*>(addr_data.data()), addr_data.size()) == k_socket_error)
        {
            if (would_block())
            {
                return connect_status::pending;
            }

            throw std::runtime_error{make_socket_error_string("failed to connect socket")};
        }

        return connect_status::connected;
    }

    std::pair<socket::transfer_status, size_t> socket::send(const std::span<const char> data)
    {
        if (sock_ == k_invalid_socket)
        {
            throw std::runtime_error{"invalid socket for send call"};
        }

        if (data.empty())
        {
            return {socket::transfer_status::success, 0};
        }

        auto count = ::send(sock_, data.data(), data.size(), 0);
        if (count == k_socket_error)
        {
            if (would_block())
            {
                return {socket::transfer_status::blocked, 0};
            }

            throw std::runtime_error{make_socket_error_string("failed to send on socket")};
        }
        else if (count == 0)
        {
            return {socket::transfer_status::disconnected, 0};
        }

        return {socket::transfer_status::success, count};
    }

    std::pair<socket::transfer_status, size_t> socket::recv(std::span<char> data)
    {
        if (sock_ == k_invalid_socket)
        {
            throw std::runtime_error{"invalid socket for recv call"};
        }

        if (data.empty())
        {
            return {socket::transfer_status::success, 0};
        }

        auto count = ::recv(sock_, data.data(), data.size(), 0);
        if (count == k_socket_error)
        {
            if (would_block())
            {
                return {socket::transfer_status::blocked, 0};
            }

            throw std::runtime_error{make_socket_error_string("failed to recv on socket")};
        }
        else if (count == 0)
        {
            return {socket::transfer_status::disconnected, 0};
        }

        return {socket::transfer_status::success, count};
    }

} // namespace jhoyt::asl