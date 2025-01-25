#pragma once

#include <string>
#include <optional>
#include <array>

#include "base/ints.h"
#include "assert/assert.h"

namespace eu
{
    /// Represents a universally unique identifier.
    struct Guid
    {
        std::array<u8, 16> data;


        /// Does nothing.
        /// @see \ref eu::nil_guid
        Guid() = default;

        /// Set the guid data.
        constexpr explicit Guid(std::array<u8, 16> d)
            : data(std::move(d))
        {
        }

        constexpr explicit Guid(u32 a, u16 b, u16 c, u16 d, u64 e)
            : data{{
                // a
                static_cast<u8>((a >> 24) & 0xFF),
                static_cast<u8>((a >> 16) & 0xFF),
                static_cast<u8>((a >> 8) & 0xFF),
                static_cast<u8>(a & 0xFF),
                // b
                static_cast<u8>((b >> 8) & 0xFF),
                static_cast<u8>(b & 0xFF),
                // c
                static_cast<u8>((c >> 8) & 0xFF),
                static_cast<u8>(c & 0xFF),
                // d
                static_cast<u8>((d >> 8) & 0xFF),
                static_cast<u8>(d & 0xFF),
                // e
                static_cast<u8>((e >> 40) & 0xFF),
                static_cast<u8>((e >> 32) & 0xFF),
                static_cast<u8>((e >> 24) & 0xFF),
                static_cast<u8>((e >> 16) & 0xFF),
                static_cast<u8>((e >> 8) & 0xFF),
                static_cast<u8>(e & 0xFF),
            }}
        {
            ASSERT((e >> 48) == 0);
        }

        [[nodiscard]] static std::optional<Guid> generate();
    };

    bool operator==(const Guid& lhs, const Guid& rhs);
    bool operator!=(const Guid& lhs, const Guid& rhs);

    /// Parse a guid from a string
    [[nodiscard]] std::optional<Guid> guid_from(const std::string& str);

    /// Convert a guid to a string representation, prefer fmt.
    [[nodiscard]] std::string string_from(const Guid& g);

    /// A nil guid, all bits are set to 0.
    constexpr Guid nil_guid = Guid
    {
        std::array<u8, 16>
        {
            0, 0, 0, 0,
            0, 0,
            0, 0,
            0, 0,
            0, 0, 0, 0, 0, 0
        }
    };

    // todo(Gustav): add std::map and std::unoredered map support

    ADD_CATCH_FORMATTER(Guid)
}

ADD_DEFAULT_FORMATTER(eu::Guid, std::string, eu::string_from);
