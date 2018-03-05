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

std::shared_ptr<vec2f>
CPosition2::GetPosition() const
{
  return std::make_shared<vec2f>(pos.x, pos.y);
}

void
CPosition2::SetPosition(vec2f* v)
{
  pos = *v;
}
