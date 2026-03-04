#pragma once

namespace eu::render
{


/// A simple integer sRGBA color.
/// unline \see Rgb or \see Color that are floats, this is "integer" based, with values from 0 to 255.
struct PixelColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

/// A simple sRGB(A) image
struct Image
{
    int width = 0;
    int height = 0;
    std::vector<unsigned char> data; ///< The data is stored in row-major order, with 4 bytes per pixel RGB(A)
    
    /// Default constructor initializes an empty (invalid) image.
    Image();

    /// Returns true if the image has valid dimensions and data.
    bool is_valid() const;

    /// Initializes the image with the specified dimensions and alpha support.
    /// @param w The width of the image.
    /// @param h The height of the image.
    void setup_with_alpha_support(int w, int h);

    /// @brief Marks the image as invalid by resetting its dimensions and clearing its data.
    void make_invalid();

    /// Sets the color of a pixel at the specified coordinates in an image buffer.
    /// @param x The x-coordinate of the pixel.
    /// @param y The y-coordinate of the pixel.
    /// @param r The red color component (0-255).
    /// @param g The green color component (0-255).
    /// @param b The blue color component (0-255).
    /// @param a The alpha (transparency) component (0-255).
    void set_pixel(int x, int y, int r, unsigned char g, unsigned char b, unsigned char a);

    /// Overload: Sets the color of a pixel using a PixelColor struct.
    void set_pixel(int x, int y, const PixelColor& color);

    /// Gets the color of a pixel at the specified coordinates.
    /// @param x The x-coordinate of the pixel.
    /// @param y The y-coordinate of the pixel.
    /// @return The color at the pixel as a PixelColor struct. If out of bounds, returns {0,0,0,0}.
    PixelColor get_pixel(int x, int y) const;
};

/// Copies pixel data from a source image to a destination image at specified coordinates.
/// Sometimes called blit.
/// @param dst The destination image.
/// @param dst_x The x-coordinate in the destination image where the source image will be pasted.
/// @param dst_y The y-coordinate in the destination image where the source image will be pasted.
/// @param src The source image.
void paste_image(Image* dst, int dst_x, int dst_y, const Image& src);

}
