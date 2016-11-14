#ifndef SPACETYPER_FONTS_H
#define SPACETYPER_FONTS_H

#include <string>
#include <map>
#include <memory>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

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

class Font;

class Text {
 public:
  Text(Font* font= nullptr);
  Text(const std::wstring& str, Font* font= nullptr);
  ~Text();

  void SetText(const std::wstring& str);
  void SetFont(Font* font);
  void SetBaseColor(const glm::vec3 color);
  void SetHighlightColor(const glm::vec3 color);
  void SetHighlightRange(int from, int to);

  void SetSize(float new_size);
  void SetScale(float scale);

  void Draw(const glm::vec2& p);
 private:
  float scale_;
  Font* font_;
  std::wstring text_;
  glm::vec3 base_color_;
  glm::vec3 hi_color_;
  int hi_from_;
  int hi_to_;
};

class Font {
 public:
  Font(Shader* shader, const std::string& path, unsigned int font_size, const std::wstring& chars);
  unsigned int GetFontSize() const;
 protected:
  friend void Text::Draw(const glm::vec2 &p);
  void Draw(const glm::vec2& p, const std::wstring& str, glm::vec3 basec, glm::vec3 hic, int hi_start, int hi_end, float scale) const;
 private:
  Shader* shader_;
  unsigned int font_size_;
  std::unique_ptr<Texture2d> texture_;
  CharDataMap chars_;
  KerningMap kerning_;
};

#endif  // SPACETYPER_FONTS_H