// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <cassert>

#include "jhoyt/asl/context.hpp"

namespace jhoyt::asl
{

    context::context()
    {
#if defined(_WIN32)
        assert(false);
#endif
    }

    context::~context()
    {
#if defined(_WIN32)
        assert(false);
#endif
    }

} // namespace jhoyt::asl