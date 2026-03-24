#pragma once

#include "eustb_textedit.h"

#include <string>

struct TextEditState
{
    STB_TexteditState state;

    TextEditState();
    void focus(std::string* str);

    void onKeyLeft(bool shift, std::string* str);
    void onKeyRight(bool shift, std::string* str);
    void onKeyLineStart(bool shift, std::string* str);
    void onKeyLineEnd(bool shift, std::string* str);
    void onKeyBackspace(bool shift, std::string* str);
    void onKeyDelete(bool shift, std::string* str);
    void onKey(bool shift, int key, std::string* str);

    void onChar(char c, std::string* str);

    void click(std::string* str, float x, float y);
};
