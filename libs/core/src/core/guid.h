#pragma once

#include <string>
#include <optional>
#include <array>

namespace euphoria::core
{
    struct Guid
    {
        std::array<uint8_t, 16> data;

        Guid() = default;
        constexpr explicit Guid(std::array<uint8_t, 16> d)
            : data(std::move(d))
        {
        }

        [[nodiscard]] static std::optional<Guid> create();
        [[nodiscard]] static std::optional<Guid> parse(const std::string& str);

        [[nodiscard]] std::string to_string() const;
    };

    constexpr Guid nil_guid = Guid
    {
        std::array<uint8_t, 16>
        {
            0, 0, 0, 0,
            0, 0,
            0, 0,
            0, 0,
            0, 0, 0, 0, 0, 0
        }
    };
}

