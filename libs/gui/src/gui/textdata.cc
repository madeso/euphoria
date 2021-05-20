#include "gui/textdata.h"

#include "core/assert.h"
#include "core/log.h"

#include "render/font.h"


namespace euphoria::gui
{
    TextData::TextData()
        : font_(nullptr)
        , size(-1.0f)
    {
    }


    TextData::~TextData() = default;


    void
    TextData::SetFont(std::shared_ptr<render::drawable_font> font)
    {
        font_ = font;
        text_.reset();
        UpdateText();
    }


    const render::drawable_font&
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


    const render::drawable_text&
    TextData::GetText() const
    {
        ASSERT(text_);
        return *text_;
    }


    render::drawable_text&
    TextData::GetText()
    {
        ASSERT(text_);
        return *text_;
    }


    void
    TextData::SetSize(float new_size)
    {
        this->size = new_size;
        if(HasText())
        {
            GetText().set_size(new_size);
        }
    }


    void
    TextData::UpdateText()
    {
        if(text_ == nullptr && font_ != nullptr)
        {
            text_ = std::make_shared<render::drawable_text>(font_.get());
        }

        if(text_ != nullptr)
        {
            // button assumes this is bottom left
            text_->set_alignment(render::align::bottom_left);

            core::ui_text text;
            if(false == text.init_by_parsing_source(string_))
            {
                LOG_ERROR("Failed to parse {0}", string_);
            }
            LOG_INFO("Loaded {0}", core::textparser::visitor_debug_string::accept_all_nodes(&text));
            text_->set_text(text);
            text_->set_size(size);
        }
    }
}

