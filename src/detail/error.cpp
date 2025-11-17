// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <format>

#include "error.hpp"

namespace
{
    constexpr auto k_strerror_cap = 256;
}

namespace jhoyt::asl::detail
{

    std::string make_socket_error_string(std::string_view msg)
    {
#if !defined(_WIN32)
        auto buf = std::array<char, k_strerror_cap>{};
        strerror_r(errno, buf.data(), buf.size());
        return std::format("{}: {}", msg, buf.data());
#endif
    }

} // namespace jhoyt::asl::detail