// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "jhoyt/asl/mocks/poller_mock.hpp"

namespace
{
    using namespace jhoyt::asl;

    auto g_add_socket_calls = std::vector<mock::poller_modify_socket_call>{};
    auto g_update_socket_calls = std::vector<mock::poller_modify_socket_call>{};
    auto g_remove_socket_calls = std::vector<mock::poller_remove_socket_call>{};
    auto g_poll_calls = std::vector<mock::poller_poll_call>{};
    auto g_poll_throws = false;
    auto g_poll_results = std::vector<poller::poll_result>{};
} // namespace

namespace jhoyt::asl
{

    struct poller::impl
    {
    };

    poller::poller() = default;
    poller::~poller() = default;

    void poller::add_socket(socket_id id, poll_type type)
    {
        g_add_socket_calls.emplace_back(id, type);
    }

    void poller::update_socket(socket_id id, poll_type type)
    {
        g_update_socket_calls.emplace_back(id, type);
    }

    void poller::remove_socket(socket_id id)
    {
        g_remove_socket_calls.emplace_back(id);
    }

    std::span<const poller::poll_result> poller::poll(const std::chrono::nanoseconds& timeout)
    {
        g_poll_calls.emplace_back(timeout);

        if (g_poll_throws)
        {
            throw std::runtime_error{"poller::poll error"};
        }

        return g_poll_results;
    }

    namespace mock
    {

        void poller_reset()
        {
            g_add_socket_calls.clear();
            g_update_socket_calls.clear();
            g_remove_socket_calls.clear();
            g_poll_calls.clear();
            g_poll_throws = false;
            g_poll_results.clear();
        }

        std::span<const poller_modify_socket_call> poller_get_add_socket_calls()
        {
            return g_add_socket_calls;
        }

        std::span<const poller_modify_socket_call> poller_get_update_socket_calls()
        {
            return g_update_socket_calls;
        }

        std::span<const poller_remove_socket_call> poller_get_remove_socket_calls()
        {
            return g_remove_socket_calls;
        }

        void poller_enqueue_poll_result(const poller::poll_result result)
        {
            g_poll_results.push_back(result);
        }

        void poller_set_poll_throws(const bool value)
        {
            g_poll_throws = value;
        }

        std::span<poller_poll_call> poller_get_poll_calls()
        {
            return g_poll_calls;
        }

    } // namespace mock

} // namespace jhoyt::asl