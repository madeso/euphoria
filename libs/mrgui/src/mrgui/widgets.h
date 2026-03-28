#pragma once
#include <SDL_keycode.h>

#include "eustb_textedit_string.h"
#include "base/colors.h"
#include "base/rect.h"
#include "base/vec2.h"

namespace eu::render
{
    struct DrawableFont;
    struct Texture2d;
    struct SpriteBatch;
}

namespace eu::mrgui
{
    struct IdStack
    {
        [[nodiscard]] u32 get(const std::string& str) const;
        [[nodiscard]] u32 get(int id) const;

        void push(const std::string& str);
        void push(int id);

        void pop();

        [[nodiscard]] u32 last_id() const;

        std::vector<u32> stack;
    };

    struct KeyboardInput
    {
        SDL_Keycode key;
        u16 mod;
    };

    struct UiState
    {
        v2 mouse = { 0,0 };
        bool mousedown = false;
        std::optional<u32> hot_item = std::nullopt;
        std::optional<u32> active_item = std::nullopt;
        bool active_item_locked = false;

        std::optional<u32> kbd_item = std::nullopt;
        std::optional<u32> last_widget = std::nullopt;
        std::optional<KeyboardInput> key = std::nullopt; // todo(Gustav): change to a vector
        std::optional<char> keychar = std::nullopt; // todo(Gustav): change to a vector

        TextEditState text_state;

        void begin();

        void end();

        [[nodiscard]] bool is_active_free() const;
    };

    // Simple button IMGUI widget
    bool button_logic(u32 id, const Rect& rect, UiState& uistate);

    bool basic_button(u32 id, const Rect& rect, UiState& uistate, render::SpriteBatch* batch);

    bool button_texture(u32 id, const Rect& rect, UiState& uistate, render::SpriteBatch* batch, const render::Texture2d* texture, const Rect& active, const Rect& hot, const Rect& normal);

    bool slider(u32 id, float* val, const Rect& rect, UiState& uistate, render::SpriteBatch* batch);


    void draw_text(render::SpriteBatch* batch, render::DrawableFont* font, const std::string& str, float size, const v2& pos, const Rgb& color);

    bool textfield(u32 id, std::string* val, render::DrawableFont* font, float size, const v2& pos, UiState& uistate, render::SpriteBatch* batch);


}
