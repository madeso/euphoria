#ifndef EUPHORIA_CORE_COLLAGE_H
#define EUPHORIA_CORE_COLLAGE_H

#include <vector>
#include <optional>

#include "core/vec2.h"
#include "core/size.h"


namespace euphoria::core
{
    struct Image;
    struct Rgbi;

    std::vector<Sizei> CollectSizes
    (
        const std::vector<Image>& images,
        int padding
    );

    Image
    DrawImage
    (
        const std::vector<vec2i>& positions,
        const std::vector<Image>& images,
        const Sizei& size,
        const Rgbi& background_color
    );

    Sizei
    PackTight
    (
        const Sizei& default_size,
        std::vector<vec2i>* positions,
        const std::vector<Image>& images,
        int padding
    );


    ////////////////////////////////////////////////

    std::vector<vec2i>
    PackImage
    (
        const Sizei& image_size,
        const std::vector<Image>& images,
        const std::vector<std::string>& files,
        int padding
    );


    std::optional<Image>
    PackImage
    (
        const std::vector<Image>& images,
        const std::vector<std::string>& files,
        const Sizei& requested_size,
        int padding,
        Rgbi background_color,
        bool pack_image
    );


    ////////////////////////////////////////////////


    std::pair<std::vector<vec2i>, Sizei>
    GridLayout
    (
        const std::vector<Image>& images,
        int padding,
        bool top_to_bottom
    );

    Image
    GridLayout
    (
        const std::vector<Image>& images,
        int padding,
        Rgbi background_color,
        bool top_to_bottom
    );

}


#endif  // EUPHORIA_CORE_COLLAGE_H

