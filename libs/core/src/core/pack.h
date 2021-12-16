#pragma once

#include <vector>
#include <optional>

#include "core/rect.h"
#include "core/size2.h"


namespace euphoria::core
{
    // place from top-left to down-right
    std::vector<std::optional<Recti>>
    pack
    (
        const Size2i& container,
        const std::vector<Size2i>& to_pack
    );
}
