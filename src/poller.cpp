// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <cassert>

#if !defined(_WIN32)
#include <poll.h>
#endif

#include "jhoyt/asl/poller.hpp"

#include "detail/error.hpp"

namespace
{
    using namespace jhoyt::asl;

#if !defined(_WIN32)
    using poll_entry_type = pollfd;
#endif

#if !defined(_WIN32)
    short map_poll_type(poller::poll_type type)
    {
        switch (type)
        {
        case poller::poll_type::connect:
            return POLLOUT;

        case poller::poll_type::read:
            return POLLIN;

        case poller::poll_type::read_write:
            return POLLIN | POLLOUT;

        default:
            assert(false);
        }

        return 0;
    }
#endif

} // namespace

namespace jhoyt::asl
{

    struct poller::impl
    {
        std::vector<poll_result> results;
        std::vector<poll_entry_type> entries;
        std::vector<poll_type> entry_types;
    };

    poller::poller() : pimpl_(std::make_unique<impl>())
    {
    }

    poller::~poller() = default;

    void poller::add_socket(socket_id id, poll_type type)
    {
        if (!pimpl_)
        {
            return;
        }

#if !defined(_WIN32)
        pimpl_->entries.emplace_back(id, map_poll_type(type), 0);
#else
        assert(false);
#endif

        pimpl_->entry_types.emplace_back(type);
    }

    void poller::update_socket(socket_id id, poll_type type)
    {
        if (!pimpl_)
        {
            return;
        }

        auto ix = size_t{0};
        for (auto& entry : pimpl_->entries)
        {
#if !defined(_WIN32)
            if (entry.fd == id)
            {
                entry.events = map_poll_type(type);
                break;
            }
#else
            assert(false);
#endif
            ++ix;
        }

        if (ix < pimpl_->entry_types.size())
        {
            pimpl_->entry_types[ix] = type;
        }
    }

    void poller::remove_socket(socket_id id)
    {
        if (!pimpl_)
        {
            return;
        }

        for (auto ix = size_t{0}; ix < pimpl_->entries.size(); ++ix)
        {
            auto& entry = pimpl_->entries[ix];
#if !defined(_WIN32)
            if (entry.fd == id)
#else
            assert(false);
#endif
            {
                if (ix < pimpl_->entries.size() - 1)
                {
                    // Swap with the last element for faster erase since order is not important for this structure.
                    std::swap(entry, pimpl_->entries.back());
                    pimpl_->entry_types[ix] = pimpl_->entry_types.back();
                }

                pimpl_->entries.pop_back();
                pimpl_->entry_types.pop_back();

                break;
            }
        }
    }

    std::span<const poller::poll_result> poller::poll(const std::chrono::nanoseconds& timeout)
    {
        if (!pimpl_)
        {
            return {};
        }

        pimpl_->results.clear();

#if !defined(WIN32)
        const auto timeout_ms =
            static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
        if (::poll(pimpl_->entries.data(), pimpl_->entries.size(), timeout_ms) == -1)
        {
            throw std::runtime_error{detail::make_socket_error_string("failed to poll sockets")};
        }

        auto ix = size_t{0};
        for (const auto& entry : pimpl_->entries)
        {
            switch (pimpl_->entry_types[ix])
            {
            case poll_type::connect:
                // TODO : what about failed connection?
                if ((entry.revents & POLLOUT) != 0)
                {
                    pimpl_->results.emplace_back(entry.fd, poll_status::connection_succeeded);
                }
                break;

            case poll_type::read_write:
                if ((entry.revents & POLLOUT) != 0)
                {
                    pimpl_->results.emplace_back(entry.fd, poll_status::ready_to_write);
                }
                [[fallthrough]];

            case poll_type::read:
                if ((entry.revents & POLLIN) != 0)
                {
                    pimpl_->results.emplace_back(entry.fd, poll_status::ready_to_read);
                }
                break;

            default:
                assert(false);
            }
            ++ix;
        }

#else
        assert(false);
#endif

        return pimpl_->results;
    }

} // namespace jhoyt::asl