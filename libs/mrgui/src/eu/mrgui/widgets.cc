#include "eu/mrgui/widgets.h"

#include <SDL_timer.h>

#include "eu/render/canvas.h"
#include "eu/render/font.h"

namespace eu::mrgui
{


    /// djb2 initial hash
    constexpr u32 initial_hash = 5381;

    /// djb2 hash function
    u32 hash(unsigned char* str, std::size_t length, u32 seed)
    {
        u32 hash = seed;

        for (std::size_t index = 0; index < length; index += 1)
        {
            auto c = str[index];
            hash = ((hash << 5) + hash) + c;
        }

        return hash;
    }

    [[nodiscard]] u32 IdStack::get(const std::string& str) const
    {
        return hash(reinterpret_cast<unsigned char*>(const_cast<char*>(str.data())), str.size(), last_id());
    }

    [[nodiscard]] u32 IdStack::get(int id) const
    {
        return hash(reinterpret_cast<unsigned char*>(&id), sizeof(int), last_id());
    }
    void IdStack::push(const std::string& str)
    {
        stack.push_back(get(str));
    }
    void IdStack::push(int id)
    {
        stack.push_back(get(id));
    }
    void IdStack::pop()
    {
        stack.pop_back();
    }
    [[nodiscard]] u32 IdStack::last_id() const
    {
        return stack.empty() ? initial_hash : *stack.rbegin();
    }



    void UiState::begin()
    {
        hot_item = std::nullopt;
    }

    void UiState::end()
    {
        if (mousedown)
        {
            if (active_item == std::nullopt)
            {
                active_item = std::nullopt;
                active_item_locked = true;
            }
        }
        else
        {
            active_item = std::nullopt;
            active_item_locked = false;
        }

        if (key && key->key == SDLK_TAB)
        {
            kbd_item = std::nullopt;
        }
        key = std::nullopt;
        keychar = std::nullopt;
    }

    [[nodiscard]] bool UiState::is_active_free() const
    {
        return active_item_locked == false && active_item.has_value() == false;
    }

    // Simple button IMGUI widget
    bool button_logic(u32 id, const Rect& rect, UiState& uistate)
    {
        if (is_within(uistate.mouse, rect))
        {
            uistate.hot_item = id;
            if (uistate.is_active_free() && uistate.mousedown)
            {
                uistate.active_item = id;
            }
        }

        if (uistate.kbd_item == std::nullopt)
        {
            uistate.kbd_item = id;
        }

        // keyboard interaction
        if (uistate.kbd_item == id)
        {
            if (uistate.key.has_value())
            {
                const auto k = *uistate.key;
                uistate.key = std::nullopt;
                switch (k.key)
                {
                case SDLK_TAB:
                    uistate.kbd_item = std::nullopt;
                    if (k.mod & KMOD_SHIFT)
                        uistate.kbd_item = uistate.last_widget;
                    break;
                case SDLK_RETURN:
                    return true;
                }
            }
        }

        uistate.last_widget = id;

        // If button is hot and active, but mouse button is not
          // down, the user must have clicked the button.
        if (uistate.mousedown == false &&
            uistate.hot_item == id &&
            uistate.active_item == id)
        {
            return true;
        }

        // Otherwise, no clicky.
        return false;
    }

    bool basic_button(u32 id, const Rect& rect, UiState& uistate, render::SpriteBatch* batch)
    {
        const auto ret = button_logic(id, rect, uistate);

        if (uistate.kbd_item == id)
        {
            render::Quad{
                .tint = colors::red_vermillion
            }.draw(batch, rect.with_inset(Lrtb{ -4 }).with_offset({ 8, -8 }));
        }

        // shadow
        render::Quad{
            .tint = colors::black
        }.draw(batch, rect.with_offset({ 8.0f, -8.0f }));

        if (uistate.hot_item == id)
        {
            if (uistate.active_item == id)
            {
                // Button is both 'hot' and 'active'
                render::Quad{ .tint = colors::white }.draw(batch, rect.with_offset({ 2.0f, -2.0f }));
            }
            else
            {
                // Button is merely 'hot'
                render::Quad{ .tint = colors::white }.draw(batch, rect);
            }
        }
        else
        {
            // button is not hot, but it may be active    
            render::Quad{ .tint = colors::orange }.draw(batch, rect);
            // todo(Gustav): add grays
        }

        return ret;
    }

    bool button_texture(u32 id, const Rect& rect, UiState& uistate, render::SpriteBatch* batch, const render::Texture2d* texture, const Rect& active, const Rect& hot, const Rect& normal)
    {
        const auto ret = button_logic(id, rect, uistate);

        if (uistate.kbd_item == id)
        {
            // keep?
            render::Quad{
                .tint = colors::red_vermillion
            }.draw(batch, rect.with_inset(Lrtb{ -4 }).with_offset({ 8, -8 }));
        }

        if (uistate.hot_item == id)
        {
            if (uistate.active_item == id)
            {
                // Button is both 'hot' and 'active'
                render::Quad{ .texture = texture, .texturecoord = active }.draw(batch, rect);
            }
            else
            {
                // Button is merely 'hot'
                render::Quad{ .texture = texture, .texturecoord = hot }.draw(batch, rect);
            }
        }
        else
        {
            // button is not hot, but it may be active    
            render::Quad{ .texture = texture, .texturecoord = normal }.draw(batch, rect);
            // todo(Gustav): add grays
        }

        return ret;
    }

    bool slider(u32 id, float* val, const Rect& rect, UiState& uistate, render::SpriteBatch* batch)
    {
        const auto grab_size = 16.0f;
        const auto inner = rect.with_inset(Lrtb{ 3.0f });
        const auto avail = inner.get_size().x - grab_size;
        const auto pos = avail * *val;

        const auto grabber_rect = Rect::from_size({ grab_size, inner.get_size().y }).with_bottom_left_at({ inner.left + pos, inner.bottom });

        if (is_within(uistate.mouse, inner))
        {
            uistate.hot_item = id;
            if (uistate.is_active_free() && uistate.mousedown)
            {
                uistate.active_item = id;
            }
        }

        if (uistate.kbd_item == std::nullopt)
        {
            uistate.kbd_item = id;
        }

        if (uistate.kbd_item == id)
        {
            render::Quad{ .tint = colors::red_vermillion }.draw(batch, rect.with_inset(Lrtb{ -6 }));
        }

        render::Quad{ .tint = colors::orange }.draw(batch, rect);
        render::Quad{ .tint = uistate.active_item == id && uistate.hot_item == id ? colors::white : colors::green_bluish }.draw(batch, grabber_rect);
        if (uistate.kbd_item == id)
        {
            if (uistate.key.has_value())
            {
                const auto k = *uistate.key;
                uistate.key = std::nullopt;
                constexpr float change = 0.1f;

                switch (k.key)
                {
                case SDLK_TAB:
                    uistate.kbd_item = std::nullopt;
                    if (k.mod & KMOD_SHIFT)
                    {
                        uistate.kbd_item = uistate.last_widget;
                    }
                    break;
                case SDLK_LEFT:
                    if (*val > 0)
                    {
                        *val = std::max(0.0f, *val - change);
                        return true;
                    }
                    break;
                case SDLK_RIGHT:
                    if (*val < 1)
                    {
                        *val = std::min(1.0f, *val + change);
                        return true;
                    }
                    break;
                }
            }
        }

        uistate.last_widget = id;

        if (uistate.active_item == id)
        {
            const auto new_range = to_01(inner, uistate.mouse);
            const auto new_value = keep_within01(new_range.x);
            if (*val != new_value)
            {
                *val = new_value;
                return true;
            }
        }

        return false;
    }


    void draw_text(render::SpriteBatch* batch, render::DrawableFont* font, const std::string& str, float size, const v2& pos, const Rgb& color)
    {
        render::DrawableText text{ font };
        text.set_text(str);
        text.set_size(size);
        text.compile();
        text.draw(batch, pos, color, color);
    }

    bool textfield(u32 id, std::string* val, render::DrawableFont* font, float size, const v2& pos, UiState& uistate, render::SpriteBatch* batch)
    {
        using namespace std;
        bool changed = false;

        auto& text_state = uistate.text_state;

        // Layout
        render::DrawableText text{ font };
        text.set_text(*val);
        text.set_size(size);
        text.compile();
        const auto rect = text.get_extents().with_offset(pos).with_inset(Lrtb{ -6 });

        if (is_within(uistate.mouse, rect)) {
            uistate.hot_item = id;
            if (uistate.active_item == std::nullopt && uistate.mousedown)
            {
                uistate.active_item = id;
                text_state.focus(val);
            }
        }
        if (uistate.kbd_item == std::nullopt)
            uistate.kbd_item = id;

        if (uistate.kbd_item == id) {
            render::Quad{ .tint = colors::red_vermillion }.draw(batch, rect.with_inset(Lrtb{ -6 }));
        }
        if (uistate.active_item == id || uistate.hot_item == id) {
            render::Quad{ .tint = colors::white }.draw(batch, rect.with_inset(Lrtb{ -6 }));
        }
        else {
            render::Quad{ .tint = colors::green_bluish }.draw(batch, rect.with_inset(Lrtb{ -6 }));
        }

        // Handle input
        if (uistate.kbd_item == id) {
            if (uistate.key.has_value()) {
                auto k = uistate.key.value();
                const auto shift = k.mod & KMOD_SHIFT;
                switch (k.key) {
                case SDLK_LEFT: text_state.onKeyLeft(shift, val); changed = true; break;
                case SDLK_RIGHT: text_state.onKeyRight(shift, val); changed = true; break;
                case SDLK_HOME: text_state.onKeyLineStart(shift, val); changed = true; break;
                case SDLK_END: text_state.onKeyLineEnd(shift, val); changed = true; break;
                case SDLK_BACKSPACE: text_state.onKeyBackspace(shift, val); changed = true; break;
                case SDLK_DELETE: text_state.onKeyDelete(shift, val); changed = true; break;
                case SDLK_RETURN: // ignore
                case SDLK_TAB:
                    uistate.kbd_item = std::nullopt;
                    if (shift)
                        uistate.kbd_item = uistate.last_widget;
                    uistate.key = std::nullopt;
                    break;
                default: break;
                }
            }
            if (uistate.keychar.has_value() && *uistate.keychar >= 32 && *uistate.keychar < 127) {
                char c = *uistate.keychar;
                text_state.onChar(c, val);
                changed = true;
            }
        }

        // Mouse click to set cursor
        if (uistate.mousedown == false && uistate.hot_item == id && uistate.active_item == id) {
            // Map mouse x to character index
            text_state.click(val, uistate.mouse.x - rect.left, uistate.mouse.y - rect.top);
            uistate.kbd_item = id;
        }

        uistate.last_widget = id;

        static int last_cursor = -10;
        const auto current_cursor = text_state.state.cursor;
        if (last_cursor != current_cursor)
        {
            LOG_INFO("Cursor position: {}", current_cursor);
            last_cursor = current_cursor;
        }

        // Draw text
        text.draw(batch, pos, colors::black, colors::black);

        // Draw cursor
        if (uistate.kbd_item == id) {
            const auto is_cursor_blink_visible = (SDL_GetTicks() >> 8) & 1;
            if (is_cursor_blink_visible) {
                // Compute cursor x
                float cursor_x = rect.left;
                if (text_state.state.cursor > 0 && text_state.state.cursor <= (int)val->size()) {
                    std::string substr = val->substr(0, text_state.state.cursor);
                    render::DrawableText t{ font };
                    t.set_text(substr);
                    t.set_size(size);
                    t.compile();
                    cursor_x = rect.left + t.get_extents().get_size().x;
                }
                const auto ascent = font->ascent * size;
                const auto descent = font->descent * size;
                render::Quad{ .tint = colors::black }.draw(batch, Rect::from_size({ 1, ascent + -descent }).with_bottom_left_at({ cursor_x, pos.y + descent }));
            }
        }

        return changed;
    }


}
