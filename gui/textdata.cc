#include "gui/textdata.h"

#include <cassert>

#include "render/fonts.h"

TextData::TextData() {
}
TextData::~TextData() {
}

void TextData::SetFont(Font* font) {
  assert(this);
  font_ = font;
  text_.reset();
  UpdateText();
}

const Font& TextData::GetFont() const {
  assert(this);
  assert(font_);
  return *font_;
}

void TextData::SetBackgroundRenderer(TextBackgroundRenderer* renderer) {
  assert(this);
  backgroundRenderer_ = renderer;
  text_.reset();
  UpdateText();
}

void TextData::SetString(const std::string& str) {
  assert(this);
  string_ = str;
  UpdateText();
}

bool TextData::HasText() const {
  assert(this);
  return text_.get() != nullptr;
}

const Text& TextData::GetText() const {
  assert(this);
  assert(text_.get());
  return *text_.get();
}

void TextData::UpdateText() {
  assert(this);
  if( text_.get() == nullptr && backgroundRenderer_ != nullptr && font_ != nullptr ) {
    text_.reset( new Text {font_, backgroundRenderer_} );
  }

  if( text_.get() != nullptr) {
    text_->SetText(string_);
  }
}
