#ifndef GUI_TEXTDATA_H
#define GUI_TEXTDATA_H

#include <memory>
#include <string>

class Font;
class Text;
class TextBackgroundRenderer;

class TextData
{
 public:
  TextData();
  ~TextData();

  void
  SetFont(std::shared_ptr<Font> font);
  const Font&
  GetFont() const;
  void
  SetBackgroundRenderer(TextBackgroundRenderer* renderer);
  void
  SetString(const std::string& str);
  const std::string&
  GetString() const;

  bool
  HasText() const;
  const Text&
  GetText() const;

 private:
  void
                          UpdateText();
  std::shared_ptr<Font>   font_;
  TextBackgroundRenderer* backgroundRenderer_;
  std::string             string_;

  std::shared_ptr<Text> text_;
};

#endif  // GUI_TEXTDATA_H
