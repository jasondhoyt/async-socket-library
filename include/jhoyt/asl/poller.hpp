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

    /// @brief Type that provides polling capabilities for sets of sockets.
    class ASL_API poller final
    {
    public:
        poller();
        ~poller();

        poller(const poller&) = delete;
        poller& operator=(const poller&) = delete;

        poller(poller&&) noexcept = default;
        poller& operator=(poller&&) noexcept = default;

        /// @brief Inner enumeration that defines possible ways to poll sockets for updates.
        enum class poll_type
        {
            /// @brief Poll for connection or disconnection state. This type will result in
            /// poll_status::connection_succeeded or poll_status::connection_failed statuses.
            connect,

            /// @brief Poll for read availability. This type will result in poll_status::ready_to_read if there is data
            /// available to be read from the socket.
            read,

            /// @brief Poll for write availability. This type will result in poll_status::ready_to_write if the socket
            /// can accept additioanl data for writing.
            read_write
        };

        /// @brief Add a new socket to the polling set.
        /// @param id The OS-level identifier for the socket to poll.
        /// @param type The type of polling that should occur for the socket.
        void add_socket(socket_id id, poll_type type);

        /// @brief Update the polling type for a specific socket in the polling set.
        /// @param id The OS-level identifier for the socket to poll.
        /// @param type The new type of polling that should occur for the socket.
        void update_socket(socket_id id, poll_type type);

        /// @brief Remove a socket from the polling set.
        /// @param id The OS-level identifier for the socket to remove.
        void remove_socket(socket_id id);

        /// @brief Inner enumeration that represents a poll status for a socket.
        enum class poll_status
        {
            /// @brief The associated socket has successfully connected.
            connection_succeeded,

            /// @brief The associated socket's connection attempt has failed.
            connection_failed,

            /// @brief The associated socket has data that can be read.
            ready_to_read,

            /// @brief The associated socket has space to write more data.
            ready_to_write
        };

        /// @brief Inner type that represents a single poll status for a socket.
        struct poll_result
        {
            socket_id id;
            poll_status status;
        };

        /// @brief Poll the set of sockets for updates.
        ///
        /// A timeout is provided in nanoseconds, although the underlying OS-level polling capabilities may have less
        /// resolution than that. The timeout is the maximum amount of time to wait for socket updates before
        /// returning. If socket updates have been detected then the function will return before the timeout has
        /// expired.
        ///
        /// @param timeout The number of nanoseconds to wait for updates to occur.
        /// @returns Sequence of socket polling results that occurred.
        std::span<const poll_result> poll(const std::chrono::nanoseconds& timeout);

        /// @brief Poll the set of sockets for updates.
        ///
        /// All timeouts are internally converted to nanoseconds, although the underlying OS-level polling capabilities
        /// may have less resolution than that. The timeout is the maximum amount of time to wait for socket updates
        /// before returning. If socket updates have been detected then the function will return before the timeout has
        /// expired.
        ///
        /// @param timeout The amount of time to wait for updates to occur.
        /// @returns Sequence of socket polling results that occurred.
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