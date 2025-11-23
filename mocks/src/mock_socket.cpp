// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <queue>

#include "jhoyt/asl/mocks/socket_mock.hpp"

namespace
{
    using namespace jhoyt::asl;

    auto g_next_socket_id = socket_id{1};

    auto g_open_throws = false;
    auto g_open_calls = std::vector<mock::socket_open_call>{};
    auto g_close_calls = std::vector<mock::socket_call>{};
    auto g_set_reuse_address_option_throws = false;
    auto g_set_reuse_address_option_calls = std::vector<mock::socket_set_reuse_address_option_call>{};
    auto g_shutdown_throws = false;
    auto g_shutdown_calls = std::vector<mock::socket_shutdown_call>{};
    auto g_bind_throws = false;
    auto g_bind_calls = std::vector<mock::socket_bind_or_connect_call>{};
    auto g_listen_throws = false;
    auto g_listen_calls = std::vector<mock::socket_listen_call>{};
    auto g_connect_results = std::queue<socket::connect_status>{};
    auto g_connect_throws = false;
    auto g_connect_calls = std::vector<mock::socket_bind_or_connect_call>{};
    auto g_accept_results = std::queue<bool>{};
    auto g_accept_throws = false;
    auto g_accept_calls = std::vector<mock::socket_accept_call>{};
    auto g_send_results = std::queue<std::pair<socket::transfer_status, size_t>>{};
    auto g_send_throws = false;
    auto g_send_calls = std::vector<mock::socket_send_or_recv_call>{};
    auto g_recv_results = std::queue<std::pair<socket::transfer_status, size_t>>{};
    auto g_recv_throws = false;
    auto g_recv_calls = std::vector<mock::socket_send_or_recv_call>{};

} // namespace

namespace jhoyt::asl
{

    socket::~socket() = default;

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
        g_open_calls.emplace_back(sock_, domain, type);

        if (g_open_throws)
        {
            throw std::runtime_error{"socket::open error"};
        }
    }

    void socket::close()
    {
        g_close_calls.emplace_back(sock_);
        sock_ = k_invalid_socket;
    }

    void socket::set_reuse_address_option(bool value)
    {
        g_set_reuse_address_option_calls.emplace_back(sock_, value);

        if (g_set_reuse_address_option_throws)
        {
            throw std::runtime_error{"socket::set_reuse_address_option error"};
        }
    }

    void socket::shutdown(shutdown_type type)
    {
        g_shutdown_calls.emplace_back(sock_, type);

        if (g_shutdown_throws)
        {
            throw std::runtime_error{"socket::shutdown error"};
        }
    }

    void socket::bind(const raw_address& addr)
    {
        g_bind_calls.emplace_back(sock_, addr);

        if (g_bind_throws)
        {
            throw std::runtime_error{"socket::bind error"};
        }
    }

    void socket::listen(int backlog)
    {
        g_listen_calls.emplace_back(sock_, backlog);

        if (g_listen_throws)
        {
            throw std::runtime_error{"socket::listen error"};
        }
    }

    socket::connect_status socket::connect(const raw_address& addr)
    {
        g_connect_calls.emplace_back(sock_, addr);

        if (g_connect_throws)
        {
            throw std::runtime_error{"socket::connect error"};
        }

        if (!g_connect_results.empty())
        {
            const auto result = g_connect_results.front();
            g_connect_results.pop();
            return result;
        }

        return socket::connect_status::pending;
    }

    bool socket::accept(socket& sock, raw_address& addr)
    {
        g_accept_calls.emplace_back(sock_, sock, addr);

        if (g_accept_throws)
        {
            throw std::runtime_error{"socket::accept error"};
        }

        if (!g_accept_results.empty())
        {
            const auto result = g_accept_results.front();
            g_accept_results.pop();
            return result;
        }

        return false;
    }

    std::pair<socket::transfer_status, size_t> socket::send(std::span<const char> data)
    {
        g_send_calls.emplace_back(sock_, data);

        if (g_send_throws)
        {
            throw std::runtime_error{"socket::send error"};
        }

        if (!g_send_results.empty())
        {
            const auto result = g_send_results.front();
            g_send_results.pop();
            return result;
        }

        return std::make_pair(socket::transfer_status::disconnected, 0);
    }

    std::pair<socket::transfer_status, size_t> socket::recv(std::span<char> data)
    {
        g_recv_calls.emplace_back(sock_, data);

        if (g_recv_throws)
        {
            throw std::runtime_error{"socket::recv error"};
        }

        if (!g_recv_results.empty())
        {
            const auto result = g_recv_results.front();
            g_recv_results.pop();
            return result;
        }

        return std::make_pair(socket::transfer_status::disconnected, 0);
    }

    namespace mock
    {

        void socket_reset()
        {
            g_next_socket_id = 1;
            g_open_throws = false;
            g_open_calls.clear();
            g_close_calls.clear();
            g_set_reuse_address_option_throws = false;
            g_set_reuse_address_option_calls.clear();
            g_shutdown_throws = false;
            g_shutdown_calls.clear();
            g_bind_throws = false;
            g_bind_calls.clear();
            g_listen_throws = false;
            g_listen_calls.clear();
            while (!g_connect_results.empty())
            {
                g_connect_results.pop();
            }
            g_connect_throws = false;
            g_connect_calls.clear();
            while (!g_accept_results.empty())
            {
                g_accept_results.pop();
            }
            g_accept_throws = false;
            g_accept_calls.clear();
            while (!g_send_results.empty())
            {
                g_send_results.pop();
            }
            g_send_throws = false;
            g_send_calls.clear();
            while (!g_recv_results.empty())
            {
                g_recv_results.pop();
            }
            g_recv_throws = false;
            g_recv_calls.clear();
        }

        void socket_set_open_throws(bool value)
        {
            g_open_throws = value;
        }

        std::span<const socket_open_call> socket_get_open_calls()
        {
            return g_open_calls;
        }

        std::span<const socket_call> socket_get_close_calls()
        {
            return g_close_calls;
        }

        void socket_set_set_reuse_address_option_throws(const bool value)
        {
            g_set_reuse_address_option_throws = value;
        }

        std::span<const socket_set_reuse_address_option_call> socket_get_set_reuse_address_option_calls()
        {
            return g_set_reuse_address_option_calls;
        }

        void socket_set_shutdown_calls(const bool value)
        {
            g_shutdown_throws = value;
        }

        std::span<const socket_shutdown_call> socket_get_shutdown_calls()
        {
            return g_shutdown_calls;
        }

        void socket_set_bind_throws(const bool value)
        {
            g_bind_throws = value;
        }

        std::span<const socket_bind_or_connect_call> socket_get_bind_calls()
        {
            return g_bind_calls;
        }

        void socket_set_listen_throws(const bool value)
        {
            g_listen_throws = value;
        }

        std::span<const socket_listen_call> socket_get_listen_calls()
        {
            return g_listen_calls;
        }

        void socket_add_connect_result(socket::connect_status status)
        {
            g_connect_results.emplace(status);
        }

        void socket_set_connect_throws(const bool value)
        {
            g_connect_throws = value;
        }

        std::span<const socket_bind_or_connect_call> socket_get_connect_calls()
        {
            return g_connect_calls;
        }

        void socket_add_accept_result(const bool value)
        {
            g_accept_results.emplace(value);
        }

        void socket_set_accept_throws(const bool value)
        {
            g_accept_throws = value;
        }

        std::span<const socket_accept_call> socket_get_accept_calls()
        {
            return g_accept_calls;
        }

        void socket_add_send_result(std::pair<socket::transfer_status, size_t> result)
        {
            g_send_results.emplace(std::move(result));
        }

        void socket_set_send_throws(const bool value)
        {
            g_send_throws = value;
        }

        std::span<const socket_send_or_recv_call> socket_get_send_calls()
        {
            return g_send_calls;
        }
        void socket_add_recv_result(std::pair<socket::transfer_status, size_t> result)
        {
            g_recv_results.emplace(std::move(result));
        }

        void socket_set_recv_throws(const bool value)
        {
            g_recv_throws = value;
        }

        std::span<const socket_send_or_recv_call> socket_get_recv_calls()
        {
            return g_recv_calls;
        }

    } // namespace mock

} // namespace jhoyt::asl