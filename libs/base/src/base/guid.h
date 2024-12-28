#pragma once

#include <string>
#include <optional>
#include <array>

#include "base/ints.h"

namespace eu
{
    struct Guid
    {
        std::array<u8, 16> data;

        Guid() = default;
        constexpr explicit Guid(std::array<u8, 16> d)
            : data(std::move(d))
        {
        }

        [[nodiscard]] static std::optional<Guid> generate();
    };

    [[nodiscard]] std::optional<Guid> guid_from(const std::string& str);
    [[nodiscard]] std::string string_from(const Guid& g);

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
}
