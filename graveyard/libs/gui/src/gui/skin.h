#pragma once

#include <memory>
#include <optional>

#include "base/rgb.h"
#include "core/easing.h"
#include "io/vfs_path.h"


namespace eu::render
{
    struct DrawableFont;
}

namespace eu::gui
{
    struct Interpolation
    {
        core::easing::Function type = core::easing::Function::linear;
        float time = 0.0f;
    };

    struct ButtonState
    {
        // std::string image;
        float scale = 0;
        Rgb image_color {0.0f};
        Rgb text_color {1.0f};
        float dx = 0;
        float dy = 0;

        Interpolation interpolation_position;
        Interpolation interpolation_color;
        Interpolation interpolation_size;
    };


    struct Skin
    {
        Skin() = default;
        ~Skin();

        Skin(const Skin& other) = delete;
        void operator=(const Skin&) = delete;
        Skin(Skin&& other) = delete;
        void operator=(Skin&&) = delete;

        std::string name;

        std::shared_ptr<render::DrawableFont> font;

        float text_size = 30.0f;

        std::optional<io::FilePath> button_image;

        ButtonState button_idle;
        ButtonState button_hot;
        ButtonState button_active_hot;
    };
}
