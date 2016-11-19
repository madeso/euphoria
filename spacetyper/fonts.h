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

class Extent {
 public:
  Extent();
 private:
  Extent(float l, float r, float t, float d);
 public:
  static Extent FromLRTD(float l, float r, float t, float d);

  void Translate(const glm::vec2& p);
  void Include(const Extent& o);
  Extent AsTranslated(const glm::vec2& p) const;
  Extent AsIncluded(const Extent& o) const;

  float left;
  float right;
  float top;
  float bottom;
};

struct CharData {
  CharData(const VaoBuilder& data, const Extent& extent, unsigned int c, float advance);
  Vao vao;
  Extent extent;

  unsigned int c;
  float advance;
};

typedef std::map<unsigned int, std::shared_ptr<CharData>> CharDataMap;
typedef std::map<std::pair<unsigned int, unsigned int>, int> KerningMap;

class Font;

enum class Align {
  TOP_LEFT, TOP_CENTER, TOP_RIGHT,
  MIDDLE_LEFT, MIDDLE_CENTER, MIDDLE_RIGHT,
  BOTTOM_LEFT, BOTTOM_CENTER, BOTTOM_RIGHT,
  LEFT=MIDDLE_LEFT, CENTER=MIDDLE_CENTER, RIGHT=MIDDLE_RIGHT,
};

class Text {
 public:
  Text(Font* font= nullptr);
  Text(const std::string& str, Font* font= nullptr);
  ~Text();

  void SetText(const std::string& str);
  void SetFont(Font* font);
  void SetBaseColor(const glm::vec3 color);
  void SetHighlightColor(const glm::vec3 color);
  void SetHighlightRange(int from, int to);
  void SetAlignment(Align alignment);

  void SetSize(float new_size);
  void SetScale(float scale);

  void Draw(const glm::vec2& p);
 private:
  float scale_;
  Font* font_;
  std::string text_;
  glm::vec3 base_color_;
  glm::vec3 hi_color_;
  int hi_from_;
  int hi_to_;
  Align alignment_;
};

class Font {
 public:
  Font(Shader* shader, const std::string& path, unsigned int font_size, const std::string& chars);
  unsigned int GetFontSize() const;
 protected:
  friend void Text::Draw(const glm::vec2 &p);
  void Draw(const glm::vec2& p, const std::string& str, glm::vec3 basec, glm::vec3 hic, int hi_start, int hi_end, float scale) const;
  Extent GetExtents(const std::string& str, float scale) const;
 private:
  Shader* shader_;
  unsigned int font_size_;
  std::unique_ptr<Texture2d> texture_;
  CharDataMap chars_;
  KerningMap kerning_;
};

#endif  // SPACETYPER_FONTS_H