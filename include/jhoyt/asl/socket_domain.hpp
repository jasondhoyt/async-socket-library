// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

namespace jhoyt::asl
{

    /// @brief Enumeration that represents the supported types of socket domains.
    enum class socket_domain
    {
        ipv4,
        ipv6,
        file
    };

} // namespace jhoyt::asl