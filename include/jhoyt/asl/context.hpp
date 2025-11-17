// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "common.hpp"

namespace jhoyt::asl
{

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