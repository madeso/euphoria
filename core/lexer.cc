#include "core/lexer.h"
#include "core/assert.h"

namespace euphoria::core::lexer
{
    struct FileString : public File
    {
        std::string text;
        std::size_t next_position = 0;

        bool
        HasMore() const override
        {
            return next_position < text.size();
        }

        char
        Peek(std::size_t advance) override
        {
            ASSERT(advance>0);
            const auto index = next_position + advance-1;

            if(index >= text.size()) return 0;
            else return text[index];
        }

        char
        Read() override
        {
            if(!HasMore())
            {
                return 0;
            }

            const auto r = text[next_position];
            next_position += 1;
            return r;
        }
    };

    std::shared_ptr<File>
    FromString(const std::string& str)
    {
        auto file = std::make_shared<FileString>();
        file->text = str;
        return file;
    }

    namespace detail
    {
        std::shared_ptr<Node>
        Children::Find(char c)
        {
            auto found_node = nodes.find(c);
            if(found_node == nodes.end())
            {
                return nullptr;
            }
            else
            {
                return found_node->second;
            }
        }

        std::shared_ptr<Node>
        Children::GetOrCreate(char c)
        {
            auto found_node = nodes.find(c);
            if(found_node == nodes.end())
            {
                auto new_node = std::make_shared<Node>();
                nodes.insert(std::make_pair(c, new_node));
                return new_node;
            }
            else
            {
                return found_node->second;
            }
        }

        bool
        Node::IsTerminal() const
        {
            const auto is_terminal = token.has_value() || custom != nullptr;

            // sanity chech:
            // if it is a terminal, the debug name must be non-empty
            ASSERTX(!is_terminal || !debug_name.empty(), is_terminal, debug_name);

            return is_terminal;
        }
    }

    namespace
    {
        std::shared_ptr<detail::Node>
        GetNodeForAddition(detail::Children* root, const std::string& token_text, const std::string& debug_name)
        {
            ASSERT(root);
            ASSERT(!token_text.empty());

            detail::Children* children = root;

            std::shared_ptr<detail::Node> token_node;
            for(auto c: token_text)
            {
                token_node = children->GetOrCreate(c);
                children = &token_node->children;
            }

            // this should never happen as we assert for empty tokens
            // and GetOrCreate should always return a valid node
            ASSERT(token_node);

            ASSERTX(!token_node->IsTerminal(), debug_name, token_node->debug_name);

            return token_node;
        }
    }

    Lexer&
    Lexer::AddToken(const std::string& token, int token_type)
    {
        auto token_node = GetNodeForAddition(&children, token, token);
        token_node->token = token_type;
        return *this;
    }
    
    Lexer&
    Lexer::AddToken(const std::string& debug_name, const std::string& token, CustomToken custom_token)
    {
        auto token_node = GetNodeForAddition(&children, token, debug_name);
        token_node->custom = std::make_shared<CustomToken>(custom_token);
        return *this;
    }

    void
    Lexer::Parse
        (
            std::shared_ptr<File> file,
            std::function<void(const Token&)> on_token,
            std::function<void(const Error&)> on_error
        ) const
    {
        // todo(Gustav): implementation needed
    }
}
