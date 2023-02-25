#pragma once

namespace euphoria::core
{
    template<typename TNormalized, typename TLength>
    struct NormalizedAndLength
    {
        TNormalized normalized;
        TLength length;
    };
}
