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

    class ASL_API socket final
    {
    public:
        socket() = default;
        ~socket();

        socket(const socket&) = delete;
        socket& operator=(const socket&) = delete;

        socket(socket& other) noexcept;
        socket& operator=(socket&& other) noexcept;

        [[nodiscard]] auto get_id() const
        {
            return sock_;
        }

        explicit operator bool() const
        {
            return sock_ != k_invalid_socket;
        }

        void open(socket_domain domain, socket_type type);

        void close();

        void set_reuse_address_option(bool value);

        enum class shutdown_type
        {
            read,
            write,
            read_write
        };

        void shutdown(shutdown_type type);

        void bind(const raw_address& addr);

        void listen(int backlog);

        enum class connect_status
        {
            connected,
            pending
        };

        connect_status connect(const raw_address& addr);

        enum class result
        {
            ok,
            blocked
        };

        bool accept(socket& sock, raw_address& addr);

        enum class transfer_status
        {
            success,
            blocked,
            disconnected
        };

        std::pair<transfer_status, size_t> send(std::span<const char> data);

        std::pair<transfer_status, size_t> recv(std::span<char> data);

    private:
        socket_id sock_ = k_invalid_socket;

        explicit socket(const socket_id sock) : sock_(sock)
        {
        }
    };

} // namespace jhoyt::asl