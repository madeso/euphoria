#include "base/hash.string.h"

namespace eu
{

    std::string string_from(const Hsh hash)
    {
        
#if USE_HASH_TEXT == 1
        return fmt::format("{0}", hash.text);
#else
        return fmt::format("0x{0:x}", hash.hash);
#endif
    }

    std::string string_from(const HshO& hash)
    {
        
#if USE_HASH_TEXT == 1
        return fmt::format("{0}", hash.text);
#else
        return fmt::format("0x{0:x}", hash.hash);
#endif
    }

    ADD_CATCH_FORMATTER_IMPL(Hsh)
    ADD_CATCH_FORMATTER_IMPL(HshO)
}
