// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "jhoyt/asl/socket.hpp"

namespace jhoyt::asl::mock
{

    void socket_reset();

    struct socket_call
    {
        socket_id id;
        std::chrono::steady_clock::time_point when;

        explicit socket_call(const socket_id id) : id(id), when(std::chrono::steady_clock::now())
        {
        }
    };

    struct socket_open_call : public socket_call
    {
        socket_domain arg_domain;
        socket_type arg_type;

        socket_open_call(const socket_id id, const socket_domain domain, const socket_type type)
            : socket_call(id), arg_domain(domain), arg_type(type)
        {
        }
    };

    void socket_set_open_throws(bool value);
    std::span<const socket_open_call> socket_get_open_calls();
    std::span<const socket_call> socket_get_close_calls();

    struct socket_set_reuse_address_option_call : public socket_call
    {
        bool arg_value;

        socket_set_reuse_address_option_call(const socket_id id, const bool value) : socket_call(id), arg_value(value)
        {
        }
    };

    void socket_set_set_reuse_address_option_throws(bool value);
    std::span<const socket_set_reuse_address_option_call> socket_get_set_reuse_address_option_calls();

    struct socket_shutdown_call : public socket_call
    {
        socket::shutdown_type arg_type;

        socket_shutdown_call(const socket_id id, const socket::shutdown_type type) : socket_call(id), arg_type(type)
        {
        }
    };

    void socket_set_shutdown_calls(bool value);
    std::span<const socket_shutdown_call> socket_get_shutdown_calls();

    struct socket_bind_or_connect_call : public socket_call
    {
        raw_address arg_addr;

        socket_bind_or_connect_call(const socket_id id, const raw_address& addr) : socket_call(id), arg_addr(addr)
        {
        }
    };

    void socket_set_bind_throws(bool value);
    std::span<const socket_bind_or_connect_call> socket_get_bind_calls();

    struct socket_listen_call : public socket_call
    {
        int arg_backlog;

        socket_listen_call(const socket_id id, const int backlog) : socket_call(id), arg_backlog(backlog)
        {
        }
    };

    void socket_set_listen_throws(bool value);
    std::span<const socket_listen_call> socket_get_listen_calls();

    void socket_add_connect_result(socket::connect_status status);
    void socket_set_connect_throws(bool value);
    std::span<const socket_bind_or_connect_call> socket_get_connect_calls();

    struct socket_accept_call : public socket_call
    {
        socket& arg_sock;
        raw_address& arg_addr;

        socket_accept_call(const socket_id id, socket& sock, raw_address& addr)
            : socket_call(id), arg_sock(sock), arg_addr(addr)
        {
        }
    };

    void socket_add_accept_result(bool value);
    void socket_set_accept_throws(bool value);
    std::span<const socket_accept_call> socket_get_accept_calls();

    struct socket_send_or_recv_call : public socket_call
    {
        std::span<const char> arg_data;

        socket_send_or_recv_call(const socket_id id, const std::span<const char> data) : socket_call(id), arg_data(data)
        {
        }
    };

    void socket_add_send_result(std::pair<socket::transfer_status, size_t> result);
    void socket_set_send_throws(bool value);
    std::span<const socket_send_or_recv_call> socket_get_send_calls();

    void socket_add_recv_result(std::pair<socket::transfer_status, size_t> result);
    void socket_set_recv_throws(bool value);
    std::span<const socket_send_or_recv_call> socket_get_recv_calls();

} // namespace jhoyt::asl::mock