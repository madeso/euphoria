#include "gui/textdata.h"

#include "core/assert.h"

#include "render/fonts.h"

TextData::TextData() : font_(nullptr), backgroundRenderer_(nullptr) {
}
TextData::~TextData() {
}

void TextData::SetFont(Font* font) {
  Assert(this);
  font_ = font;
  text_.reset();
  UpdateText();
}

const Font& TextData::GetFont() const {
  Assert(this);
  Assert(font_);
  return *font_;
}

void TextData::SetBackgroundRenderer(TextBackgroundRenderer* renderer) {
  Assert(this);
  backgroundRenderer_ = renderer;
  text_.reset();
  UpdateText();
}

void TextData::SetString(const std::string& str) {
  Assert(this);
  string_ = str;
  UpdateText();
}

const std::string& TextData::GetString() const {
  Assert(this);
  return string_;
}

bool TextData::HasText() const {
  Assert(this);
  return text_.get() != nullptr;
}

const Text& TextData::GetText() const {
  Assert(this);
  Assert(text_.get());
  return *text_.get();
}

void TextData::UpdateText() {
  Assert(this);
  if( text_.get() == nullptr && backgroundRenderer_ != nullptr && font_ != nullptr ) {
    text_.reset( new Text {font_, backgroundRenderer_} );
  }

  if( text_.get() != nullptr) {
    text_->SetText(string_);
  }
}
