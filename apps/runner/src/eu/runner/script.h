#pragma once

#include "lax/lax.h"

namespace eu::runner
{

struct Script
{
    Script();

    bool run_file(const std::string& str);

    lax::Lax lax;
};

}
