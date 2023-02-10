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
        explicit Guid(std::array<uint8_t, 16> d);

        [[nodiscard]] static std::optional<Guid> create();
        [[nodiscard]] static Guid nil();
        [[nodiscard]] static std::optional<Guid> parse(const std::string& str);

        [[nodiscard]] std::string to_string() const;
    };
}

