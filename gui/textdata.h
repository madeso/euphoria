#ifndef GUI_TEXTDATA_H
#define GUI_TEXTDATA_H

#include <memory>
#include <string>


namespace euphoria::render
{
    class Font;
    class Text;
}  // namespace euphoria::render

namespace euphoria::gui
{
    class TextData
    {
    public:
        TextData();
        ~TextData();

        void
        SetFont(std::shared_ptr<render::Font> font);

        const render::Font&
        GetFont() const;

        void
        SetString(const std::string& str);

        const std::string&
        GetString() const;

        bool
        HasText() const;

        const render::Text&
        GetText() const;

        render::Text&
        GetText();

        void
        SetSize(float size);

    private:
        void
                                      UpdateText();
        std::shared_ptr<render::Font> font_;
        std::string                   string_;
        float                         size;

        std::shared_ptr<render::Text> text_;
    };
}  // namespace euphoria::gui

#endif  // GUI_TEXTDATA_H
