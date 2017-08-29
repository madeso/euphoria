#include "render/fonts.h"

#include "render/texture.h"
#include "render/gl.h"
#include "render/shader.h"

#include <vector>
#include <memory>
#include <map>
#include "core/assert.h"
#include "core/noncopyable.h"
#include <iostream>

#include "render/bufferbuilder2d.h"
#include "render/shaderattribute2d.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace
{
  void
  Error(FT_Error err)
  {
    if(err == 0)
    {
      return;
    }
    std::cerr << "FONT Error: " << err << "\n";
  }
  void
  ErrorNoThrow(FT_Error err)
  {
    if(err == 0)
    {
      return;
    }
    std::cerr << "FONT Error: " << err << "\n";
  }

  unsigned int
  ConvertCharToIndex(char c)
  {
    // not entirely sure this is correct
    return static_cast<unsigned int>(c);
  }
}  // namespace

struct Library
{
  NONCOPYABLE_CONSTRUCTOR(Library);
  NONCOPYABLE_ASSIGNMENT(Library);
  NONCOPYABLE_MOVE_CONSTRUCTOR(Library);
  NONCOPYABLE_MOVE_ASSIGNMENT(Library);

  FT_Library library{nullptr};

  Library()
  {
    Error(FT_Init_FreeType(&library));
  }

  ~Library()
  {
    ErrorNoThrow(FT_Done_FreeType(library));
  }
};

struct FontChar
{
  unsigned int               c{0};
  bool                       valid{false};
  int                        glyph_width{0};
  int                        glyph_height{0};
  int                        bearing_x{0};
  int                        bearing_y{0};
  int                        advance{0};
  std::vector<unsigned char> pixels;
};

struct Face
{
  FT_Face face;

  NONCOPYABLE_CONSTRUCTOR(Face);
  NONCOPYABLE_ASSIGNMENT(Face);
  NONCOPYABLE_MOVE_CONSTRUCTOR(Face);
  NONCOPYABLE_MOVE_ASSIGNMENT(Face);

  Face(Library* lib, const std::string& path, unsigned int size)
      : face(nullptr)
  {
    int face_index = 0;
    Error(FT_New_Face(lib->library, path.c_str(), face_index, &face));
    Error(FT_Set_Pixel_Sizes(face, 0, size));
  }

  FontChar
  GetChar(unsigned int c)
  {
    const FT_Error error = FT_Load_Char(face, c, FT_LOAD_RENDER);
    if(error != 0)
    {
      std::cerr << "Failed to get char\n";
      return FontChar();
    }

    FT_GlyphSlot slot = face->glyph;


    FontChar ch;
    ch.c            = c;
    ch.glyph_height = slot->bitmap.rows;
    ch.glyph_width  = slot->bitmap.width;
    ch.bearing_x    = slot->bitmap_left;
    ch.bearing_y    = slot->bitmap_top;
    ch.valid        = true;
    ch.advance      = slot->advance.x >> 6;
    // pen_y += slot->advance.y >> 6;
    const int size = ch.glyph_width * ch.glyph_height;
    ch.pixels.resize(size, 0);
    memcpy(&ch.pixels[0], slot->bitmap.buffer, size);

    return ch;
  }

  ~Face()
  {
    FT_Done_Face(face);
  }
};

CharData::CharData(
    const BufferBuilder2d& data, const Rectf& ex, unsigned int ch, float ad)
    : buffer(data)
    , extent(ex)
    , c(ch)
    , advance(ad)
{
}

struct Pixels
{
  Pixels(int texture_width, int texture_height)
      : pixels(texture_width * texture_height * 4, 0)
      , texture_width_(texture_width)
      , texture_height_(texture_height)
  {
  }

  void
  Set(int x, int y, unsigned char v)
  {
    const size_t id = (x + static_cast<size_t>(y) * texture_width_) * 4;
    pixels[id + 0]  = 255;
    pixels[id + 1]  = 255;
    pixels[id + 2]  = 255;
    pixels[id + 3]  = v;
  }

  std::vector<unsigned char> pixels;
  int                        texture_width_;
  int                        texture_height_;
};

void
PasteCharacterToImage(Pixels* pixels, const stbrp_rect& r, const FontChar& ch)
{
  ASSERT(pixels);
  for(int y = 0; y < ch.glyph_height; ++y)
  {
    for(int x = 0; x < ch.glyph_width; ++x)
    {
      const int           id  = x + y * ch.glyph_width;
      const unsigned char val = ch.pixels[id];
      pixels->Set(r.x + x, r.y + y, val);
    }
  }
}

struct FontChars
{
  std::vector<FontChar> chars;
  KerningMap            kerning;
};

FontChars
GetCharactersFromFont(
    const std::string& font_file,
    unsigned int       font_size,
    const std::string& chars)
{
  Library lib;
  Face    f(&lib, font_file, font_size);

  FontChars fontchars{};
  fontchars.chars.reserve(chars.length());
  for(char c : chars)
  {
    FontChar cc = f.GetChar(ConvertCharToIndex(c));
    if(!cc.valid)
    {
      continue;
    }
    fontchars.chars.push_back(cc);
  }

  std::cout << "Loaded " << fontchars.chars.size() << " characters from "
            << font_file << "\n";

  FT_Bool use_kerning = FT_HAS_KERNING(f.face);
  std::cout << "kerning..." << static_cast<int>(use_kerning) << "\n";
  if(use_kerning == 1)
  {
    std::cout << "kerning...\n";
    for(const FontChar& previous : fontchars.chars)
    {
      for(const FontChar& current : fontchars.chars)
      {
        if(previous.c == current.c)
        {
          continue;
        }
        FT_Vector delta{};
        FT_Get_Kerning(
            f.face, previous.c, current.c, FT_KERNING_DEFAULT, &delta);
        int dx = delta.x >> 6;
        if(dx != 0)
        {
          fontchars.kerning.insert(KerningMap::value_type(
              KerningMap::key_type(previous.c, current.c), dx));
        }
      }
    }
  }

  return fontchars;
}

std::pair<BufferBuilder2d, Rectf>
BuildCharVao(
    const stbrp_rect& src_rect,
    const FontChar&   src_char,
    int               image_width,
    int               image_height)
{
  //
  //             width_
  //          <--------->
  //          A---------B
  // bearingX |         |   |              |
  // -------> |         |   |  bearingY    |
  // origin   |         |   |              | height
  // X........|.........|....... baseline  |
  //          |         |                  |
  //          |         |                  |
  //          C---------D
  //

  const int vert_left   = src_char.bearing_x;
  const int vert_right  = vert_left + src_char.glyph_width;
  const int vert_top    = -src_char.bearing_y;
  const int vert_bottom = vert_top + src_char.glyph_height;

  const float iw = image_width;
  const float ih = image_height;

  const stbrp_coord uv_left   = src_rect.x;
  const stbrp_coord uv_right  = uv_left + src_rect.w;
  const stbrp_coord uv_top    = src_rect.y;
  const stbrp_coord uv_bottom = uv_top + src_rect.h;

  BufferBuilder2d builder;
  const auto      a = Point(vert_left, vert_top, uv_left / iw, uv_top / ih);
  const auto      b = Point(vert_right, vert_top, uv_right / iw, uv_top / ih);
  const auto c = Point(vert_left, vert_bottom, uv_left / iw, uv_bottom / ih);
  const auto d = Point(vert_right, vert_bottom, uv_right / iw, uv_bottom / ih);
  builder.AddQuad(a, b, c, d);
  return std::make_pair(
      builder,
      Rectf::FromLeftRightTopBottom(
          vert_left, vert_right, vert_top, vert_bottom));
}

////////////////////////////////////////////////////////////////////////////////

BufferBuilder2d
SimpleQuad()
{
  BufferBuilder2d data;

  Point a(0.0f, 1.0f, 0.0f, 1.0f);
  Point b(1.0f, 0.0f, 1.0f, 0.0f);
  Point c(0.0f, 0.0f, 0.0f, 0.0f);
  Point d(1.0f, 1.0f, 1.0f, 1.0f);

  data.AddQuad(c, b, a, d);

  return data;
}

TextBackgroundRenderer::TextBackgroundRenderer(Shader* shader)
    : buffer_(SimpleQuad())
    , shader_(shader)
    , model_(shader->GetUniform("model"))
    , color_(shader->GetUniform("color"))
{
  ASSERT(shader);
}

void
TextBackgroundRenderer::Draw(float alpha, const Rectf& area)
{
  Use(shader_);
  const mat4f model =
      mat4f::Identity()
          .Translate(vec3f(area.left, area.top, 0.0f))
          .Scale(vec3f(area.GetWidth(), area.GetHeight(), 1.0f));

  shader_->SetUniform(model_, model);
  shader_->SetUniform(color_, Rgba(0.0f, 0.0f, 0.0f, alpha));
  buffer_.Draw();
}

////////////////////////////////////////////////////////////////////////////////

Font::Font(
    Shader*            shader,
    const std::string& font_file,
    unsigned int       font_size,
    const std::string& possible_chars)
    : shader_(shader)
    , font_size_(font_size)
    , color_(shader->GetUniform("color"))
    , model_(shader->GetUniform("model"))
{
  const int texture_width  = 512;
  const int texture_height = 512;

  const FontChars fontchars =
      GetCharactersFromFont(font_file, font_size, possible_chars);

  // pack char textures to a single texture
  const int               num_rects = fontchars.chars.size();
  std::vector<stbrp_rect> rects(num_rects);
  for(int i = 0; i < num_rects; ++i)
  {
    stbrp_rect& r = rects[i];
    r.id          = i;
    r.w           = fontchars.chars[i].glyph_width;
    r.h           = fontchars.chars[i].glyph_height;
  }
  stbrp_context           context{};
  const int               num_nodes = texture_width;
  std::vector<stbrp_node> nodes(num_nodes);
  stbrp_init_target(
      &context, texture_width, texture_height, &nodes[0], num_nodes);
  stbrp_pack_rects(&context, &rects[0], num_rects);

  CharDataMap map;
  Pixels      pixels(texture_width, texture_height);
  for(int i = 0; i < num_rects; ++i)
  {
    const stbrp_rect& src_rect = rects[i];
    if(src_rect.was_packed == 0)
    {
      std::cerr << "Failed to pack\n";
      continue;
    }
    const FontChar& src_char = fontchars.chars[src_rect.id];
    PasteCharacterToImage(&pixels, src_rect, src_char);
    const auto char_vao =
        BuildCharVao(src_rect, src_char, texture_width, texture_height);

    // store data in useful data
    std::shared_ptr<CharData> dest(new CharData(
        char_vao.first, char_vao.second, src_char.c, src_char.advance));
    map.insert(CharDataMap::value_type(dest->c, dest));
  }

  // load pixels into texture
  kerning_ = fontchars.kerning;
  chars_   = map;
  Texture2dLoadData load_data;
  texture_ = std::make_unique<Texture2d>();
  texture_->LoadFromPixels(
      texture_width,
      texture_height,
      &pixels.pixels[0],
      GL_RGBA,
      GL_RGBA,
      load_data);
}

void
Font::Draw(
    const vec2f&       start_position,
    const std::string& str,
    const Rgb&         base_color,
    const Rgb&         hi_color,
    int                hi_start,
    int                hi_end,
    float              scale) const
{
  Use(shader_);

  vec2f position = start_position;

  glActiveTexture(GL_TEXTURE0);
  Use(texture_.get());

  const bool apply_highlight = hi_end != -1 && hi_start != -1;
  if(!apply_highlight)
  {
    shader_->SetUniform(color_, base_color);
  }

  int          index           = 0;
  unsigned int last_char_index = 0;
  for(char c : str)
  {
    const int this_index = index;
    ++index;
    const unsigned int char_index = ConvertCharToIndex(c);
    auto               it         = chars_.find(char_index);
    if(it == chars_.end())
    {
      std::cerr << "Failed to print\n";
      continue;
    }
    std::shared_ptr<CharData> ch = it->second;

    const mat4f model = mat4f::Identity()
                            .Translate(vec3f(position, 0.0f))
                            .Scale(vec3f(scale, scale, 1.0f));
    shader_->SetUniform(model_, model);

    if(apply_highlight)
    {
      bool       use_hi_color = hi_start <= this_index && this_index < hi_end;
      const Rgb& color        = use_hi_color ? hi_color : base_color;
      shader_->SetUniform(color_, color);
    }
    ch->buffer.Draw();
    auto kerning = kerning_.find(std::make_pair(last_char_index, char_index));
    int  the_kerning = kerning == kerning_.end() ? 0 : kerning->second;
    position.x += (ch->advance + the_kerning) * scale;
  }
}

Rectf
Font::GetExtents(const std::string& str, float scale) const
{
  unsigned int last_char_index = 0;
  vec2f        position(0.0f);
  Rectf        ret;

  for(char c : str)
  {
    const unsigned int char_index = ConvertCharToIndex(c);
    auto               it         = chars_.find(char_index);
    if(it == chars_.end())
    {
      continue;
    }
    std::shared_ptr<CharData> ch = it->second;

    ret.Include(ch->extent.OffsetCopy(position));

    auto kerning = kerning_.find(std::make_pair(last_char_index, char_index));
    int  the_kerning = kerning == kerning_.end() ? 0 : kerning->second;
    position.x += (ch->advance + the_kerning) * scale;
  }

  return ret;
}

unsigned int
Font::GetFontSize() const
{
  return font_size_;
}

Text::Text(Font* font, TextBackgroundRenderer* back)
    : font_(font)
    , backgroundRenderer_(back)
    , scale_(1.0f)
    , text_("")
    , base_color_(0.0f)
    , hi_color_(1.0f)
    , hi_from_(-1)
    , hi_to_(-1)
    , alignment_(Align::LEFT)
    , use_background_(false)
    , background_alpha_(0.0f)

{
}

Text::~Text() = default;

void
Text::SetText(const std::string& str)
{
  text_ = str;
}

const std::string&
Text::GetText() const
{
  return text_;
}

void
Text::SetBaseColor(const Rgb& color)
{
  base_color_ = color;
}

void
Text::SetHighlightColor(const Rgb& color)
{
  hi_color_ = color;
}

void
Text::SetHighlightRange(int from, int to)
{
  hi_from_ = from;
  hi_to_   = to;
}

void
Text::SetBackground(bool use_background, float alpha)
{
  use_background_   = use_background;
  background_alpha_ = alpha;
}

void
Text::SetAlignment(Align alignment)
{
  alignment_ = alignment;
}

void
Text::SetSize(float new_size)
{
  ASSERT(font_);
  SetScale(new_size / font_->GetFontSize());
}

void
Text::SetScale(float scale)
{
  scale_ = scale;
  std::cout << "setting the scale to " << scale << "\n";
}

vec2f
GetOffset(Align alignment, const Rectf& extent)
{
  // todo: test this more
  const float middle = -(extent.left + extent.right) / 2;
  const float right  = -extent.right;
  const float top    = extent.top;
  const float bottom = -extent.bottom;

  switch(alignment)
  {
    case Align::TOP_LEFT:
      return vec2f(0.0f, top);
    case Align::TOP_CENTER:
      return vec2f(middle, top);
    case Align::TOP_RIGHT:
      return vec2f(right, top);
    case Align::MIDDLE_LEFT:
      return vec2f(0.0f, 0.0f);
    case Align::MIDDLE_CENTER:
      return vec2f(middle, 0.0f);
    case Align::MIDDLE_RIGHT:
      return vec2f(right, 0.0f);
    case Align::BOTTOM_LEFT:
      return vec2f(0.0f, bottom);
    case Align::BOTTOM_CENTER:
      return vec2f(middle, bottom);
    case Align::BOTTOM_RIGHT:
      return vec2f(right, bottom);
    default:
      DIE("Unhandled case");
      return vec2f(0.0f, 0.0f);
  }
}

void
Text::Draw(const vec2f& p) const
{
  Draw(p, base_color_);
}

void
Text::Draw(const vec2f& p, const Rgb& override_color) const
{
  if(font_ == nullptr)
  {
    return;
  }
  const Rectf& e   = GetExtents();
  const vec2f  off = GetOffset(alignment_, e);
  if(use_background_)
  {
    backgroundRenderer_->Draw(
        background_alpha_, e.ExtendCopy(5.0f).OffsetCopy(p + off));
  }
  font_->Draw(
      p + off, text_, override_color, hi_color_, hi_from_, hi_to_, scale_);
}

Rectf
Text::GetExtents() const
{
  return font_->GetExtents(text_, scale_);
}
