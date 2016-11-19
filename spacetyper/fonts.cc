#include "spacetyper/fonts.h"

#include "spacetyper/vao.h"
#include "spacetyper/texture.h"
#include "spacetyper/shader.h"

#include <vector>
#include <memory>
#include <map>
#include <cassert>
#include <iostream>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace {

void Error(FT_Error err) {
  if( err == 0 ) return;
  std::cerr << "FONT Error: " << err << "\n";
}
void ErrorNoThrow(FT_Error err) {
  if( err == 0 ) return;
  std::cerr << "FONT Error: " << err << "\n";
}

unsigned int ConvertCharToIndex(char c) {
  // not entirely sure this is correct
  return static_cast<unsigned int>(c);
}

}

struct Library {
  FT_Library  library;

  Library() {
    Error(FT_Init_FreeType( &library ));
  }

  ~Library() {
    ErrorNoThrow(FT_Done_FreeType(library));
  }
};

struct FontChar {
  FontChar() : valid(false) { }
  unsigned int c;
  bool valid;
  int glyph_width;
  int glyph_height;
  int bearing_x;
  int bearing_y;
  int advance;
  std::vector<unsigned char> pixels;
};

struct Face {
  FT_Face face;

  Face(Library* lib, const std::string& path, unsigned int size) {
    int face_index = 0;
    Error(FT_New_Face(lib->library, path.c_str(), face_index, &face));
    Error(FT_Set_Pixel_Sizes(face, 0, size));
  }

  FontChar GetChar(unsigned int c) {
    const FT_Error error = FT_Load_Char( face, c, FT_LOAD_RENDER );
    if ( error ) {
      std::cerr << "Failed to get char\n";
      return FontChar();
    }

    FT_GlyphSlot slot = face->glyph;


    FontChar ch;
    ch.c = c;
    ch.glyph_height = slot->bitmap.rows;
    ch.glyph_width = slot->bitmap.width;
    ch.bearing_x = slot->bitmap_left;
    ch.bearing_y = slot->bitmap_top;
    ch.valid = true;
    ch.advance = slot->advance.x >> 6;
    // pen_y += slot->advance.y >> 6;
    const unsigned long size = ch.glyph_width*ch.glyph_height;
    ch.pixels.resize(size, 0);
    memcpy(&ch.pixels[0], slot->bitmap.buffer, size);

    return ch;
  }

  ~Face() {
    FT_Done_Face(face);
  }
};

CharData::CharData(const VaoBuilder& data, const Extent& ex, unsigned int ch, float ad) : vao(data), extent(ex), c(ch), advance(ad) {
}

struct Pixels {
  Pixels(int texture_width, int texture_height) : pixels(texture_width * texture_height * 4, 0), texture_width_(texture_width), texture_height_(texture_height) {
  }

  void Set(int x, int y, unsigned char v) {
    const size_t id = (x + y*texture_width_)*4;
    pixels[id + 0] = 255;
    pixels[id + 1] = 255;
    pixels[id + 2] = 255;
    pixels[id + 3] = v;
  }

  std::vector<unsigned char> pixels;
  int texture_width_;
  int texture_height_;
};

void PasteCharacterToImage(Pixels &pixels, const stbrp_rect &r,
                           const FontChar &ch) {
  for(int y=0; y < ch.glyph_height; ++y) {
      for (int x = 0; x < ch.glyph_width; ++x) {
        const int id = x + y*ch.glyph_width;
        const unsigned char val = ch.pixels[id];
        pixels.Set(r.x+x, r.y+y, val);
      }
    }
}

struct FontChars {
  std::vector<FontChar> chars;
  KerningMap kerning;
};

FontChars GetCharactersFromFont(const std::string &font_file, unsigned int font_size, const std::string& chars) {
  Library lib;
  Face f(&lib, font_file, font_size);

  FontChars fontchars;
  fontchars.chars.reserve(chars.length());
  for (std::string::const_iterator c = chars.begin(); c != chars.end(); c++) {
    FontChar cc = f.GetChar(ConvertCharToIndex(*c));
    if(cc.valid == false) continue;
    fontchars.chars.push_back(cc);
  }

  std::cout << "Loaded " << fontchars.chars.size() << " characters from " << font_file << "\n";

  FT_Bool use_kerning = FT_HAS_KERNING( f.face );
  std::cout << "kerning..." << static_cast<int>(use_kerning) << "\n";
  if( use_kerning ) {
    std::cout << "kerning...\n";
    for(const FontChar& previous : fontchars.chars) {
      for(const FontChar& current : fontchars.chars) {
        if( previous.c == current.c) continue;
        FT_Vector delta;
        FT_Get_Kerning( f.face, previous.c, current.c,
                        FT_KERNING_DEFAULT, &delta);
        int dx = delta.x  >> 6;
        if( dx != 0 ) {
          fontchars.kerning.insert( KerningMap::value_type(KerningMap::key_type(previous.c, current.c), dx) );
        }
      }
    }
  }

  return fontchars;
}

std::pair<VaoBuilder, Extent> BuildCharVao(const stbrp_rect &src_rect, const FontChar &src_char, int image_width, int image_height) {
  //
  //             width
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
  
  const int vert_left = src_char.bearing_x;
  const int vert_right = vert_left + src_char.glyph_width;
  const int vert_top = -src_char.bearing_y;
  const int vert_bottom = vert_top + src_char.glyph_height;

  const float iw = image_width;
  const float ih = image_height;

  const stbrp_coord uv_left = src_rect.x;
  const stbrp_coord uv_right = uv_left + src_rect.w;
  const stbrp_coord uv_top = src_rect.y;
  const stbrp_coord uv_bottom = uv_top + src_rect.h;

  VaoBuilder builder;
  builder.quad(
      Point(vert_left, vert_top,
            uv_left/iw, uv_top/ih),
      Point(vert_right, vert_top,
            uv_right/iw, uv_top/ih),
      Point(vert_left, vert_bottom,
            uv_left/iw, uv_bottom/ih),
      Point(vert_right, vert_bottom,
            uv_right/iw, uv_bottom/ih)
  );
  return std::make_pair(builder, Extent::FromLRTD(vert_left, vert_right, vert_top, vert_bottom));
}

////////////////////////////////////////////////////////////////////////////////

Extent::Extent() : left(0.0f), right(0.0f), top(0.0f), bottom(0.0f) {
}

Extent::Extent(float l, float r, float t, float b) : left(l), right(r), top(t), bottom(b)  {
  assert(this);
}

Extent Extent::FromLRTD(float l, float r, float t, float d) {
  return Extent(l, r, t, d);
}

void Extent::Translate(const glm::vec2& p) {
  assert(this);
  left += p.x;
  right += p.x;
  top += p.y;
  bottom += p.y;
}

void Extent::Include(const Extent& o) {
  assert(this);
  left = std::min(left, o.left);
  right = std::max(right, o.right);
  top = std::min(top, o.top);
  bottom = std::max(bottom, o.bottom);
}

Extent Extent::AsTranslated(const glm::vec2& p) const{
  assert(this);
  Extent r = *this;
  r.Translate(p);
  return r;
}

Extent Extent::AsIncluded(const Extent& o) const {
  assert(this);
  Extent r = *this;
  r.Include(o);
  return r;
}

////////////////////////////////////////////////////////////////////////////////

Font::Font(Shader* shader, const std::string& font_file, unsigned int font_size, const std::string& possible_chars) : shader_(shader), font_size_(font_size) {
  const int texture_width = 512;
  const int texture_height = 512;

  const FontChars fontchars = GetCharactersFromFont(font_file, font_size, possible_chars);

  // pack char textures to a single texture
  const int num_rects = fontchars.chars.size();
  std::vector<stbrp_rect> rects(num_rects);
  for(int i=0; i<num_rects; ++i) {
    stbrp_rect& r = rects[i];
    r.id = i;
    r.w = fontchars.chars[i].glyph_width;
    r.h = fontchars.chars[i].glyph_height;
  }
  stbrp_context context;
  const int num_nodes = texture_width;
  std::vector<stbrp_node> nodes(num_nodes);
  stbrp_init_target(&context, texture_width, texture_height, &nodes[0], num_nodes);
  stbrp_pack_rects (&context, &rects[0], num_rects);

  CharDataMap map;
  Pixels pixels(texture_width, texture_height);
  for(int i=0; i<num_rects; ++i) {
    const stbrp_rect& src_rect = rects[i];
    if( src_rect.was_packed == 0) {
      std::cerr << "Failed to pack\n";
      continue;
    }
    const FontChar& src_char = fontchars.chars[src_rect.id];
    PasteCharacterToImage(pixels, src_rect, src_char);
    const std::pair<VaoBuilder, Extent> char_vao = BuildCharVao(src_rect, src_char, texture_width, texture_height);

    // store data in useful data
    std::shared_ptr<CharData> dest(new CharData(char_vao.first, char_vao.second, src_char.c, src_char.advance));
    map.insert(CharDataMap::value_type(dest->c, dest));
  }

  // load pixels into texture
  kerning_ = fontchars.kerning;
  chars_ = map;
  Texture2dLoadData load_data;
  texture_.reset(new Texture2d());
  texture_->Load(texture_width, texture_height, &pixels.pixels[0], GL_RGBA, GL_RGBA, load_data);
}

void Font::Draw(const glm::vec2& p, const std::string& str, glm::vec3 basec, glm::vec3 hic, int hi_start, int hi_end, float scale) const {
  Use(shader_);

  glm::vec2 position = p;

  glActiveTexture(GL_TEXTURE0);
  Use(texture_.get());

  const bool applyHi = hi_end != -1 && hi_start != -1;
  if(applyHi == false) {
    shader_->SetVector3f("spriteColor", basec);
  }

  int index = 0;
  unsigned int last_char_index = 0;
  for (std::string::const_iterator c = str.begin(); c != str.end(); c++) {
    const int this_index = index;
    ++index;
    const unsigned int char_index = ConvertCharToIndex(*c);
    CharDataMap::const_iterator it = chars_.find(char_index);
    if( it == chars_.end() ) {
      std::cerr << "Failed to print\n";
      continue;
    }
    std::shared_ptr<CharData> ch = it->second;

    glm::mat4 model = translate(glm::mat4(), glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
    shader_->SetMatrix4("model", model);

    if(applyHi) {
      bool useHiColor = hi_start <= this_index && this_index < hi_end;
      const glm::vec3& color = useHiColor ? hic : basec;
      shader_->SetVector3f("spriteColor", color);
    }
    ch->vao.Draw();
    KerningMap::const_iterator kerning = kerning_.find(std::make_pair(last_char_index, char_index));
    int the_kerning = kerning == kerning_.end() ? 0 : kerning->second;
    position.x += (ch->advance + the_kerning)*scale;
  }
}

Extent Font::GetExtents(const std::string& str, float scale) const {
  unsigned int last_char_index = 0;
  glm::vec2 position(0.0f);
  Extent ret;

  for (std::string::const_iterator c = str.begin(); c != str.end(); c++) {
    const unsigned int char_index = ConvertCharToIndex(*c);
    CharDataMap::const_iterator it = chars_.find(char_index);
    if( it == chars_.end() ) {
      continue;
    }
    std::shared_ptr<CharData> ch = it->second;

    // todo: increase extents
    ret.Include(ch->extent.AsTranslated(position));

    KerningMap::const_iterator kerning = kerning_.find(std::make_pair(last_char_index, char_index));
    int the_kerning = kerning == kerning_.end() ? 0 : kerning->second;
    position.x += (ch->advance + the_kerning)*scale;
  }

  return ret;
}

unsigned int Font::GetFontSize() const {
  return font_size_;
}

Text::Text(Font* font) : scale_(1.0f), font_(font), text_(""), base_color_(0.0f), hi_color_(1.0f), hi_from_(-1), hi_to_(-1) {
}

Text::Text(const std::string& str, Font* font) : scale_(1.0f), font_(font), text_(str), base_color_(0.0f), hi_color_(1.0f), hi_from_(-1), hi_to_(-1) {}

Text::~Text() {}

void Text::SetText(const std::string& str) {
  text_ = str;
}
void Text::SetFont(Font* font) {
  font_ = font;
}
void Text::SetBaseColor(const glm::vec3 color) {
  base_color_ = color;
}
void Text::SetHighlightColor(const glm::vec3 color) {
  hi_color_ = color;
}
void Text::SetHighlightRange(int from, int to) {
  hi_from_ = from;
  hi_to_ = to;
}

void Text::SetSize(float new_size) {
  assert(font_);
  SetScale(new_size/font_->GetFontSize());
}

void Text::SetScale(float scale) {
  scale_ = scale;
}

void Text::Draw(const glm::vec2& p) {
  if( font_ == nullptr) return;
  font_->Draw(p, text_, base_color_, hi_color_, hi_from_, hi_to_, scale_);
}
