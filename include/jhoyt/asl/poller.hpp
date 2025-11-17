// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <chrono>
#include <memory>
#include <span>

#include "common.hpp"
#include "socket_id.hpp"

namespace jhoyt::asl
{

    class ASL_API poller final
    {
    public:
        poller();
        ~poller();

        poller(const poller&) = delete;
        poller& operator=(const poller&) = delete;

        poller(poller&&) noexcept = default;
        poller& operator=(poller&&) noexcept = default;

        enum class poll_type
        {
            connect,
            read,
            read_write
        };

        void add_socket(socket_id id, poll_type type);

        void update_socket(socket_id id, poll_type type);

        void remove_socket(socket_id id);

        enum class poll_status
        {
            connection_succeeded,
            connection_failed,
            ready_to_read,
            ready_to_write
        };

        struct poll_result
        {
            socket_id id;
            poll_status status;
        };

        std::span<const poll_result> poll(const std::chrono::nanoseconds& timeout);

        template <typename Rep, typename Period>
        std::span<const poll_result> poll(const std::chrono::duration<Rep, Period>& timeout)
        {
            return poll(std::chrono::duration_cast<std::chrono::nanoseconds>(timeout));
        }

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
    };

} // namespace jhoyt::asl