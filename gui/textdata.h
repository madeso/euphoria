#ifndef GUI_TEXTDATA_H
#define GUI_TEXTDATA_H

#include <memory>
#include <string>

class Font;
class Text;

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
  SetString(const std::string& str);
  const std::string&
  GetString() const;

  bool
  HasText() const;

  const Text&
  GetText() const;

  Text&
  GetText();

 private:
  void
                        UpdateText();
  std::shared_ptr<Font> font_;
  std::string           string_;

  std::shared_ptr<Text> text_;
};

#endif  // GUI_TEXTDATA_H
