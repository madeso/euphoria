#include "gui/skin.h"
#include "render/fonts.h"

namespace euphoria::gui
{
    ButtonState::ButtonState()
        : image_color(0)
        , text_color(0)
        , dx(0)
        , dy(0)
        , interpolationPosition(core::InterpolationType::Linear)
        , interpolationPositionTime(0)
        , interpolationColor(core::InterpolationType::Linear)
        , interpolationColorTime(0)
        , interpolationSize(core::InterpolationType::Linear)
        , interpolationSizeTime(0)
    {}

    ButtonState::~ButtonState() {}

    Skin::Skin() : text_size(30.0f) {}

    Skin::~Skin() {}
}  // namespace euphoria::gui
