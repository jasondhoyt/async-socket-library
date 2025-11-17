// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <chrono>
#include <thread>

#include <catch.hpp>

#include <jhoyt/asl/asl.hpp>

TEST_CASE("Connection Succeeds")
{
    auto ctx = jhoyt::asl::context{};

    const auto raw_address = jhoyt::asl::raw_address{jhoyt::asl::ipv4_address{.host = "127.0.0.1", .port = 5555}};
    auto server = jhoyt::asl::socket{};
    server.open(jhoyt::asl::socket_domain::ipv4, jhoyt::asl::socket_type::stream);
    server.set_reuse_address_option(true);
    server.bind(raw_address);
    server.listen(1);

    auto client = jhoyt::asl::socket{};
    client.open(jhoyt::asl::socket_domain::ipv4, jhoyt::asl::socket_type::stream);
    client.connect(raw_address);

    auto poller = jhoyt::asl::poller{};
    poller.add_socket(server.get_id(), jhoyt::asl::poller::poll_type::read);
    poller.add_socket(client.get_id(), jhoyt::asl::poller::poll_type::connect);

    const auto end_time = std::chrono::steady_clock::now() + std::chrono::seconds{1};
    auto connected = false;
    auto incoming_socket = jhoyt::asl::socket{};
    auto incoming_address = jhoyt::asl::raw_address{};
    while (!connected && std::chrono::steady_clock::now() < end_time)
    {
        for (auto events = poller.poll(std::chrono::milliseconds{150}); const auto& [id, status] : events)
        {
            if (id == server.get_id() && status == jhoyt::asl::poller::poll_status::ready_to_read)
            {
                server.accept(incoming_socket, incoming_address);
            }
            else if (id == client.get_id() && status == jhoyt::asl::poller::poll_status::connection_succeeded)
            {
                connected = true;
            }
        }
    }

    CHECK(connected);
}

TEST_CASE("Connection Timed Out")
{
    auto ctx = jhoyt::asl::context{};

    const auto raw_address = jhoyt::asl::raw_address{jhoyt::asl::ipv4_address{.host = "127.0.0.1", .port = 5555}};

    auto client = jhoyt::asl::socket{};
    client.open(jhoyt::asl::socket_domain::ipv4, jhoyt::asl::socket_type::stream);
    client.connect(raw_address);

    auto poller = jhoyt::asl::poller{};
    poller.add_socket(client.get_id(), jhoyt::asl::poller::poll_type::connect);

    const auto end_time = std::chrono::steady_clock::now() + std::chrono::seconds{1};
    auto connected = false;
    while (!connected && std::chrono::steady_clock::now() < end_time)
    {
        for (auto events = poller.poll(std::chrono::milliseconds{150}); const auto& [id, status] : events)
        {
            if (id == client.get_id() && status == jhoyt::asl::poller::poll_status::connection_succeeded)
            {
                connected = true;
            }
        }
    }

    CHECK(!connected);
}