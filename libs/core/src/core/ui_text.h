#pragma once

#include <memory>

#include "base/stringbuilder.h"


namespace eu::core
{
    struct UiText;

    namespace textparser
    {
        struct Visitor;

        struct Node
        {
            Node() = default;
            virtual ~Node() = default;

            Node(const Node& other) = delete;
            void operator=(const Node&) = delete;
            Node(Node&& other) = delete;
            void operator=(Node&&) = delete;

            virtual void
            accept(Visitor* visitor) const = 0;
        };

        // todo(Gustav): move nodes to private

        struct NodeText : Node
        {
            std::string text;

            explicit NodeText(const std::string& t);

            void
            accept(Visitor* visitor) const override;
        };

        struct NodeImage : Node
        {
            std::string image;

            explicit NodeImage(const std::string& t);

            void
            accept(Visitor* visitor) const override;
        };

        struct NodeBeginOrEnd : Node
        {
            bool begin;

            explicit NodeBeginOrEnd(bool b);

            void
            accept(Visitor* visitor) const override;
        };

        struct Visitor
        {
            Visitor() = default;
            virtual ~Visitor() = default;

            Visitor(const Visitor& other) = delete;
            void operator=(const Visitor&) = delete;
            Visitor(Visitor&& other) = delete;
            void operator=(Visitor&&) = delete;

            virtual void
            on_text(const std::string& text) = 0;

            virtual void
            on_image(const std::string& image) = 0;

            virtual void
            on_begin() = 0;

            virtual void
            on_end() = 0;
        };

        struct VisitorDebugString : public Visitor
        {
            StringBuilder ss;

            static std::string accept_all_nodes(UiText* visitor);

            void on_text(const std::string& text) override;
            void on_image(const std::string& img) override;
            void on_begin() override;
            void on_end() override;
        };
    }

    /** Represents displayed text.
     * Format: @code @image with text and {-begin and }-end markers \ escapes @endcode
     */
    // todo(Gustav): add ability to specify button/keys so that we can use:
    // use "[jump] to jump" and said [jump] would look up the keybind and display a
    // sprite/ninepath w/ text representing the bind this could also be useful when
    // displaying all the keybinds. needs to work out how (or if) the joystick
    // id/number also should be displayed.
    struct UiText
    {
        std::vector<std::shared_ptr<textparser::Node>> nodes;

        [[nodiscard]] static UiText create_from_text(const std::string& str);

        void clear();

        void add_text(const std::string& str);
        void add_image(const std::string& img);
        void add_begin();
        void add_end();

        void init_with_text(const std::string& str);
        bool init_by_parsing_source(const std::string& str);

        void accept(textparser::Visitor* visitor);
        void accept(textparser::Visitor* visitor) const;
    };

}
