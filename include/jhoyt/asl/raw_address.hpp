// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <span>

#if !defined(_WIN32)
#include <sys/socket.h>
#endif

#include "address.hpp"
#include "common.hpp"

namespace jhoyt::asl
{

    /// @brief Type that represents a (raw) stored address used as inputs to various socket functions.
    ///
    /// This class serves as a bridge between cross-platform address representations (i.e. `jhoyt::asl::address`) and
    /// the platform-specific structures that are used as inputs to various OS-level socket functions.
    class ASL_API raw_address final
    {
    public:
        /// @brief Construct a new raw address from a provided generic address value.
        ///
        /// @param addr The generic address from which to construct the raw address.
        explicit raw_address(const address& addr);

        /// @brief Construct a new raw address from a provided chunk of raw address data.
        ///
        /// Note that this functionality should usually not be used directly. It is present for completeness and use by
        /// other lower-level library functions.
        ///
        /// @param data The raw bytes that represent the internal OS-level address type.
        explicit raw_address(std::span<const char> data);

        /// @brief Retrieve the raw address bytes stored in this value.
        ///
        /// @returns Span that represents the total bytes used by the corresponding and internal OS-level address type.
        [[nodiscard]] std::span<const char> get_data() const;

        /// @brief Retrieve the generic address representation of this value.
        ///
        /// @returns Generic address representation in a cross-platform form.
        [[nodiscard]] address get_address() const;

    private:
        sockaddr_storage data_;
    };

} // namespace jhoyt::asl
