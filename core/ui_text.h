#ifndef CORE_TEXTPARSER_H
#define CORE_TEXTPARSER_H

#include <string>
#include <memory>
#include <vector>
#include <sstream>

namespace euphoria::core
{
    struct UiText;

    namespace textparser
    {
        struct Visitor;

        struct Node
        {
            virtual ~Node() = default;

            virtual void
            Visit(Visitor* visitor) const = 0;
        };

        // todo: move nodes to private

        struct TextNode : public Node
        {
            std::string text;

            explicit TextNode(const std::string& t);

            void
            Visit(Visitor* visitor) const override;
        };

        struct ImageNode : public Node
        {
            std::string image;

            explicit ImageNode(const std::string& t);

            void
            Visit(Visitor* visitor) const override;
        };

        struct BeginEndNode : public Node
        {
            bool begin;

            explicit BeginEndNode(bool b);

            void
            Visit(Visitor* visitor) const override;
        };

        struct Visitor
        {
            virtual ~Visitor() = default;

            virtual void
            OnText(const std::string& text)
                    = 0;

            virtual void
            OnImage(const std::string& image)
                    = 0;

            virtual void
            OnBegin()
                    = 0;

            virtual void
            OnEnd() = 0;
        };

        struct VisitorDebugString : public Visitor
        {
            std::ostringstream ss;

            void
            OnText(const std::string& text) override;

            void
            OnImage(const std::string& img) override;

            void
            OnBegin() override;

            void
            OnEnd() override;

            static std::string
            Visit(UiText* visitor);
        };
    }  // namespace textparser

    /** Represents displayed text.
     * Format: @image with text and {-begin and }-end markers \ escapes
     */
    // todo: add ability to specify button/keys so that we can use:
    // use "[jump] to jump" and said [jump] would look up the keybind and display a
    // sprite/ninepath w/ text representing the bind this could also be useful when
    // displaying all the keybinds. needs to work out how (or if) the joystick
    // id/number also should be displayed.
    struct UiText
    {
        void
        Clear();

        void
        AddText(const std::string& str);

        void
        AddImage(const std::string& img);

        void
        AddBegin();

        void
        AddEnd();

        void
        CreateText(const std::string& str);

        bool
        CreateParse(const std::string& str);

        [[nodiscard]] static UiText
        FromText(const std::string& str);

        void
        Visit(textparser::Visitor* visitor);

        void
        Visit(textparser::Visitor* visitor) const;

        std::vector<std::shared_ptr<textparser::Node>> nodes;
    };

}  // namespace euphoria::core

#endif  // SPACETYPER_TEXTPARSER_H
