/*
	Port of
	Simple DirectMedia Layer PHI_keycode.h
	From Sam Lantinga <slouken@libsdl.org>
*/

#ifndef _PHI_KEYCODE_H_
#define _PHI_KEYCODE_H_

#include "scancode.h"

typedef int keycode;

#define PHI_SCANCODE_MASK (1<<30)
#define PHI_SCANCODE_TO_KEYCODE(X)  (X | PHI_SCANCODE_MASK)

enum
{
    PHIK_UNKNOWN = 0,

    PHIK_RETURN = '\r',
    PHIK_ESCAPE = '\033',
    PHIK_BACKSPACE = '\b',
    PHIK_TAB = '\t',
    PHIK_SPACE = ' ',
    PHIK_EXCLAIM = '!',
    PHIK_QUOTEDBL = '"',
    PHIK_HASH = '#',
    PHIK_PERCENT = '%',
    PHIK_DOLLAR = '$',
    PHIK_AMPERSAND = '&',
    PHIK_QUOTE = '\'',
    PHIK_LEFTPAREN = '(',
    PHIK_RIGHTPAREN = ')',
    PHIK_ASTERISK = '*',
    PHIK_PLUS = '+',
    PHIK_COMMA = ',',
    PHIK_MINUS = '-',
    PHIK_PERIOD = '.',
    PHIK_SLASH = '/',
    PHIK_0 = '0',
    PHIK_1 = '1',
    PHIK_2 = '2',
    PHIK_3 = '3',
    PHIK_4 = '4',
    PHIK_5 = '5',
    PHIK_6 = '6',
    PHIK_7 = '7',
    PHIK_8 = '8',
    PHIK_9 = '9',
    PHIK_COLON = ':',
    PHIK_SEMICOLON = ';',
    PHIK_LESS = '<',
    PHIK_EQUALS = '=',
    PHIK_GREATER = '>',
    PHIK_QUESTION = '?',
    PHIK_AT = '@',
    /*
       Skip uppercase letters
     */
    PHIK_LEFTBRACKET = '[',
    PHIK_BACKSLASH = '\\',
    PHIK_RIGHTBRACKET = ']',
    PHIK_CARET = '^',
    PHIK_UNDERSCORE = '_',
    PHIK_BACKQUOTE = '`',
    PHIK_a = 'a',
    PHIK_b = 'b',
    PHIK_c = 'c',
    PHIK_d = 'd',
    PHIK_e = 'e',
    PHIK_f = 'f',
    PHIK_g = 'g',
    PHIK_h = 'h',
    PHIK_i = 'i',
    PHIK_j = 'j',
    PHIK_k = 'k',
    PHIK_l = 'l',
    PHIK_m = 'm',
    PHIK_n = 'n',
    PHIK_o = 'o',
    PHIK_p = 'p',
    PHIK_q = 'q',
    PHIK_r = 'r',
    PHIK_s = 's',
    PHIK_t = 't',
    PHIK_u = 'u',
    PHIK_v = 'v',
    PHIK_w = 'w',
    PHIK_x = 'x',
    PHIK_y = 'y',
    PHIK_z = 'z',

    PHIK_CAPSLOCK = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_CAPSLOCK),

    PHIK_F1 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F1),
    PHIK_F2 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F2),
    PHIK_F3 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F3),
    PHIK_F4 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F4),
    PHIK_F5 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F5),
    PHIK_F6 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F6),
    PHIK_F7 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F7),
    PHIK_F8 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F8),
    PHIK_F9 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F9),
    PHIK_F10 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F10),
    PHIK_F11 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F11),
    PHIK_F12 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F12),

    PHIK_PRINTSCREEN = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_PRINTSCREEN),
    PHIK_SCROLLLOCK = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_SCROLLLOCK),
    PHIK_PAUSE = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_PAUSE),
    PHIK_INSERT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_INSERT),
    PHIK_HOME = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_HOME),
    PHIK_PAGEUP = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_PAGEUP),
    PHIK_DELETE = '\177',
    PHIK_END = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_END),
    PHIK_PAGEDOWN = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_PAGEDOWN),
    PHIK_RIGHT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_RIGHT),
    PHIK_LEFT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_LEFT),
    PHIK_DOWN = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_DOWN),
    PHIK_UP = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_UP),

    PHIK_NUMLOCKCLEAR = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_NUMLOCKCLEAR),
    PHIK_KP_DIVIDE = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_DIVIDE),
    PHIK_KP_MULTIPLY = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_MULTIPLY),
    PHIK_KP_MINUS = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_MINUS),
    PHIK_KP_PLUS = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_PLUS),
    PHIK_KP_ENTER = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_ENTER),
    PHIK_KP_1 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_1),
    PHIK_KP_2 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_2),
    PHIK_KP_3 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_3),
    PHIK_KP_4 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_4),
    PHIK_KP_5 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_5),
    PHIK_KP_6 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_6),
    PHIK_KP_7 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_7),
    PHIK_KP_8 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_8),
    PHIK_KP_9 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_9),
    PHIK_KP_0 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_0),
    PHIK_KP_PERIOD = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_PERIOD),

    PHIK_APPLICATION = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_APPLICATION),
    PHIK_POWER = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_POWER),
    PHIK_KP_EQUALS = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_EQUALS),
    PHIK_F13 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F13),
    PHIK_F14 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F14),
    PHIK_F15 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F15),
    PHIK_F16 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F16),
    PHIK_F17 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F17),
    PHIK_F18 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F18),
    PHIK_F19 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F19),
    PHIK_F20 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F20),
    PHIK_F21 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F21),
    PHIK_F22 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F22),
    PHIK_F23 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F23),
    PHIK_F24 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_F24),
    PHIK_EXECUTE = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_EXECUTE),
    PHIK_HELP = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_HELP),
    PHIK_MENU = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_MENU),
    PHIK_SELECT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_SELECT),
    PHIK_STOP = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_STOP),
    PHIK_AGAIN = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_AGAIN),
    PHIK_UNDO = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_UNDO),
    PHIK_CUT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_CUT),
    PHIK_COPY = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_COPY),
    PHIK_PASTE = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_PASTE),
    PHIK_FIND = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_FIND),
    PHIK_MUTE = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_MUTE),
    PHIK_VOLUMEUP = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_VOLUMEUP),
    PHIK_VOLUMEDOWN = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_VOLUMEDOWN),
    PHIK_KP_COMMA = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_COMMA),
    PHIK_KP_EQUALSAS400 =
        PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_EQUALSAS400),

    PHIK_ALTERASE = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_ALTERASE),
    PHIK_SYSREQ = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_SYSREQ),
    PHIK_CANCEL = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_CANCEL),
    PHIK_CLEAR = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_CLEAR),
    PHIK_PRIOR = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_PRIOR),
    PHIK_RETURN2 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_RETURN2),
    PHIK_SEPARATOR = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_SEPARATOR),
    PHIK_OUT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_OUT),
    PHIK_OPER = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_OPER),
    PHIK_CLEARAGAIN = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_CLEARAGAIN),
    PHIK_CRSEL = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_CRSEL),
    PHIK_EXSEL = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_EXSEL),

    PHIK_KP_00 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_00),
    PHIK_KP_000 = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_000),
    PHIK_THOUSANDSSEPARATOR =
        PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_THOUSANDSSEPARATOR),
    PHIK_DECIMALSEPARATOR =
        PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_DECIMALSEPARATOR),
    PHIK_CURRENCYUNIT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_CURRENCYUNIT),
    PHIK_CURRENCYSUBUNIT =
        PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_CURRENCYSUBUNIT),
    PHIK_KP_LEFTPAREN = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_LEFTPAREN),
    PHIK_KP_RIGHTPAREN = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_RIGHTPAREN),
    PHIK_KP_LEFTBRACE = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_LEFTBRACE),
    PHIK_KP_RIGHTBRACE = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_RIGHTBRACE),
    PHIK_KP_TAB = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_TAB),
    PHIK_KP_BACKSPACE = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_BACKSPACE),
    PHIK_KP_A = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_A),
    PHIK_KP_B = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_B),
    PHIK_KP_C = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_C),
    PHIK_KP_D = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_D),
    PHIK_KP_E = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_E),
    PHIK_KP_F = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_F),
    PHIK_KP_XOR = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_XOR),
    PHIK_KP_POWER = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_POWER),
    PHIK_KP_PERCENT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_PERCENT),
    PHIK_KP_LESS = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_LESS),
    PHIK_KP_GREATER = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_GREATER),
    PHIK_KP_AMPERSAND = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_AMPERSAND),
    PHIK_KP_DBLAMPERSAND =
        PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_DBLAMPERSAND),
    PHIK_KP_VERTICALBAR =
        PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_VERTICALBAR),
    PHIK_KP_DBLVERTICALBAR =
        PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_DBLVERTICALBAR),
    PHIK_KP_COLON = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_COLON),
    PHIK_KP_HASH = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_HASH),
    PHIK_KP_SPACE = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_SPACE),
    PHIK_KP_AT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_AT),
    PHIK_KP_EXCLAM = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_EXCLAM),
    PHIK_KP_MEMSTORE = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_MEMSTORE),
    PHIK_KP_MEMRECALL = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_MEMRECALL),
    PHIK_KP_MEMCLEAR = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_MEMCLEAR),
    PHIK_KP_MEMADD = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_MEMADD),
    PHIK_KP_MEMSUBTRACT =
        PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_MEMSUBTRACT),
    PHIK_KP_MEMMULTIPLY =
        PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_MEMMULTIPLY),
    PHIK_KP_MEMDIVIDE = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_MEMDIVIDE),
    PHIK_KP_PLUSMINUS = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_PLUSMINUS),
    PHIK_KP_CLEAR = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_CLEAR),
    PHIK_KP_CLEARENTRY = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_CLEARENTRY),
    PHIK_KP_BINARY = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_BINARY),
    PHIK_KP_OCTAL = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_OCTAL),
    PHIK_KP_DECIMAL = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_DECIMAL),
    PHIK_KP_HEXADECIMAL =
        PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KP_HEXADECIMAL),

    PHIK_LCTRL = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_LCTRL),
    PHIK_LSHIFT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_LSHIFT),
    PHIK_LALT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_LALT),
    PHIK_LGUI = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_LGUI),
    PHIK_RCTRL = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_RCTRL),
    PHIK_RSHIFT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_RSHIFT),
    PHIK_RALT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_RALT),
    PHIK_RGUI = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_RGUI),

    PHIK_MODE = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_MODE),

    PHIK_AUDIONEXT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_AUDIONEXT),
    PHIK_AUDIOPREV = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_AUDIOPREV),
    PHIK_AUDIOSTOP = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_AUDIOSTOP),
    PHIK_AUDIOPLAY = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_AUDIOPLAY),
    PHIK_AUDIOMUTE = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_AUDIOMUTE),
    PHIK_MEDIASELECT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_MEDIASELECT),
    PHIK_WWW = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_WWW),
    PHIK_MAIL = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_MAIL),
    PHIK_CALCULATOR = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_CALCULATOR),
    PHIK_COMPUTER = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_COMPUTER),
    PHIK_AC_SEARCH = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_AC_SEARCH),
    PHIK_AC_HOME = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_AC_HOME),
    PHIK_AC_BACK = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_AC_BACK),
    PHIK_AC_FORWARD = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_AC_FORWARD),
    PHIK_AC_STOP = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_AC_STOP),
    PHIK_AC_REFRESH = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_AC_REFRESH),
    PHIK_AC_BOOKMARKS = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_AC_BOOKMARKS),

    PHIK_BRIGHTNESSDOWN =
        PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_BRIGHTNESSDOWN),
    PHIK_BRIGHTNESSUP = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_BRIGHTNESSUP),
    PHIK_DISPLAYSWITCH = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_DISPLAYSWITCH),
    PHIK_KBDILLUMTOGGLE =
        PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KBDILLUMTOGGLE),
    PHIK_KBDILLUMDOWN = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KBDILLUMDOWN),
    PHIK_KBDILLUMUP = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_KBDILLUMUP),
    PHIK_EJECT = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_EJECT),
    PHIK_SLEEP = PHI_SCANCODE_TO_KEYCODE(PHI_SCANCODE_SLEEP)
};

/**
 * \brief Enumeration of valid key mods (possibly OR'd together).
 */
typedef enum
{
    KEYMOD_NONE = 0x0000,
    KEYMOD_LSHIFT = 0x0001,
    KEYMOD_RSHIFT = 0x0002,
    KEYMOD_LCTRL = 0x0040,
    KEYMOD_RCTRL = 0x0080,
    KEYMOD_LALT = 0x0100,
    KEYMOD_RALT = 0x0200,
    KEYMOD_LGUI = 0x0400,
    KEYMOD_RGUI = 0x0800,
    KEYMOD_NUM = 0x1000,
    KEYMOD_CAPS = 0x2000,
    KEYMOD_MODE = 0x4000,
    KEYMOD_RESERVED = 0x8000
} keymod;

#define KEYMOD_CTRL   (KMOD_LCTRL|KMOD_RCTRL)
#define KEYMOD_SHIFT  (KMOD_LSHIFT|KMOD_RSHIFT)
#define KEYMOD_ALT    (KMOD_LALT|KMOD_RALT)
#define KEYMOD_GUI    (KMOD_LGUI|KMOD_RGUI)

#endif /* _PHI_keycode_h */

/* vi: set ts=4 sw=4 expandtab: */
