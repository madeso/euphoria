#include "core/ui_text.h"

#include "core/log.h"

#include <sstream>

namespace euphoria::core
{
    LOG_SPECIFY_DEFAULT_LOGGER("core.textparser")

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace textparser
    {
        node_text::node_text(const std::string& t) : text(t) {}

        void
        node_text::accept(visitor* visitor) const
        {
            visitor->on_text(text);
        }

        node_image::node_image(const std::string& t) : image(t) {}

        void
        node_image::accept(visitor* visitor) const
        {
            visitor->on_image(image);
        }

        node_begin_or_end::node_begin_or_end(bool b) : begin(b) {}

        void
        node_begin_or_end::accept(visitor* visitor) const
        {
            if(begin)
            {
                visitor->on_begin();
            }
            else
            {
                visitor->on_end();
            }
        }
    }  // namespace textparser

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace textparser
    {
        void
        visitor_debug_string::on_text(const std::string& text)
        {
            ss << "{text " << text << "}";
        }

        void
        visitor_debug_string::on_image(const std::string& img)
        {
            ss << "{image " << img << "}";
        }

        void
        visitor_debug_string::on_begin()
        {
            ss << "{begin}";
        }

        void
        visitor_debug_string::on_end()
        {
            ss << "{end}";
        }

        std::string
        visitor_debug_string::accept_all_nodes(ui_text* visitor)
        {
            visitor_debug_string str;
            visitor->accept(&str);
            return str.ss.str();
        }
    }  // namespace textparser

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void
    ui_text::clear()
    {
        nodes.resize(0);
    }

    void
    ui_text::add_text(const std::string& str)
    {
        nodes.emplace_back(std::make_shared<textparser::node_text>(str));
    }

    void
    ui_text::add_image(const std::string& img)
    {
        nodes.emplace_back(std::make_shared<textparser::node_image>(img));
    }

    void
    ui_text::add_begin()
    {
        nodes.emplace_back(std::make_shared<textparser::node_begin_or_end>(true));
    }

    void
    ui_text::add_end()
    {
        nodes.emplace_back(std::make_shared<textparser::node_begin_or_end>(false));
    }

    void
    ui_text::init_with_text(const std::string& str)
    {
        clear();
        add_text(str);
    }

    namespace
    {
        enum class state
        {
            text,
            image
        };


        // todo(Gustav): this parser looks shitty... improve?
        struct parser
        {
            ui_text* nodes = nullptr;

            state             state  = state::text;
            bool              escape = false;
            std::stringstream buff;
            bool              ok = true;

            void
            close()
            {
                const std::string data = buff.str();
                buff.str("");
                switch(state)
                {
                case state::text:
                    if(!data.empty())
                    {
                        nodes->add_text(data);
                    }
                    break;
                case state::image:
                    if(!data.empty())
                    {
                        nodes->add_image(data);
                    }
                    else
                    {
                        LOG_ERROR("Tried to create a empty @image");
                        ok = false;
                    }
                    break;
                }
            }

            void
            on_char(char c)
            {
                switch(state)
                {
                case state::text:
                    if(escape)
                    {
                        buff << c;
                        escape = false;
                    }
                    else
                    {
                        switch(c)
                        {
                        case '@':
                            close();
                            state = state::image;
                            break;
                        case '{':
                            close();
                            nodes->add_begin();
                            break;
                        case '}':
                            close();
                            nodes->add_end();
                            break;
                        case '\\': escape = true; break;
                        default: buff << c; break;
                        }
                    }
                    break;
                case state::image:
                    if(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')
                       || c == '-' || c == '_')
                    {
                        buff << c;
                    }
                    else
                    {
                        close();
                        state = state::text;
                        if(c == ' ')
                        {
                            // nop
                        }
                        else
                        {
                            on_char(c);
                            return;
                        }
                    }
                    break;
                }
            }
        };
    }  // namespace

    bool
    ui_text::init_by_parsing_source(const std::string& str)
    {
        nodes.resize(0);
        parser parser;
        parser.nodes = this;

        for(char c: str)
        {
            parser.on_char(c);
            if(!parser.ok)
            {
                return false;
            }
        }

        parser.close();
        if(!parser.ok)
        {
            return false;
        }
        if(parser.escape)
        {
            LOG_ERROR("Ending with escape");
            return false;
        }
        return true;
    }

    ui_text
    ui_text::create_from_text(const std::string& str)
    {
        ui_text text;
        text.init_with_text(str);
        return text;
    }

    void
    ui_text::accept(textparser::visitor* visitor)
    {
        for(auto& node: nodes)
        {
            node->accept(visitor);
        }
    }

    void
    ui_text::accept(textparser::visitor* visitor) const
    {
        for(const auto& node: nodes)
        {
            node->accept(visitor);
        }
    }

}
