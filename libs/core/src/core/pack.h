#ifndef EUPHORIA_CORE_PACK_H
#define EUPHORIA_CORE_PACK_H

#include <vector>
#include <optional>

#include "core/rect.h"
#include "core/size.h"


namespace euphoria::core
{
    // place from top-left to down-right
    std::vector<std::optional<recti>>
    pack
    (
        const Sizei& container,
        const std::vector<Sizei>& to_pack
    );
}

#endif // EUPHORIA_CORE_PACK_H


