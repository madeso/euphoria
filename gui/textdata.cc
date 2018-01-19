#include "gui/textdata.h"

#include "core/assert.h"

#include "render/fonts.h"

TextData::TextData()
    : font_(nullptr)
{
}

TextData::~TextData()
{
}

void
TextData::SetFont(std::shared_ptr<Font> font)
{
  font_ = font;
  text_.reset();
  UpdateText();
}

const Font&
TextData::GetFont() const
{
  ASSERT(font_);
  return *font_;
}

void
TextData::SetString(const std::string& str)
{
  string_ = str;
  UpdateText();
}

const std::string&
TextData::GetString() const
{
  return string_;
}

bool
TextData::HasText() const
{
  return text_ != nullptr;
}

const Text&
TextData::GetText() const
{
  ASSERT(text_);
  return *text_;
}

Text&
TextData::GetText()
{
  ASSERT(text_);
  return *text_;
}

void
TextData::SetSize(float size)
{
  this->size = size;
  if(HasText())
  {
    GetText().SetSize(size);
  }
}

void
TextData::UpdateText()
{
  if(text_ == nullptr && font_ != nullptr)
  {
    text_.reset(new Text{font_.get()});
  }

  if(text_ != nullptr)
  {
    // button assumes this is bottom left
    text_->SetAlignment(Align::BOTTOM_LEFT);

    ParsedText text;
    text.CreateParse(string_);
    text_->SetText(text);
    text_->SetSize(size);
  }
}
