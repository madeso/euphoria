#pragma once

namespace eu::core
{
    template<typename TNormalized, typename TLength>
    struct NormalizedAndLength
    {
        TNormalized normalized;
        TLength length;
    };
}
