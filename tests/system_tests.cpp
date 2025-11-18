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

TEST_CASE("Read Ready")
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

    const auto end_time = std::chrono::steady_clock::now() + std::chrono::seconds{5};
    auto connected = false;
    auto read_succeeded = false;
    auto incoming_socket = jhoyt::asl::socket{};
    auto incoming_address = jhoyt::asl::raw_address{};
    auto msg = std::string_view{"Hello, world"};
    while (!read_succeeded && std::chrono::steady_clock::now() < end_time)
    {
        for (auto events = poller.poll(std::chrono::milliseconds{150}); const auto& [id, status] : events)
        {
            if (id == server.get_id() && status == jhoyt::asl::poller::poll_status::ready_to_read)
            {
                server.accept(incoming_socket, incoming_address);

                auto [send_status, count] = incoming_socket.send({msg.data(), msg.size()});
                CHECK(send_status == jhoyt::asl::socket::transfer_status::success);
                CHECK(count == msg.size());
            }
            else if (id == client.get_id() && status == jhoyt::asl::poller::poll_status::connection_succeeded)
            {
                connected = true;
                poller.update_socket(client.get_id(), jhoyt::asl::poller::poll_type::read);
            }
            else if (id == client.get_id() && status == jhoyt::asl::poller::poll_status::ready_to_read)
            {
                auto buf = std::string{};
                buf.resize(64);
                auto [recv_status, count] = client.recv({buf.data(), buf.size()});
                CHECK(recv_status == jhoyt::asl::socket::transfer_status::success);
                CHECK(count == msg.size());

                buf.resize(count);
                CHECK(buf == msg);
                read_succeeded = true;
            }
        }
    }

    CHECK(connected);
    CHECK(read_succeeded);
}

TEST_CASE("Write Ready")
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

    const auto end_time = std::chrono::steady_clock::now() + std::chrono::seconds{5};
    auto connected = false;
    auto write_succeeded = false;
    auto read_succeeded = false;
    auto incoming_socket = jhoyt::asl::socket{};
    auto incoming_address = jhoyt::asl::raw_address{};
    auto msg = std::string_view{"Hello, world"};
    while (!read_succeeded && std::chrono::steady_clock::now() < end_time)
    {
        for (auto events = poller.poll(std::chrono::milliseconds{150}); const auto& [id, status] : events)
        {
            if (id == server.get_id() && status == jhoyt::asl::poller::poll_status::ready_to_read)
            {
                server.accept(incoming_socket, incoming_address);
                poller.add_socket(incoming_socket.get_id(), jhoyt::asl::poller::poll_type::read_write);
            }
            else if (id == incoming_socket.get_id() && status == jhoyt::asl::poller::poll_status::ready_to_write)
            {
                auto [send_status, count] = incoming_socket.send({msg.data(), msg.size()});
                CHECK(send_status == jhoyt::asl::socket::transfer_status::success);
                CHECK(count == msg.size());
                write_succeeded = true;
            }
            else if (id == client.get_id() && status == jhoyt::asl::poller::poll_status::connection_succeeded)
            {
                connected = true;
                poller.update_socket(client.get_id(), jhoyt::asl::poller::poll_type::read);
            }
            else if (id == client.get_id() && status == jhoyt::asl::poller::poll_status::ready_to_read)
            {
                auto buf = std::string{};
                buf.resize(64);
                auto [recv_status, count] = client.recv({buf.data(), buf.size()});
                CHECK(recv_status == jhoyt::asl::socket::transfer_status::success);
                CHECK(count == msg.size());

                buf.resize(count);
                CHECK(buf == msg);
                read_succeeded = true;
            }
        }
    }

    CHECK(connected);
    CHECK(write_succeeded);
    CHECK(read_succeeded);
}