#include "gui/textdata.h"

#include "core/assert.h"

#include "render/fonts.h"


namespace euphoria::gui
{
    TextData::TextData() : font_(nullptr) {}

    TextData::~TextData() {}

    void
    TextData::SetFont(std::shared_ptr<render::Font> font)
    {
        font_ = font;
        text_.reset();
        UpdateText();
    }

    const render::Font&
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

    const render::Text&
    TextData::GetText() const
    {
        ASSERT(text_);
        return *text_;
    }

    render::Text&
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
            text_.reset(new render::Text {font_.get()});
        }

        if(text_ != nullptr)
        {
            // button assumes this is bottom left
            text_->SetAlignment(render::Align::BOTTOM_LEFT);

            core::ParsedText text;
            text.CreateParse(string_);
            text_->SetText(text);
            text_->SetSize(size);
        }
    }
}  // namespace euphoria::gui
