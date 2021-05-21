#include "gui/textdata.h"

#include "core/assert.h"
#include "core/log.h"

#include "render/font.h"


namespace euphoria::gui
{
    text_data::text_data()
        : font(nullptr)
        , size(-1.0f)
    {
    }


    text_data::~text_data() = default;


    void
    text_data::set_font(std::shared_ptr<render::drawable_font> font)
    {
        font = font;
        text.reset();
        update_text();
    }


    const render::drawable_font&
    text_data::get_font() const
    {
        ASSERT(font);
        return *font;
    }


    void
    text_data::update_string(const std::string& str)
    {
        string = str;
        update_text();
    }


    bool
    text_data::has_text() const
    {
        return text != nullptr;
    }


    const render::drawable_text&
    text_data::get_text() const
    {
        ASSERT(text);
        return *text;
    }


    render::drawable_text&
    text_data::get_text()
    {
        ASSERT(text);
        return *text;
    }


    void
    text_data::set_size(float new_size)
    {
        this->size = new_size;
        if(has_text())
        {
            get_text().set_size(new_size);
        }
    }


    void
    text_data::update_text()
    {
        if(text == nullptr && font != nullptr)
        {
            text = std::make_shared<render::drawable_text>(font.get());
        }

        if(text != nullptr)
        {
            // button assumes this is bottom left
            text->set_alignment(render::align::bottom_left);

            core::ui_text parsed;
            if(false == parsed.init_by_parsing_source(string))
            {
                LOG_ERROR("Failed to parse {0}", string);
            }
            LOG_INFO("Loaded {0}", core::textparser::visitor_debug_string::accept_all_nodes(&parsed));
            text->set_text(parsed);
            text->set_size(size);
        }
    }
}

