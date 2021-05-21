#ifndef EUPHORIA_CORE_PACK_H
#define EUPHORIA_CORE_PACK_H

#include <vector>
#include <optional>

#include "core/rect.h"
#include "core/size2.h"


namespace euphoria::core
{
    // place from top-left to down-right
    std::vector<std::optional<recti>>
    pack
    (
        const size2i& container,
        const std::vector<size2i>& to_pack
    );
}

#endif // EUPHORIA_CORE_PACK_H


