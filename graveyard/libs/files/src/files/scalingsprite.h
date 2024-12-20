#pragma once

#include "io/json.h"


namespace eu::files::scalingsprite
{
    struct Rect {
        int left = 10;
        int right = 10;
        int top = 10;
        int bottom = 10;
    };

    struct ScalingSprite {
        /* positive = absolute size, negative=abs percent size  */
        std::vector<int> rows;
        std::vector<int> cols;
        std::vector<Rect> rects;
    };

    JSON_PARSE_FUNC(ScalingSprite);
}

