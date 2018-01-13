#include "render/fonts.h"

#include "render/texture.h"
#include "core/log.h"
#include "core/proto.h"

#include <vector>
#include <memory>
#include <map>
#include "core/assert.h"
#include "core/noncopyable.h"
#include "core/image.h"
#include <iostream>

#include "font.pb.h"

#include "render/spriterender.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

#include <ft2build.h>
#include FT_FREETYPE_H

LOG_SPECIFY_DEFAULT_LOGGER("core.font")

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

  std::string
  ConvertCharToIndex(char c)
  {
    return std::string{c};
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

// This represents a loaded glyph not yet placed on a texture image
// todo: rename to something with a glyph
struct FontChar
{
  //
  //              width_
  //           <--------->
  //           +---------+
  // bearingX  |         |   |              |
  // ------->  |         |   |  bearingY    |
  // origin    |         |   |              | height
  // X.........|.........|....... baseline  |
  //           |         |                  |
  //           |         |                  |
  //           +---------+
  //
  // ------------------------------> advance (x to next glyph)
  //

  bool        valid{false};
  std::string c;
  int         bearing_x{0};
  int         bearing_y{0};
  int         advance{0};
  Image       image;
};

namespace
{
  FT_UInt
  CharToFt(char c)
  {
    // todo: support unicode/utf-8 character encoding
    return static_cast<FT_UInt>(c);
  }
}

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
  GetChar(char c)
  {
    const FT_Error error = FT_Load_Char(face, CharToFt(c), FT_LOAD_RENDER);
    if(error != 0)
    {
      std::cerr << "Failed to get char\n";
      return FontChar();
    }

    FT_GlyphSlot slot = face->glyph;


    FontChar ch;
    ch.c         = c;
    ch.bearing_x = slot->bitmap_left;
    ch.bearing_y = slot->bitmap_top;
    ch.valid     = true;
    ch.advance   = slot->advance.x >> 6;
    // pen_y += slot->advance.y >> 6;
    if(slot->bitmap.width == 0)
    {
      ch.image.MakeInvalid();
    }
    else
    {
      ch.image.Setup(slot->bitmap.width, slot->bitmap.rows, true);
      auto* buffer = slot->bitmap.buffer;
      for(int y = 0; y < ch.image.GetHeight(); y += 1)
      {
        for(int x = 0; x < ch.image.GetWidth(); x += 1)
        {
          ch.image.SetPixel(
              x, y, 255, 255, 255, buffer[ch.image.GetWidth() * y + x]);
        }
      }
    }

    return ch;
  }

  ~Face()
  {
    FT_Done_Face(face);
  }
};

CharData::CharData(
    const Rectf& sprite, const Rectf& texture, const std::string& ch, float ad)
    : sprite_rect(sprite)
    , texture_rect(texture)
    , c(ch)
    , advance(ad)
{
}

// todo: move to core
void
PasteCharacterToImage(
    Image* target_image, const stbrp_rect& r, const Image& source_image)
{
  ASSERT(target_image);
  for(int y = 0; y < source_image.GetHeight(); ++y)
  {
    for(int x = 0; x < source_image.GetWidth(); ++x)
    {
      target_image->SetPixel(r.x + x, r.y + y, source_image.GetPixel(x, y));
    }
  }
}

// represent a loaded font, but not yet converted into a renderable texture
struct FontChars
{
  std::vector<FontChar> chars;
  KerningMap            kerning;

  void
  CombineWith(const FontChars& fc)
  {
    for(const auto& c : fc.chars)
    {
      chars.push_back(c);
    }

    for(const auto& e : fc.kerning)
    {
      const auto found = kerning.find(e.first);
      if(found == kerning.end())
      {
        kerning.insert(e);
      }
      else
      {
        LOG_ERROR("Multiple kernings found when trying to combine");
      }
    }
  }
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
    FontChar cc = f.GetChar(c);
    if(!cc.valid)
    {
      continue;
    }
    fontchars.chars.push_back(cc);
  }

  const FT_Bool use_kerning = FT_HAS_KERNING(f.face);

  LOG_INFO(
      "Loaded " << fontchars.chars.size() << " characters from " << font_file);
  LOG_INFO("kerning..." << static_cast<int>(use_kerning));

  if(use_kerning == 1)
  {
    std::cout << "kerning...\n";
    for(const char previous : chars)
    {
      for(const char current : chars)
      {
        if(previous == current)
        {
          continue;
        }
        const std::string previous_c = ConvertCharToIndex(previous);
        const std::string current_c  = ConvertCharToIndex(current);
        FT_Vector         delta{};
        FT_Get_Kerning(
            f.face,
            CharToFt(previous),
            CharToFt(current),
            FT_KERNING_DEFAULT,
            &delta);
        int dx = delta.x >> 6;
        if(dx != 0)
        {
          fontchars.kerning.insert(KerningMap::value_type(
              KerningMap::key_type(previous_c, current_c), dx));
        }
      }
    }
  }

  return fontchars;
}

std::pair<Rectf, Rectf>
ConstructCharacterRects(
    const stbrp_rect& src_rect,
    const FontChar&   src_char,
    int               image_width,
    int               image_height)
{
  const int vert_left   = src_char.bearing_x;
  const int vert_right  = vert_left + src_char.image.GetWidth();
  const int vert_top    = src_char.bearing_y;
  const int vert_bottom = vert_top - std::max(1, src_char.image.GetHeight());

  const float iw = image_width;
  const float ih = image_height;

  const stbrp_coord uv_left   = src_rect.x;
  const stbrp_coord uv_right  = uv_left + src_rect.w;
  const stbrp_coord uv_bottom = src_rect.y;
  const stbrp_coord uv_top =
      uv_bottom + std::max(static_cast<stbrp_coord>(1), src_rect.h);

  // todo: add ability to be a quad for tighter fit
  ASSERTX(vert_top > vert_bottom, vert_top, vert_bottom, src_char.c);
  ASSERTX(uv_top > uv_bottom, uv_top, uv_bottom, src_char.c);
  const auto sprite = Rectf::FromLeftRightTopBottom(
      vert_left, vert_right, vert_top, vert_bottom);
  const auto texture = Rectf::FromLeftRightTopBottom(
      uv_left / iw, uv_right / iw, uv_top / ih, uv_bottom / ih);

  return std::make_pair(sprite, texture);
}

////////////////////////////////////////////////////////////////////////////////

Font::Font(FileSystem* fs, const std::string& font_file)
{
  const int texture_width  = 512;
  const int texture_height = 512;

  FontChars  fontchars;
  font::Root font_root;

  std::string error = LoadProtoJson(fs, &font_root, font_file);
  if(!error.empty())
  {
    LOG_ERROR("Failed to load " << font_file << ": " << error);
  }
  for(const auto& source : font_root.sources())
  {
    if(source.has_font())
    {
      const font::FontFile& font = source.font();
      fontchars.CombineWith(GetCharactersFromFont(
          font.file(), font_root.size(), font.characters()));
    }
  }

  // pack char textures to a single texture
  const int               num_rects = fontchars.chars.size();
  std::vector<stbrp_rect> rects(num_rects);
  for(int i = 0; i < num_rects; ++i)
  {
    stbrp_rect& r = rects[i];
    r.id          = i;
    r.w           = fontchars.chars[i].image.GetWidth();
    r.h           = fontchars.chars[i].image.GetHeight();
  }
  stbrp_context           context{};
  const int               num_nodes = texture_width;
  std::vector<stbrp_node> nodes(num_nodes);
  stbrp_init_target(
      &context, texture_width, texture_height, &nodes[0], num_nodes);
  stbrp_pack_rects(&context, &rects[0], num_rects);

  CharDataMap map;
  Image       image;
  image.Setup(texture_width, texture_height, true);
  for(int i = 0; i < num_rects; ++i)
  {
    const stbrp_rect& src_rect = rects[i];
    if(src_rect.was_packed == 0)
    {
      std::cerr << "Failed to pack\n";
      continue;
    }
    const FontChar& src_char = fontchars.chars[src_rect.id];
    PasteCharacterToImage(&image, src_rect, src_char.image);
    const auto rects = ConstructCharacterRects(
        src_rect, src_char, texture_width, texture_height);

    std::shared_ptr<CharData> dest(
        new CharData(rects.first, rects.second, src_char.c, src_char.advance));
    map.insert(CharDataMap::value_type(dest->c, dest));
  }

  // load pixels into texture
  kerning_ = fontchars.kerning;
  chars_   = map;
  Texture2dLoadData load_data;
  texture_ = std::make_unique<Texture2d>();
  texture_->LoadFromImage(image, AlphaLoad::Keep, Texture2dLoadData());
}

void
Font::Draw(
    SpriteRenderer*    renderer,
    const vec2f&       start_position,
    const std::string& str,
    const Rgb&         base_color,
    const Rgb&         hi_color,
    int                hi_start,
    int                hi_end,
    float              scale) const
{
  vec2f position = start_position;

  const bool apply_highlight = hi_end != -1 && hi_start != -1;

  int         index           = 0;
  std::string last_char_index = "";
  for(char c : str)
  {
    const int this_index = index;
    ++index;
    const std::string char_index = ConvertCharToIndex(c);
    auto              it         = chars_.find(char_index);
    if(it == chars_.end())
    {
      LOG_ERROR("Failed to print " << char_index);
      continue;
    }
    std::shared_ptr<CharData> ch = it->second;

    const Rgb& color =
        apply_highlight && hi_start <= this_index && this_index < hi_end
            ? hi_color
            : base_color;
    // todo: support scaling
    renderer->DrawRect(
        *texture_.get(),
        ch->sprite_rect.OffsetCopy(position),
        ch->texture_rect,
        Angle::Zero(),
        vec2f{0.5f, 0.5f},
        Rgba{color});

    auto kerning = kerning_.find(std::make_pair(last_char_index, char_index));
    int  the_kerning = kerning == kerning_.end() ? 0 : kerning->second;
    position.x += (ch->advance + the_kerning) * scale;
  }
}

Rectf
Font::GetExtents(const std::string& str, float scale) const
{
  // todo: refactor rendering and extent to a compiled commands
  // and either render or get extent of that

  std::string last_char_index = "";
  vec2f       position(0.0f);
  Rectf       ret;

  for(char c : str)
  {
    // todo: support character ligatures
    const std::string char_index = ConvertCharToIndex(c);
    auto              it         = chars_.find(char_index);
    if(it == chars_.end())
    {
      continue;
    }
    std::shared_ptr<CharData> ch = it->second;

    ret.Include(ch->sprite_rect.OffsetCopy(position));

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

Text::Text(Font* font)
    : font_(font)
    , scale_(1.0f)
    , text_("")
    , base_color_(0.0f)
    , hi_color_(1.0f)
    , hi_from_(-1)
    , hi_to_(-1)
    , alignment_(Align::MIDDLE_LEFT)
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
Text::Draw(SpriteRenderer* renderer, const vec2f& p, float scale) const
{
  Draw(renderer, p, base_color_, scale);
}

void
Text::Draw(
    SpriteRenderer* renderer,
    const vec2f&    p,
    const Rgb&      override_color,
    float           scale) const
{
  if(font_ == nullptr)
  {
    return;
  }
  const Rectf& e   = GetExtents(scale);
  const vec2f  off = GetOffset(alignment_, e);
  if(use_background_)
  {
    // todo: render background using a white image rect with a tint
    // backgroundRenderer_->Draw(
    //    background_alpha_, e.ExtendCopy(5.0f).OffsetCopy(p + off));
  }
  font_->Draw(
      renderer,
      p + off,
      text_,
      override_color,
      hi_color_,
      hi_from_,
      hi_to_,
      scale_ * scale);
}

Rectf
Text::GetExtents(float scale) const
{
  return font_->GetExtents(text_, scale_ * scale);
}
