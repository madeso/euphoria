#ifndef EUPHORIA_CORE_PACK_H
#define EUPHORIA_CORE_PACK_H

#include <vector>
#include <optional>

#include "core/rect.h"
#include "core/size.h"


namespace euphoria::core
{
    std::vector<std::optional<Recti>>
    Pack
    (
        const Sizei& container,
        const std::vector<Sizei>& to_pack
    );
}

#endif // EUPHORIA_CORE_PACK_H


