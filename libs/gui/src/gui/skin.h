#pragma once

#include "core/rgb.h"
#include "core/interpolate.h"
#include "core/vfs_path.h"
#include "core/noncopyable.h"

#include <string>
#include <memory>
#include <optional>


namespace euphoria::render
{
    struct drawable_font;
}

namespace euphoria::gui
{
    struct interpolation
    {
        core::InterpolationType type = core::InterpolationType::Linear;
        float time = 0.0f;
    };

    struct button_state
    {
        // std::string image;
        float scale = 0;
        core::Rgb image_color {0.0f};
        core::Rgb text_color {1.0f};
        float dx = 0;
        float dy = 0;

        interpolation interpolation_position;
        interpolation interpolation_color;
        interpolation interpolation_size;
    };


    struct skin
    {
        skin() = default;
        ~skin();

        NONCOPYABLE(skin);

        std::string name;

        std::shared_ptr<render::drawable_font> font;

        float text_size = 30.0f;

        std::optional<core::vfs::FilePath> button_image;

        button_state button_idle;
        button_state button_hot;
        button_state button_active_hot;
    };
}
