#ifndef SPACETYPER_FONTS_H
#define SPACETYPER_FONTS_H

#include <string>
#include <map>
#include <memory>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/rgb.h"
#include "core/rect.h"

#include "core/textparser.h"

#include "render/texture.h"

namespace vfs
{
class FileSystem;
}

class SpriteRenderer;
class TextureCache;

// todo: seperate rendering and the rest and move to core

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
typedef std::map<std::pair<std::string, std::string>, float> KerningMap;

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
  bool             hi;

  TextDrawCommand(
      const Texture2d* texture,
      const Rectf&     sprite_rect,
      const Rectf&     texture_rect,
      bool             hi);
};

struct TextDrawCommandList
{
  std::vector<TextDrawCommand> commands;

  void
  Add(const Texture2d* texture,
      const Rectf&     sprite_rect,
      const Rectf&     texture_rect,
      bool             hi);

  void
  Draw(
      SpriteRenderer* renderer,
      const vec2f&  start_position,
      const Rgb&      base_color,
      const Rgb&      hi_color);

  Rectf
  GetExtents() const;
};

class Text
{
 public:
  explicit Text(Font* font);
  ~Text();

  void
  SetText(const ParsedText& text);

  void
  SetBackground(bool use_background, float alpha = 0.5f);

  void
  SetAlignment(Align alignment);

  void
  SetSize(float new_size);

  void
  Draw(SpriteRenderer* renderer, const vec2f& p, const Rgb& base_hi_color)
      const;

  void
  Draw(
      SpriteRenderer* renderer,
      const vec2f&  p,
      const Rgb&      base_color,
      const Rgb&      hi_color) const;

  Rectf
  GetExtents() const;

  void
  Compile() const;

 private:
  const Font* font_;
  float       size_;
  ParsedText  text_;
  Align       alignment_;

  bool  use_background_;
  float background_alpha_;

  // updated in Compile function
  mutable bool                dirty;
  mutable TextDrawCommandList commands;
};

class Font
{
 public:
  Font(vfs::FileSystem* fs, TextureCache* cache, const std::string& font_file);

  // todo: expose background property and move this away from font
  void
  DrawBackground(
      SpriteRenderer* renderer, float alpha, const Rectf& where) const;

  TextDrawCommandList
  CompileList(const ParsedText& text, float size) const;

 private:
  std::unique_ptr<Texture2d> texture_;
  std::shared_ptr<Texture2d> background;
  CharDataMap                chars_;
  KerningMap                 kerning_;
};

#endif  // SPACETYPER_FONTS_H
