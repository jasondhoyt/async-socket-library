// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

namespace jhoyt::asl
{

#if !defined(_WIN32)
    using socket_id = int;
    constexpr auto k_invalid_socket = socket_id{-1};
#endif

} // namespace jhoyt::asl
