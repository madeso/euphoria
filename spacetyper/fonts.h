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
  void Extend(float value);
  Extent AsTranslated(const glm::vec2& p) const;
  Extent AsIncluded(const Extent& o) const;
  Extent AsExtended(float value) const;

  float GetWidth() const;
  float GetHeight() const;

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

class TextBackgroundRenderer {
 public:
  TextBackgroundRenderer(Shader* shader);

  void Draw(float alpha, const Extent& area);
 private:
  Vao vao_;
  Shader* shader_;
};

class Text {
 public:
  Text(Font* font, TextBackgroundRenderer* back);
  ~Text();

  void SetText(const std::string& str);
  const std::string& GetText() const;
  void SetBaseColor(const glm::vec3 color);
  void SetHighlightColor(const glm::vec3 color);
  void SetHighlightRange(int from, int to);
  void SetBackground(bool use_background, float alpha=0.5f);
  void SetAlignment(Align alignment);

  void SetSize(float new_size);
  void SetScale(float scale);

  void Draw(const glm::vec2& p);

  Extent GetExtents() const;
 private:
  Font* font_;
  TextBackgroundRenderer* backgroundRenderer_;
  float scale_;
  std::string text_;
  glm::vec3 base_color_;
  glm::vec3 hi_color_;
  int hi_from_;
  int hi_to_;
  Align alignment_;

  bool use_background_;
  float background_alpha_;
};

class Font {
 public:
  Font(Shader* shader, const std::string& path, unsigned int font_size, const std::string& chars);
  unsigned int GetFontSize() const;
 protected:
  friend void Text::Draw(const glm::vec2 &p);
  friend Extent Text::GetExtents() const;

  // todo: support drawing background color behind string
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