// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "common.hpp"

namespace jhoyt::asl
{

    /// @brief Class that performs any setup and teardown operations that OS-level socket operations may require.
    ///
    /// Some operating systems will not have any functions to be performed but this should be present regardless in
    /// order to provide cross-platform support. At this time only the Windows platform requires setup and teardown
    /// operations for its socket functions.
    ///
    /// @note This context is only needed for the socket and poller classes. The raw_address class does not need this
    /// context in order to function properly.
    ///
    /// @note While only one of this context object is required per process, multiple can be instantiated safely.
    class ASL_API context final
    {
    public:
        context();
        ~context();

        context(const context&) = delete;
        context& operator=(const context&) = delete;

        context(context&&) = delete;
        context& operator=(context&&) = delete;
    };

} // namespace jhoyt::asl