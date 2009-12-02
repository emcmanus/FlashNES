#ifdef SDL

#include <SDL.h>
#define SDLK_A SDLK_a
#define SDLK_B SDLK_b
#define SDLK_C SDLK_c
#define SDLK_D SDLK_d
#define SDLK_E SDLK_e
#define SDLK_F SDLK_f
#define SDLK_G SDLK_g
#define SDLK_H SDLK_h
#define SDLK_I SDLK_i
#define SDLK_J SDLK_j
#define SDLK_K SDLK_k
#define SDLK_L SDLK_l
#define SDLK_M SDLK_m
#define SDLK_N SDLK_n
#define SDLK_O SDLK_o
#define SDLK_P SDLK_p
#define SDLK_Q SDLK_q
#define SDLK_R SDLK_r
#define SDLK_S SDLK_s
#define SDLK_T SDLK_t
#define SDLK_U SDLK_u
#define SDLK_V SDLK_v
#define SDLK_W SDLK_w
#define SDLK_X SDLK_x
#define SDLK_Y SDLK_y
#define SDLK_Z SDLK_z
#define SDLK_LEFTCONTROL SDLK_LCTRL
#define SDLK_RIGHTCONTROL SDLK_RCTRL
#define SDLK_LEFTALT SDLK_LALT
#define SDLK_RIGHTALT SDLK_RALT
#define SDLK_LEFTSHIFT SDLK_LSHIFT
#define SDLK_RIGHTSHIFT SDLK_RSHIFT
#define SDLK_CURSORDOWN SDLK_DOWN
#define SDLK_CURSORUP SDLK_UP
#define SDLK_CURSORLEFT SDLK_LEFT
#define SDLK_CURSORRIGHT SDLK_RIGHT
#define SDLK_ENTER SDLK_RETURN
#define SDLK_EQUAL SDLK_EQUALS
#define SDLK_APOSTROPHE SDLK_QUOTE
#define SDLK_BRACKET_LEFT SDLK_LEFTBRACKET
#define SDLK_BRACKET_RIGHT SDLK_RIGHTBRACKET
#define SDLK_SCROLLLOCK SDLK_SCROLLOCK		/* I guess the SDL people don't like lots of Ls... */
#define SDLK_GRAVE	SDLK_BACKQUOTE
#define MKK(k) SDLK_##k
#define MKK_COUNT (SDLK_LAST+1)


#else

// Moved from SDL_keysym.h

#define SDLK_A 97
#define SDLK_B 98
#define SDLK_C 99
#define SDLK_D 100
#define SDLK_E 101
#define SDLK_F 102
#define SDLK_G 103
#define SDLK_H 104
#define SDLK_I 105
#define SDLK_J 106
#define SDLK_K 107
#define SDLK_L 108
#define SDLK_M 109
#define SDLK_N 110
#define SDLK_O 111
#define SDLK_P 112
#define SDLK_Q 113
#define SDLK_R 114
#define SDLK_S 115
#define SDLK_T 116
#define SDLK_U 117
#define SDLK_V 118
#define SDLK_W 119
#define SDLK_X 120
#define SDLK_Y 121
#define SDLK_Z 122
#define SDLK_LEFTCONTROL 306
#define SDLK_RIGHTCONTROL 305
#define SDLK_LEFTALT 308
#define SDLK_RIGHTALT 307
#define SDLK_LEFTSHIFT 304
#define SDLK_RIGHTSHIFT 303
#define SDLK_CURSORDOWN 274
#define SDLK_CURSORUP 273
#define SDLK_CURSORLEFT 276
#define SDLK_CURSORRIGHT 275
#define SDLK_ENTER 13
#define SDLK_EQUAL 272
#define SDLK_APOSTROPHE 39
#define SDLK_BRACKET_LEFT 91
#define SDLK_BRACKET_RIGHT 93
#define SDLK_SCROLLLOCK 302
#define SDLK_GRAVE 96

// Other definitions referenced

#define SDLK_BACKSPACE 8
#define SDLK_TAB 9
#define SDLK_ESCAPE 27
#define SDLK_SPACE 32
#define SDLK_COMMA 44
#define SDLK_MINUS 45
#define SDLK_PERIOD 46
#define SDLK_SLASH 47

#define SDLK_0 48
#define SDLK_1 49
#define SDLK_2 50
#define SDLK_3 51
#define SDLK_4 52
#define SDLK_5 53
#define SDLK_6 54
#define SDLK_7 55
#define SDLK_8 56
#define SDLK_9 57

#define SDLK_COLON 58
#define SDLK_SEMICOLON 59
#define SDLK_BACKSLASH 92
#define SDLK_DELETE 127
#define SDLK_INSERT 277
#define SDLK_HOME 278
#define SDLK_END 279
#define SDLK_PAGEUP 280
#define SDLK_PAGEDOWN 281

#define SDLK_F1 282
#define SDLK_F2 283
#define SDLK_F3 284
#define SDLK_F4 285
#define SDLK_F5 286
#define SDLK_F6 287
#define SDLK_F7 288
#define SDLK_F8 289
#define SDLK_F9 290
#define SDLK_F10 291
#define SDLK_F11 292
#define SDLK_F12 293
#define SDLK_F13 294
#define SDLK_F14 295
#define SDLK_F15 296

// last ID defined in SDL's keysym
#define SDLK_LAST 322

#define MKK(k) SDLK_##k
#define MKK_COUNT (SDLK_LAST+1)

#endif
