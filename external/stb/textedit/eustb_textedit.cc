#include <string>

#include "eustb_textedit_string.h"




// ========================================================================================================
//  User-defined types

#ifndef STB_TEXTEDIT_KEYTYPE
#define STB_TEXTEDIT_KEYTYPE int
#endif

// the type of object representing a string being edited, typically this is a wrapper object with other data you need
#define STB_TEXTEDIT_STRING            TextEditString
struct TextEditString
{
    std::string* str;
};




// ========================================================================================================
//   User-implemented functions

// the length of the string (ideally O(1))
int STB_TEXTEDIT_STRINGLEN(STB_TEXTEDIT_STRING* obj)
{
    return static_cast<int>(obj->str->size());
}

// returns the results of laying out a line of characters starting from character #n
inline void STB_TEXTEDIT_LAYOUTROW(StbTexteditRow* row, TextEditString* obj, int n) {
    row->x0 = 0;
    row->x1 = (float)obj->str->size();
    row->baseline_y_delta = 1.0f;
    row->ymin = 0.0f;
    row->ymax = 1.0f;
    row->num_chars = static_cast<int>(obj->str->size());
}

// returns the pixel delta from the xpos of the i'th character to the xpos of the i+1'th char for a line of characters starting at character #n (i.e. accounts for kerning with previous char)
inline int STB_TEXTEDIT_GETWIDTH(STB_TEXTEDIT_STRING* obj, int n, int i)
{
    return 1.0f;
}

// maps a keyboard input to an insertable character (return type is int, -1 means not valid to insert)
int STB_TEXTEDIT_KEYTOTEXT(STB_TEXTEDIT_KEYTYPE k)
{
    return ((k) >= 32 && (k) < 127 ? (k) : -1);
}

// returns the i'th character of obj, 0-based
STB_TEXTEDIT_CHARTYPE STB_TEXTEDIT_GETCHAR(STB_TEXTEDIT_STRING* obj, int i)
{
    return (obj)->str->at(i);
}

// the character returned by _GETCHAR() we recognize as manually wordwrapping for end-of-line positioning
#define STB_TEXTEDIT_NEWLINE           '\n'

// delete n characters starting at i
void STB_TEXTEDIT_DELETECHARS(STB_TEXTEDIT_STRING* obj, int i, int n)
{
    (obj)->str->erase(i, n);
}

// insert n characters at i (pointed to by STB_TEXTEDIT_CHARTYPE*)
bool STB_TEXTEDIT_INSERTCHARS(TextEditString* obj, int i, STB_TEXTEDIT_CHARTYPE* cptr, int n)
{
    (obj)->str->insert(i, reinterpret_cast<char*>(cptr), n);
    return true;
}




// ========================================================================================================
// Key definitions

#define STB_TEXTEDIT_K_LEFT         0x200000 // keyboard input to move cursor left
#define STB_TEXTEDIT_K_RIGHT        0x200001 // keyboard input to move cursor right
#define STB_TEXTEDIT_K_UP           0x200002 // keyboard input to move cursor up
#define STB_TEXTEDIT_K_DOWN         0x200003 // keyboard input to move cursor down
#define STB_TEXTEDIT_K_LINESTART    0x200004 // keyboard input to move cursor to start of line
#define STB_TEXTEDIT_K_LINEEND      0x200005 // keyboard input to move cursor to end of line
#define STB_TEXTEDIT_K_TEXTSTART    0x200006 // keyboard input to move cursor to start of text
#define STB_TEXTEDIT_K_TEXTEND      0x200007 // keyboard input to move cursor to end of text
#define STB_TEXTEDIT_K_DELETE       0x200008 // keyboard input to delete selection or character under cursor
#define STB_TEXTEDIT_K_BACKSPACE    0x200009 // keyboard input to delete selection or character left of cursor
#define STB_TEXTEDIT_K_UNDO         0x20000A // keyboard input to perform undo
#define STB_TEXTEDIT_K_REDO         0x20000B // keyboard input to perform redo
#define STB_TEXTEDIT_K_WORDLEFT     0x20000C // keyboard input to move cursor left one word
#define STB_TEXTEDIT_K_WORDRIGHT    0x20000D // keyboard input to move cursor right one word
#define STB_TEXTEDIT_K_PGUP         0x20000E // keyboard input to move cursor up a page
#define STB_TEXTEDIT_K_PGDOWN       0x20000F // keyboard input to move cursor down a page
#define STB_TEXTEDIT_K_SHIFT        0x400000




// ========================================================================================================
// stb implementation

#define STB_TEXTEDIT_IMPLEMENTATION
#include "eustb_textedit.h"




// ========================================================================================================
// TextEditState implementation

TextEditState::TextEditState()
{
    stb_textedit_initialize_state(&state, 1);
}

void TextEditState::focus(std::string* str)
{
    // todo(Gustav): select all instead?
    // state.cursor = (int)str.size();
    // state.select_start = state.select_end = state.cursor;
    onKey(false, STB_TEXTEDIT_K_LINEEND, str);
}

void TextEditState::onKeyLeft(bool shift, std::string* str)
{
    onKey(shift, STB_TEXTEDIT_K_LEFT, str);
}
void TextEditState::onKeyRight(bool shift, std::string* str)
{
    onKey(shift, STB_TEXTEDIT_K_RIGHT, str);
}
void TextEditState::onKeyLineStart(bool shift, std::string* str)
{
    onKey(shift, STB_TEXTEDIT_K_LINESTART, str);
}
void TextEditState::onKeyLineEnd(bool shift, std::string* str)
{
    onKey(shift, STB_TEXTEDIT_K_LINEEND, str);
}
void TextEditState::onKeyBackspace(bool shift, std::string* str)
{
    onKey(shift, STB_TEXTEDIT_K_BACKSPACE, str);
}
void TextEditState::onKeyDelete(bool shift, std::string* str)
{
    onKey(shift, STB_TEXTEDIT_K_DELETE, str);
}


void TextEditState::onKey(bool shift, int key, std::string* str)
{
    TextEditString wrapper{ str };
    stb_textedit_key(&wrapper, &state, key | (shift ? STB_TEXTEDIT_K_SHIFT : 0));
}

void TextEditState::onChar(char c, std::string* str)
{
    TextEditString wrapper{ str };
    stb_textedit_key(&wrapper, &state, c);
}

void TextEditState::click(std::string* str, float x, float y)
{
    TextEditString wrapper{ str };
    stb_textedit_click(&wrapper, &state, x, y);
}
