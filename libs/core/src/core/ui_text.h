#pragma once


#include <string>
#include <memory>
#include <vector>
#include <sstream>

#include "core/noncopyable.h"

namespace euphoria::core
{
    struct ui_text;

    namespace textparser
    {
        struct visitor;

        struct node
        {
            node() = default;
            virtual ~node() = default;

            NONCOPYABLE(node);

            virtual void
            accept(visitor* visitor) const = 0;
        };

        // todo(Gustav): move nodes to private

        struct node_text : node
        {
            std::string text;

            explicit node_text(const std::string& t);

            void
            accept(visitor* visitor) const override;
        };

        struct node_image : node
        {
            std::string image;

            explicit node_image(const std::string& t);

            void
            accept(visitor* visitor) const override;
        };

        struct node_begin_or_end : node
        {
            bool begin;

            explicit node_begin_or_end(bool b);

            void
            accept(visitor* visitor) const override;
        };

        struct visitor
        {
            visitor() = default;
            virtual ~visitor() = default;

            NONCOPYABLE(visitor);

            virtual void
            on_text(const std::string& text) = 0;

            virtual void
            on_image(const std::string& image) = 0;

            virtual void
            on_begin() = 0;

            virtual void
            on_end() = 0;
        };

        struct visitor_debug_string : public visitor
        {
            std::ostringstream ss;

            void
            on_text(const std::string& text) override;

            void
            on_image(const std::string& img) override;

            void
            on_begin() override;

            void
            on_end() override;

            static std::string
            accept_all_nodes(ui_text* visitor);
        };
    } // namespace textparser

    /** Represents displayed text.
     * Format: @code @image with text and {-begin and }-end markers \ escapes @endcode
     */
    // todo(Gustav): add ability to specify button/keys so that we can use:
    // use "[jump] to jump" and said [jump] would look up the keybind and display a
    // sprite/ninepath w/ text representing the bind this could also be useful when
    // displaying all the keybinds. needs to work out how (or if) the joystick
    // id/number also should be displayed.
    struct ui_text
    {
        void
        clear();

        void
        add_text(const std::string& str);

        void
        add_image(const std::string& img);

        void
        add_begin();

        void
        add_end();

        void
        init_with_text(const std::string& str);

        bool
        init_by_parsing_source(const std::string& str);

        [[nodiscard]] static ui_text
        create_from_text(const std::string& str);

        void
        accept(textparser::visitor* visitor);

        void
        accept(textparser::visitor* visitor) const;

        std::vector<std::shared_ptr<textparser::node>> nodes;
    };

}
