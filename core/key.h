// Euphoria - Copyright (c) Gustav

#ifndef EUPHORIA_INPUT_KEY_H_
#define EUPHORIA_INPUT_KEY_H_

#include <string>


/** The mouse button
 */
enum class MouseButton
{
  INVALID  /// Invalid key
  ,
  UNBOUND  /// No key
  ,
  LEFT  /// The left mouse button
  ,
  MIDDLE  /// The middle mouse button
  ,
  RIGHT  /// The right mouse button
  ,
  X1  /// The X2 mouse button
  ,
  X2  /// The X2 mouse button
};

/** Generate a string representation of the given button
@param k the key
@returns the string representation
 */
std::string
ToString(MouseButton k);

/** Generate a key from a string.
@param keyname the name of the button
@returns the Button or None
 */
MouseButton
ToMouseButton(const std::string& keyname);

/** A key code
 */
enum class Key
{
  INVALID  /// Invalid key
  ,
  UNBOUND  /// No key
  ,
  RETURN  /// The key for Return
  ,
  ESCAPE  /// The key for Escape
  ,
  BACKSPACE  /// The key for Backspace
  ,
  TAB  /// The key for Tab
  ,
  SPACE  /// The key for Space
  ,
  EXCLAIM  /// The key for Exclaim
  ,
  QUOTEDBL  /// The key for Quotedbl
  ,
  HASH  /// The key for Hash
  ,
  PERCENT  /// The key for Percent
  ,
  DOLLAR  /// The key for Dollar
  ,
  AMPERSAND  /// The key for Ampersand
  ,
  QUOTE  /// The key for Quote
  ,
  LEFT_PAREN  /// The key for Leftparen
  ,
  RIGHT_PAREN  /// The key for Rightparen
  ,
  ASTERIX  /// The key for Asterisk
  ,
  PLUS  /// The key for Plus
  ,
  COMMA  /// The key for Comma
  ,
  MINUS  /// The key for Minus
  ,
  PERIOD  /// The key for Period
  ,
  SLASH  /// The key for Slash
  ,
  NUM_0  /// The key for Num0
  ,
  NUM_1  /// The key for Num1
  ,
  NUM_2  /// The key for Num2
  ,
  NUM_3  /// The key for Num3
  ,
  NUM_4  /// The key for Num4
  ,
  NUM_5  /// The key for Num5
  ,
  NUM_6  /// The key for Num6
  ,
  NUM_7  /// The key for Num7
  ,
  NUM_8  /// The key for Num8
  ,
  NUM_9  /// The key for Num9
  ,
  COLON  /// The key for Colon
  ,
  SEMICOLON  /// The key for Semicolon
  ,
  LESS  /// The key for Less
  ,
  EQUALS  /// The key for Equals
  ,
  GREATER  /// The key for Greater
  ,
  QUESTION  /// The key for Question
  ,
  AT  /// The key for At
  ,
  LEFT_BRACKET  /// The key for Leftbracket
  ,
  BACKSLASH  /// The key for Backslash
  ,
  RIGHT_BRACKET  /// The key for Rightbracket
  ,
  CARET  /// The key for Caret
  ,
  UNDERSCORE  /// The key for Underscore
  ,
  BACKQUOTE  /// The key for Backquote
  ,
  A  /// The key for A
  ,
  B  /// The key for B
  ,
  C  /// The key for C
  ,
  D  /// The key for D
  ,
  E  /// The key for E
  ,
  F  /// The key for F
  ,
  G  /// The key for G
  ,
  H  /// The key for H
  ,
  I  /// The key for I
  ,
  J  /// The key for J
  ,
  K  /// The key for K
  ,
  L  /// The key for L
  ,
  M  /// The key for M
  ,
  N  /// The key for N
  ,
  O  /// The key for O
  ,
  P  /// The key for P
  ,
  Q  /// The key for Q
  ,
  R  /// The key for R
  ,
  S  /// The key for S
  ,
  T  /// The key for T
  ,
  U  /// The key for U
  ,
  V  /// The key for V
  ,
  W  /// The key for W
  ,
  X  /// The key for X
  ,
  Y  /// The key for Y
  ,
  Z  /// The key for Z
  ,
  CAPSLOCK  /// The key for Capslock
  ,
  F1  /// The key for F1
  ,
  F2  /// The key for F2
  ,
  F3  /// The key for F3
  ,
  F4  /// The key for F4
  ,
  F5  /// The key for F5
  ,
  F6  /// The key for F6
  ,
  F7  /// The key for F7
  ,
  F8  /// The key for F8
  ,
  F9  /// The key for F9
  ,
  F10  /// The key for F10
  ,
  F11  /// The key for F11
  ,
  F12  /// The key for F12
  ,
  PRINTSCREEN  /// The key for Printscreen
  ,
  SCROLLLOCK  /// The key for Scrolllock
  ,
  PAUSE  /// The key for Pause
  ,
  INSERT  /// The key for Insert
  ,
  HOME  /// The key for Home
  ,
  PAGEUP  /// The key for Pageup
  ,
  DELETE  /// The key for Delete
  ,
  END  /// The key for End
  ,
  PAGEDOWN  /// The key for Pagedown
  ,
  RIGHT  /// The key for Right
  ,
  LEFT  /// The key for Left
  ,
  DOWN  /// The key for Down
  ,
  UP  /// The key for Up
  ,
  NUMLOCK_CLEAR  /// The key for Numlockclear
  ,
  KEYPAD_DIVIDE  /// The key for Kp_Divide
  ,
  KEYPAD_MULTIPLY  /// The key for Kp_Multiply
  ,
  KEYPAD_MINUS  /// The key for Kp_Minus
  ,
  KEYPAD_PLUS  /// The key for Kp_Plus
  ,
  KEYPAD_ENTER  /// The key for Kp_Enter
  ,
  KEYPAD_1  /// The key for Kp_1
  ,
  KEYPAD_2  /// The key for Kp_2
  ,
  KEYPAD_3  /// The key for Kp_3
  ,
  KEYPAD_4  /// The key for Kp_4
  ,
  KEYPAD_5  /// The key for Kp_5
  ,
  KEYPAD_6  /// The key for Kp_6
  ,
  KEYPAD_7  /// The key for Kp_7
  ,
  KEYPAD_8  /// The key for Kp_8
  ,
  KEYPAD_9  /// The key for Kp_9
  ,
  KEYPAD_0  /// The key for Kp_0
  ,
  KEYPAD_PERIOD  /// The key for Kp_Period
  ,
  APPLICATION  /// The key for Application
  ,
  POWER  /// The key for Power
  ,
  KEYPAD_EQUALS  /// The key for Kp_Equals
  ,
  F13  /// The key for F13
  ,
  F14  /// The key for F14
  ,
  F15  /// The key for F15
  ,
  F16  /// The key for F16
  ,
  F17  /// The key for F17
  ,
  F18  /// The key for F18
  ,
  F19  /// The key for F19
  ,
  F20  /// The key for F20
  ,
  F21  /// The key for F21
  ,
  F22  /// The key for F22
  ,
  F23  /// The key for F23
  ,
  F24  /// The key for F24
  ,
  EXECUTE  /// The key for Execute
  ,
  HELP  /// The key for Help
  ,
  MENU  /// The key for Menu
  ,
  SELECT  /// The key for Select
  ,
  STOP  /// The key for Stop
  ,
  AGAIN  /// The key for Again
  ,
  UNDO  /// The key for Undo
  ,
  CUT  /// The key for Cut
  ,
  COPY  /// The key for Copy
  ,
  PASTE  /// The key for Paste
  ,
  FIND  /// The key for Find
  ,
  MUTE  /// The key for Mute
  ,
  VOLUME_UP  /// The key for Volumeup
  ,
  VOLUME_DOWN  /// The key for Volumedown
  ,
  KEYPAD_COMMA  /// The key for Kp_Comma
  ,
  KEYPAD_EQUALS_AS_400  /// The key for Kp_Equalsas400
  ,
  ALT_ERASE  /// The key for Alterase
  ,
  SYSREQ  /// The key for Sysreq
  ,
  CANCEL  /// The key for Cancel
  ,
  CLEAR  /// The key for Clear
  ,
  PRIOR  /// The key for Prior
  ,
  SECOND_RETURN  /// The key for Return2
  ,
  SEPARATOR  /// The key for Separator
  ,
  OUT  /// The key for Out
  ,
  OPER  /// The key for Oper
  ,
  CLEAR_AGAIN  /// The key for Clearagain
  ,
  CRSEL  /// The key for Crsel
  ,
  EXSEL  /// The key for Exsel
  ,
  KEYPAD_00  /// The key for Kp_00
  ,
  KEYPAD_000  /// The key for Kp_000
  ,
  THOUSANDSEPARATOR  /// The key for Thousandsseparator
  ,
  DECIMALSEPARATOR  /// The key for Decimalseparator
  ,
  CURRENCY_UNIT  /// The key for Currencyunit
  ,
  CURRENCY_SUBUNIT  /// The key for Currencysubunit
  ,
  KEYPAD_LEFTPAREN  /// The key for Kp_Leftparen
  ,
  KEYPAD_RIGHTPAREN  /// The key for Kp_Rightparen
  ,
  KEYPAD_LEFTBRACE  /// The key for Kp_Leftbrace
  ,
  KEYPAD_RIGHTBRACE  /// The key for Kp_Rightbrace
  ,
  KEYPAD_TAB  /// The key for Kp_Tab
  ,
  KEYPAD_BACKSPACE  /// The key for Kp_Backspace
  ,
  KEYPAD_A  /// The key for Kp_A
  ,
  KEYPAD_B  /// The key for Kp_B
  ,
  KEYPAD_C  /// The key for Kp_C
  ,
  KEYPAD_D  /// The key for Kp_D
  ,
  KEYPAD_E  /// The key for Kp_E
  ,
  KEYPAD_F  /// The key for Kp_F
  ,
  KEYPAD_XOR  /// The key for Kp_Xor
  ,
  KEYPAD_POWER  /// The key for Kp_Power
  ,
  KEYPAD_PERCENT  /// The key for Kp_Percent
  ,
  KEYPAD_LESS  /// The key for Kp_Less
  ,
  KEYPAD_GREATER  /// The key for Kp_Greater
  ,
  KEYPAD_AMPERSAND  /// The key for Kp_Ampersand
  ,
  KEYPAD_DOUBLE_AMPERSAND  /// The key for Kp_Dblampersand
  ,
  KEYPAD_VERTICAL_BAR  /// The key for Kp_Verticalbar
  ,
  KEYPAD_DOUBLE_VERTICLE_BAR  /// The key for Kp_Dblverticalbar
  ,
  KEYPAD_COLON  /// The key for Kp_Colon
  ,
  KEYPAD_HASH  /// The key for Kp_Hash
  ,
  KEYPAD_SPACE  /// The key for Kp_Space
  ,
  KEYPAD_AT  /// The key for Kp_At
  ,
  KEYPAD_EXCLAM  /// The key for Kp_Exclam
  ,
  KEYPAD_MEM_STORE  /// The key for Kp_Memstore
  ,
  KEYPAD_MEM_RECALL  /// The key for Kp_Memrecall
  ,
  KEYPAD_MEM_CLEAR  /// The key for Kp_Memclear
  ,
  KEYPAD_MEM_ADD  /// The key for Kp_Memadd
  ,
  KEYPAD_MEM_SUBTRACT  /// The key for Kp_Memsubtract
  ,
  KEYPAD_MEM_MULTIPLY  /// The key for Kp_Memmultiply
  ,
  KEYPAD_MEM_DIVIDE  /// The key for Kp_Memdivide
  ,
  KEYPAD_PLUS_MINUS  /// The key for Kp_Plusminus
  ,
  KEYPAD_CLEAR  /// The key for Kp_Clear
  ,
  KEYPAD_CLEAR_ENTRY  /// The key for Kp_Clearentry
  ,
  KEYPAD_BINARY  /// The key for Kp_Binary
  ,
  KEYPAD_OCTAL  /// The key for Kp_Octal
  ,
  KEYPAD_DECIMAL  /// The key for Kp_Decimal
  ,
  KEYPAD_HEXADECIMAL  /// The key for Kp_Hexadecimal
  ,
  CTRL_LEFT  /// The key for Lctrl
  ,
  SHIFT_LEFT  /// The key for Lshift
  ,
  ALT_LEFT  /// The key for Lalt
  ,
  GUI_LEFT  /// The key for Lgui
  ,
  CTRL_RIGHT  /// The key for Rctrl
  ,
  SHIFT_RIGHT  /// The key for Rshift
  ,
  ALT_RIGHT  /// The key for Ralt
  ,
  GUI_RIGHT  /// The key for Rgui
  ,
  MODE  /// The key for Mode
  ,
  AUDIO_NEXT  /// The key for Audionext
  ,
  AUDIO_PREV  /// The key for Audioprev
  ,
  AUDIO_STOP  /// The key for Audiostop
  ,
  AUDIO_PLAY  /// The key for Audioplay
  ,
  AUDIO_MUTE  /// The key for Audiomute
  ,
  MEDIASELECT  /// The key for Mediaselect
  ,
  WWW  /// The key for Www
  ,
  MAIL  /// The key for Mail
  ,
  CALCULATOR  /// The key for Calculator
  ,
  COMPUTER  /// The key for Computer
  ,
  AC_SEARCH  /// The key for Ac_Search
  ,
  AC_HOME  /// The key for Ac_Home
  ,
  AC_BACK  /// The key for Ac_Back
  ,
  AC_FORWARD  /// The key for Ac_Forward
  ,
  AC_STOP  /// The key for Ac_Stop
  ,
  AC_REFRESH  /// The key for Ac_Refresh
  ,
  AC_BOOKMARKS  /// The key for Ac_Bookmarks
  ,
  BRIGHTNESS_DOWN  /// The key for Brightnessdown
  ,
  BRIGHTNESS_UP  /// The key for Brightnessup
  ,
  DISPLAYSWITCH  /// The key for Displayswitch
  ,
  KEYBOARD_ILLUM_TOGGLE  /// The key for Kbdillumtoggle
  ,
  KEYBOARD_ILLUM_DOWN  /// The key for Kbdillumdown
  ,
  KEYBOARD_ILLUM_UP  /// The key for Kbdillumup
  ,
  EJECT  /// The key for Eject
  ,
  SLEEP  /// The key for Sleep
};

/** Generate a string representation of the given key
@param k the key
@returns the string representation
 */
std::string
ToString(Key k);

/** Generate a key from a string.
@param keyname the name of the key
@returns the key or None
 */
Key
ToKey(const std::string& keyname);

#endif  // EUPHORIA_INPUT_KEY_H_
