#ifndef SPACETYPER_FONTS_H
#define SPACETYPER_FONTS_H

#include <string>
#include <map>
#include <memory>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/rgb.h"
#include "core/rect.h"

#include "render/buffer2d.h"
#include "render/texture.h"
#include "render/shaderuniform.h"

class Shader;
class BufferBuilder2d;

struct CharData
{
  CharData(
      const BufferBuilder2d& data, const Rectf& ex, unsigned int ch, float ad);

  Buffer2d     buffer;
  Rectf        extent;
  unsigned int c;
  float        advance;
};

typedef std::map<unsigned int, std::shared_ptr<CharData>> CharDataMap;
typedef std::map<std::pair<unsigned int, unsigned int>, int> KerningMap;

class Font;

enum class Align
{
  TOP_LEFT,
  TOP_CENTER,
  TOP_RIGHT,
  MIDDLE_LEFT,
  MIDDLE_CENTER,
  MIDDLE_RIGHT,
  BOTTOM_LEFT,
  BOTTOM_CENTER,
  BOTTOM_RIGHT,
  LEFT   = MIDDLE_LEFT,
  CENTER = MIDDLE_CENTER,
  RIGHT  = MIDDLE_RIGHT,
};

class TextBackgroundRenderer
{
 public:
  TextBackgroundRenderer(Shader* shader);

  void
  Draw(float alpha, const Rectf& area);

 private:
  Buffer2d      buffer_;
  Shader*       shader_;
  ShaderUniform model_;
  ShaderUniform color_;
};

class Text
{
 public:
  Text(Font* font, TextBackgroundRenderer* back);
  ~Text();

  void
  SetText(const std::string& str);
  const std::string&
  GetText() const;
  void
  SetBaseColor(const Rgb& color);
  void
  SetHighlightColor(const Rgb& color);
  void
  SetHighlightRange(int from, int to);
  void
  SetBackground(bool use_background, float alpha = 0.5f);
  void
  SetAlignment(Align alignment);

  void
  SetSize(float new_size);
  void
  SetScale(float scale);

  void
  Draw(const vec2f& p) const;
  void
  Draw(const vec2f& p, const Rgb& override_color) const;

  Rectf
  GetExtents() const;

 private:
  Font*                   font_;
  TextBackgroundRenderer* backgroundRenderer_;
  float                   scale_;
  std::string             text_;
  Rgb                     base_color_;
  Rgb                     hi_color_;
  int                     hi_from_;
  int                     hi_to_;
  Align                   alignment_;

  bool  use_background_;
  float background_alpha_;
};

class Font
{
 public:
  Font(
      Shader*            shader,
      const std::string& font_file,
      unsigned int       font_size,
      const std::string& possible_chars);
  unsigned int
  GetFontSize() const;

 protected:
  friend void
  Text::Draw(const vec2f& p, const Rgb& override_color) const;
  friend Rectf
  Text::GetExtents() const;

  // todo: support drawing background color behind string
  void
  Draw(
      const vec2f&       start_position,
      const std::string& str,
      const Rgb&         base_color,
      const Rgb&         hi_color,
      int                hi_start,
      int                hi_end,
      float              scale) const;
  Rectf
  GetExtents(const std::string& str, float scale) const;

 private:
  Shader*                    shader_;
  unsigned int               font_size_;
  ShaderUniform              color_;
  ShaderUniform              model_;
  std::unique_ptr<Texture2d> texture_;
  CharDataMap                chars_;
  KerningMap                 kerning_;
};

#endif  // SPACETYPER_FONTS_H
