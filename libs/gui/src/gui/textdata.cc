#include "gui/textdata.h"

#include "assert/assert.h"
#include "log/log.h"

#include "render/font.h"


namespace eu::gui
{
    TextData::TextData()
        : font(nullptr)
        , size(-1.0f)
    {
    }


    TextData::~TextData() = default;


    void
    TextData::set_font(std::shared_ptr<render::DrawableFont> new_font)
    {
        font = new_font;
        text.reset();
        update_text();
    }


    const render::DrawableFont&
    TextData::get_font() const
    {
        ASSERT(font);
        return *font;
    }


    void
    TextData::update_string(const std::string& str)
    {
        string = str;
        update_text();
    }


    bool
    TextData::has_text() const
    {
        return text != nullptr;
    }


    const render::DrawableText&
    TextData::get_text() const
    {
        ASSERT(text);
        return *text;
    }


    render::DrawableText&
    TextData::get_text()
    {
        ASSERT(text);
        return *text;
    }


    void
    TextData::set_size(float new_size)
    {
        this->size = new_size;
        if(has_text())
        {
            get_text().set_size(new_size);
        }
    }


    void
    TextData::update_text()
    {
        if(text == nullptr && font != nullptr)
        {
            text = std::make_shared<render::DrawableText>(font.get());
        }

        if(text != nullptr)
        {
            // button assumes this is bottom left
            text->set_alignment(render::Align::bottom_left);

            core::UiText parsed;
            if(false == parsed.init_by_parsing_source(string))
            {
                LOG_ERROR("Failed to parse {0}", string);
            }
            LOG_INFO("Loaded {0}", core::textparser::VisitorDebugString::accept_all_nodes(&parsed));
            text->set_text(parsed);
            text->set_size(size);
        }
    }
}

