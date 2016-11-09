#ifndef SPACETYPER_FONTS_H
#define SPACETYPER_FONTS_H

#include <string>
#include <map>
#include <memory>

#include "spacetyper/vao.h"
#include "spacetyper/texture.h"

class Shader;

struct CharData {
  explicit CharData(const VaoBuilder& data);

  unsigned int c;
  float advance;

  // only useful when calculating font metrics for a specific text
  float glyph_width;
  float glyph_height;
  float bearing_x;
  float bearing_y;

  Vao vao;
};

typedef std::map<unsigned int, std::shared_ptr<CharData>> CharDataMap;
typedef std::map<std::pair<unsigned int, unsigned int>, int> KerningMap;

class Font {
 public:
  Font(Shader* shader, const std::string& path, unsigned int font_size, const std::wstring& chars);
  void Draw(const glm::vec2& p, const std::wstring& str) const;
 private:
  Shader* shader_;
  std::unique_ptr<Texture2d> texture_;
  CharDataMap chars_;
  KerningMap kerning_;
};

#endif  // SPACETYPER_FONTS_H