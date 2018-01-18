#ifndef SPACETYPER_FONTS_H
#define SPACETYPER_FONTS_H

#include <string>
#include <map>
#include <memory>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/rgb.h"
#include "core/rect.h"

#include "render/texture.h"

class FileSystem;
class SpriteRenderer;
class TextureCache;

struct Glyph
{
  Glyph(
      const Rectf&       sprite,
      const Rectf&       texture,
      const std::string& ch,
      float              ad);

  Rectf       sprite_rect;   // relative to 0,0
  Rectf       texture_rect;  // image texture uvs
  std::string c;             // the character or string id
  float       advance;
};

typedef std::map<std::string, std::shared_ptr<Glyph>> CharDataMap;
typedef std::map<std::pair<std::string, std::string>, int> KerningMap;

class Font;

enum class Align
{
  TOP_LEFT,
  TOP_CENTER,
  TOP_RIGHT,
  BASELINE_LEFT,
  BASELINE_CENTER,
  BASELINE_RIGHT,
  BOTTOM_LEFT,
  BOTTOM_CENTER,
  BOTTOM_RIGHT
};

struct TextDrawCommand
{
  const Texture2d* texture;
  Rectf            sprite_rect;
  Rectf            texture_rect;
  Rgb              tint;

  TextDrawCommand(
      const Texture2d* texture,
      const Rectf&     sprite_rect,
      const Rectf&     texture_rect,
      const Rgb&       tint);
};

struct TextDrawCommandList
{
  std::vector<TextDrawCommand> commands;

  void
  Add(const Texture2d* texture,
      const Rectf&     sprite_rect,
      const Rectf&     texture_rect,
      const Rgb&       tint);

  void
  Draw(SpriteRenderer* renderer, const vec2f& start_position);
};

class Text
{
 public:
  explicit Text(Font* font);
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
  Draw(SpriteRenderer* renderer, const vec2f& p) const;

  void
  Draw(SpriteRenderer* renderer, const vec2f& p, const Rgb& override_color)
      const;

  Rectf
  GetExtents() const;

 private:
  Font*       font_;
  float       scale_;
  std::string text_;
  Rgb         base_color_;
  Rgb         hi_color_;
  int         hi_from_;
  int         hi_to_;
  Align       alignment_;

  bool  use_background_;
  float background_alpha_;
};

class Font
{
 public:
  Font(FileSystem* fs, TextureCache* cache, const std::string& font_file);
  unsigned int
  GetFontSize() const;

 protected:
  friend void
  Text::Draw(
      SpriteRenderer* renderer,
      const vec2f&    p,
      const Rgb&      override_color) const;

  friend Rectf
  Text::GetExtents() const;

  void
  DrawBackground(SpriteRenderer* renderer, float alpha, const Rectf& where);

  TextDrawCommandList
  CompileList(const std::string& str,
              const Rgb&         base_color,
              const Rgb&         hi_color,
              int                hi_start,
              int                hi_end,
              float              scale) const;

  // todo: replace scale with size
  void
  Draw(
      SpriteRenderer*    renderer,
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
  unsigned int               font_size_;
  std::unique_ptr<Texture2d> texture_;
  std::shared_ptr<Texture2d> background;
  CharDataMap                chars_;
  KerningMap                 kerning_;
};

#endif  // SPACETYPER_FONTS_H
