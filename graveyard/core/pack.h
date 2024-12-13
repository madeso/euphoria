#pragma once


#include <optional>

#include "base/rect.h"
#include "base/size2.h"


namespace eu::core
{
    // place from top-left to down-right
    std::vector<std::optional<Recti>>
    pack
    (
        const size2i& container,
        const std::vector<size2i>& to_pack
    );
}
