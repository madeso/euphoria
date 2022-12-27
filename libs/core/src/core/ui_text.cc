#include "core/ui_text.h"

#include "log/log.h"

#include "core/stringbuilder.h"


namespace euphoria::core
{
    namespace textparser
    {
        NodeText::NodeText(const std::string& t) : text(t) {}

        void
        NodeText::accept(Visitor* visitor) const
        {
            visitor->on_text(text);
        }

        NodeImage::NodeImage(const std::string& t) : image(t) {}

        void
        NodeImage::accept(Visitor* visitor) const
        {
            visitor->on_image(image);
        }

        NodeBeginOrEnd::NodeBeginOrEnd(bool b) : begin(b) {}

        void
        NodeBeginOrEnd::accept(Visitor* visitor) const
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
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace textparser
    {
        void
        VisitorDebugString::on_text(const std::string& text)
        {
            ss << "{text " << text << "}";
        }

        void
        VisitorDebugString::on_image(const std::string& img)
        {
            ss << "{image " << img << "}";
        }

        void
        VisitorDebugString::on_begin()
        {
            ss << "{begin}";
        }

        void
        VisitorDebugString::on_end()
        {
            ss << "{end}";
        }

        std::string
        VisitorDebugString::accept_all_nodes(UiText* visitor)
        {
            VisitorDebugString str;
            visitor->accept(&str);
            return str.ss.str();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void
    UiText::clear()
    {
        nodes.resize(0);
    }

    void
    UiText::add_text(const std::string& str)
    {
        nodes.emplace_back(std::make_shared<textparser::NodeText>(str));
    }

    void
    UiText::add_image(const std::string& img)
    {
        nodes.emplace_back(std::make_shared<textparser::NodeImage>(img));
    }

    void
    UiText::add_begin()
    {
        nodes.emplace_back(std::make_shared<textparser::NodeBeginOrEnd>(true));
    }

    void
    UiText::add_end()
    {
        nodes.emplace_back(std::make_shared<textparser::NodeBeginOrEnd>(false));
    }

    void
    UiText::init_with_text(const std::string& str)
    {
        clear();
        add_text(str);
    }

    namespace
    {
        enum class ParserState
        {
            text,
            image
        };


        // todo(Gustav): this parser looks shitty... improve?
        struct Parser
        {
            UiText* nodes = nullptr;

            ParserState state = ParserState::text;
            bool escape = false;
            StringBuilder2 buff;
            bool ok = true;

            void
            close()
            {
                const std::string data = buff.to_string();
                buff.clear();
                switch(state)
                {
                case ParserState::text:
                    if(!data.empty())
                    {
                        nodes->add_text(data);
                    }
                    break;
                case ParserState::image:
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
                case ParserState::text:
                    if(escape)
                    {
                        buff.add_char(c);
                        escape = false;
                    }
                    else
                    {
                        switch(c)
                        {
                        case '@':
                            close();
                            state = ParserState::image;
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
                        default: buff.add_char(c); break;
                        }
                    }
                    break;
                case ParserState::image:
                    if(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')
                       || c == '-' || c == '_')
                    {
                        buff.add_char(c);
                    }
                    else
                    {
                        close();
                        state = ParserState::text;
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
    }

    bool
    UiText::init_by_parsing_source(const std::string& str)
    {
        nodes.resize(0);
        Parser parser;
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

    UiText
    UiText::create_from_text(const std::string& str)
    {
        UiText text;
        text.init_with_text(str);
        return text;
    }

    void
    UiText::accept(textparser::Visitor* visitor)
    {
        for(auto& node: nodes)
        {
            node->accept(visitor);
        }
    }

    void
    UiText::accept(textparser::Visitor* visitor) const
    {
        for(const auto& node: nodes)
        {
            node->accept(visitor);
        }
    }

}
