// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "common.hpp"
#include "raw_address.hpp"
#include "socket_domain.hpp"
#include "socket_id.hpp"
#include "socket_type.hpp"

namespace jhoyt::asl
{

    /// @brief Type that wraps an OS-specific socket and the operations that can be performed on it.
    class ASL_API socket final
    {
    public:
        socket() = default;
        ~socket();

        socket(const socket&) = delete;
        socket& operator=(const socket&) = delete;

        socket(socket& other) noexcept;
        socket& operator=(socket&& other) noexcept;

        /// @brief Retrieve the OS-level identifier of the socket, or k_invalid_socket if there is not one.
        /// @returns The OS-level identifier for the socket.
        [[nodiscard]] auto get_id() const
        {
            return sock_;
        }

        /// @brief Check if the OS-level identifier for the socket is valid or not.
        /// @returns True if the OS-level identifier for the socket is valid, otherwise false.
        explicit operator bool() const
        {
            return sock_ != k_invalid_socket;
        }

        /// @brief Opens a new socket.
        /// @param domain The socket domain to use.
        /// @param type The socket type to use.
        void open(socket_domain domain, socket_type type);

        /// @brief Closes an existing socket.
        void close();

        /// @brief Enable or disable the socket-level reuse address option.
        /// @param value The value of the option to set.
        void set_reuse_address_option(bool value);

        /// @brief Inner enumeration that represents what side of a socket to shut down.
        enum class shutdown_type
        {
            /// @brief Shut down only the read side of the socket.
            read,

            /// @brief Shut down only the write side of the socket.
            write,

            /// @brief Shut down both the read and write sides of the socket.
            read_write
        };

        /// @brief Shut down one or both ends (read and write) of a socket for further communications.
        /// @param type The type of shut down to perform on the socket.
        void shutdown(shutdown_type type);

        /// @brief Bind the socket to a specific address.
        /// @param addr The address to bind the socket to.
        void bind(const raw_address& addr);

        /// @brief Listen on the socket for incoming connections.
        /// @param backlog The number of incoming connections that can be enqueued.
        void listen(int backlog);

        /// @brief Inner type that represents a connection status.
        enum class connect_status
        {
            /// @brief The socket immediately connected successfully.
            connected,

            /// @brief The socket's connection is in progress.
            pending
        };

        /// @brief Connect a socket to an address.
        /// @param addr The address to connect the socket to.
        /// @returns connect_status::connected if the connection succeeded, otherwise connect_status::pending if it is
        /// still in progress.
        connect_status connect(const raw_address& addr);

        /// @brief Accept a new incoming connection.
        /// @param sock A socket object to update with the new incoming connection.
        /// @param addr An address object to update with the new address for the connection.
        /// @returns True if a successful incoming connection was processed, otherwise false if there was no incoming
        /// connection to process.
        bool accept(socket& sock, raw_address& addr);

        /// @brief Inner type that represents the status of a transfer (read or write) operation.
        enum class transfer_status
        {
            /// @brief The transfer was successful, although potentially not all bytes were transferred.
            success,

            /// @brief The transfer could not be completed; no bytes were transferred.
            blocked,

            /// @brief The socket was disconnected; no bytes were transferred.
            disconnected
        };

        /// @brief Attempt to send a chunk of data on the socket.
        /// @param data Sequence of bytes to send.
        /// @returns The transfer status along with the number of bytes (from the front of the data sequence) that were
        /// successfully sent.
        std::pair<transfer_status, size_t> send(std::span<const char> data);

        /// @brief Attempt to receive a chunk of data from the socket.
        /// @param data Buffer for a sequence of bytes to be received.
        /// @returns The transfer status along with the number of bytes (into the front of the buffer) that were
        /// successfully received.
        std::pair<transfer_status, size_t> recv(std::span<char> data);

    private:
        socket_id sock_ = k_invalid_socket;

        explicit socket(const socket_id sock) : sock_(sock)
        {
        }
    };

} // namespace jhoyt::asl