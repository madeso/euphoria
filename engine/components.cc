#include "engine/components.h"

#include <memory>
#include "dukglue/dukglue.h"

#include "core/componentsystem.h"
#include "core/vec2.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

CPosition2::CPosition2()
    : pos(0, 0)
{
}

vec2f*
CPosition2::GetPositionPtr()
{
  return &pos;
}
