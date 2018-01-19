#include "gui/skin.h"
#include "render/fonts.h"

ButtonState::ButtonState()
    : image_color(0)
    , text_color(0)
    , dx(0)
    , dy(0)
    , interpolationPosition(InterpolationType::Linear)
    , interpolationPositionTime(0)
    , interpolationColor(InterpolationType::Linear)
    , interpolationColorTime(0)
    , interpolationSize(InterpolationType::Linear)
    , interpolationSizeTime(0)
{
}

ButtonState::~ButtonState()
{
}

Skin::Skin()
    : text_size(30.0f)
{
}

Skin::~Skin()
{
}
