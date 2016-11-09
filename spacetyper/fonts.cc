#include "spacetyper/fonts.h"

#include "spacetyper/vao.h"
#include "spacetyper/texture.h"

#include <vector>
#include <memory>
#include <map>
#include <iostream>

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

unsigned int ConvertWcharToIndex(wchar_t c) {
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
  unsigned int c;
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

CharData::CharData(const VaoBuilder& data) : vao(data) {
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

FontChars GetCharactersFromFont(const std::string &font_file, unsigned int font_size, const std::wstring& chars) {
  Library lib;
  Face f(&lib, font_file, font_size);

  FontChars fontchars;
  fontchars.chars.reserve(chars.length());
  for (std::wstring::const_iterator c = chars.begin(); c != chars.end(); c++) {
    FontChar cc = f.GetChar(ConvertWcharToIndex(*c));
    if(cc.glyph_width == 0) continue;
    fontchars.chars.push_back(cc);
  }

  std::cout << "Loaded " << fontchars.chars.size() << " characters from " << font_file << "\n";

  FT_Bool use_kerning = FT_HAS_KERNING( f.face );
  if( use_kerning ) {
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

VaoBuilder BuildCharVao(const stbrp_rect &src_rect, const FontChar &src_char) {
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
  const int vert_top = src_char.bearing_y;
  const int vert_bottom = vert_top - src_char.glyph_height;

  const stbrp_coord uv_left = src_rect.x;
  const stbrp_coord uv_right = uv_left + src_rect.w;
  const stbrp_coord uv_top = src_rect.y;
  const stbrp_coord uv_bottom = uv_top + src_rect.h;

  VaoBuilder builder;
  builder.quad(
      Point(vert_left, vert_top,
            uv_left, uv_top),
      Point(vert_right, vert_top,
            uv_right, uv_top),
      Point(vert_left, vert_bottom,
            uv_left, uv_bottom),
      Point(vert_right, vert_bottom,
            uv_right, uv_bottom)
  );
  return builder;
}

Font::Font(const std::string& font_file, unsigned int font_size, const std::wstring& possible_chars) {
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
    const VaoBuilder builder = BuildCharVao(src_rect, src_char);

    // store data in useful data
    std::shared_ptr<CharData> dest(new CharData(builder));
    dest->glyph_width = src_char.glyph_width;
    dest->glyph_height = src_char.glyph_height;
    dest->advance = src_char.advance;
    dest->bearing_x = src_char.bearing_x;
    dest->bearing_y = src_char.bearing_y;
    dest->c = src_char.c;

    map.insert(CharDataMap::value_type(dest->c, dest));
  }

  // load pixels into texture
  kerning_ = fontchars.kerning;
  chars_ = map;
  Texture2dLoadData load_data;
  texture_.reset(new Texture2d());
  texture_->Load(texture_width, texture_height, &pixels.pixels[0], GL_RGBA, GL_RGBA, load_data);
}

void Font::Draw(const glm::vec2& p, const std::wstring& str) const {
  // implement me
}
