// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "jhoyt/asl/poller.hpp"
#include "jhoyt/asl/socket_id.hpp"

namespace jhoyt::asl::mock
{
    void poller_reset();

    struct poller_modify_socket_call
    {
        socket_id arg_id;
        poller::poll_type arg_type;
        std::chrono::steady_clock::time_point when;

        poller_modify_socket_call(const socket_id id, const poller::poll_type type)
            : arg_id(id), arg_type(type), when(std::chrono::steady_clock::now())
        {
        }
    };

    std::span<const poller_modify_socket_call> poller_get_add_socket_calls();
    std::span<const poller_modify_socket_call> poller_get_update_socket_calls();

    struct poller_remove_socket_call
    {
        socket_id arg_id;
        std::chrono::steady_clock::time_point when;

        explicit poller_remove_socket_call(const socket_id id) : arg_id(id), when(std::chrono::steady_clock::now())
        {
        }
    };

    std::span<const poller_remove_socket_call> poller_get_remove_socket_calls();

    struct poller_poll_call
    {
        std::chrono::nanoseconds arg_timeout;
        std::chrono::steady_clock::time_point when;

        explicit poller_poll_call(const std::chrono::nanoseconds& timeout)
            : arg_timeout(timeout), when(std::chrono::steady_clock::now())
        {
        }
    };

    void poller_enqueue_poll_result(poller::poll_result result);
    void poller_set_poll_throws(bool value);
    std::span<poller_poll_call> poller_get_poll_calls();

} // namespace jhoyt::asl::mock