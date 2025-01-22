#pragma once

#include <string>
#include <optional>
#include <array>

#include "base/ints.h"

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

        [[nodiscard]] static std::optional<Guid> generate();
    };

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

    ADD_CATCH_FORMATTER(Guid)
}

ADD_DEFAULT_FORMATTER(eu::Guid, std::string, eu::string_from);
