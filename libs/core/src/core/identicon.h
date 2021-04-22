#ifndef EUPHORIA_CORE_IDENTICON_H
#define EUPHORIA_CORE_IDENTICON_H

// converted to c++ from https://github.com/donpark/identicon/blob/master/identicon-canvas/identicon_canvas.js
namespace euphoria::core
{
    struct image;

    void render_identicon(image* image, int code);
}

#endif // EUPHORIA_CORE_IDENTICON_H
