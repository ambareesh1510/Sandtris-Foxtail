#ifndef GBA_H
#define GBA_H

/* An unsigned 32-bit (4-byte) type */
#include "syscall_defs.h"
typedef unsigned int u32;

/* An unsigned 16-bit (2-byte) type */
typedef unsigned short u16;

/* An unsigned 8-bit (1-byte) type. Note that this type cannot be written onto
 * RAM directly. */
typedef unsigned char u8;

// ---------------------------------------------------------------------------
//                       MODE3 MACROS
// ---------------------------------------------------------------------------
#define OFFSET(row, col, width) ((col) + (width) * (row))

#define REG_DISPCNT (*(volatile unsigned short *)0x4000000)
#define MODE3 3
#define BG2_ENABLE (1 << 10)

#define COLOR(r, g, b) ((r) | (g) << 5 | (b) << 10)
#define COL_WHITE COLOR(31, 31, 31)
#define COL_RED COLOR(31, 0, 0)
#define COL_GREEN COLOR(0, 31, 0)
#define COL_BLUE COLOR(0, 0, 31)
#define COL_MAGENTA COLOR(31, 0, 31)
#define COL_CYAN COLOR(0, 31, 31)
#define COL_YELLOW COLOR(31, 31, 0)
#define COL_BLACK COLOR(0, 0, 0)
#define COL_GRAY COLOR(5, 5, 5)

// The size of the GBA Screen
#define WIDTH 240
#define HEIGHT 160

// This is initialized in gba.c
extern volatile u32 *videoBuffer;
void initVideoBuffer(void);
void presentVideoBuffer(void);

// ---------------------------------------------------------------------------
//                       BUTTON INPUT
// ---------------------------------------------------------------------------
#define BUTTON_A (1 << 0)
#define BUTTON_B (1 << 1)
#define BUTTON_SELECT (1 << 2)
#define BUTTON_START (1 << 3)
#define BUTTON_RIGHT (1 << 4)
#define BUTTON_LEFT (1 << 5)
#define BUTTON_UP (1 << 6)
#define BUTTON_DOWN (1 << 7)
#define BUTTON_R (1 << 8)
#define BUTTON_L (1 << 9)

#define SC_A 44 // 'z'
#define SC_B 45 // 'x'
#define SC_SELECT 28 // '\n'
#define SC_START 54 // right shift
#define SC_RIGHT 38 // 'l'
#define SC_LEFT 36 // 'j'
#define SC_UP 23 // 'i'
#define SC_DOWN 37 // 'k'
#define SC_R 30 // 'a'
#define SC_L 31 // 's'
#define SC_BREAK 0b10000000

#define KEY_MASK 0x03FF
#define KEY_DOWN(key, buttons) (~(buttons) & (key))

// Remember that a button is recently pressed if it wasn't pressed in the last
// input (oldButtons) but is pressed in the current input. Use the KEY_DOWN
// macro to check if the button was pressed in the inputs.
extern u16 __key_curr, __key_prev;

static inline void key_poll(void) {
    __key_prev = __key_curr;
    // TODO: implement this properly
    unsigned char buf[100];
    int bytes = read(1, (char *) buf, 100);
    for (int i = 0; i < bytes; i++) {
        unsigned char scancode = buf[i];
        switch (scancode) {
            case SC_A: __key_curr |= BUTTON_A; break;
            case SC_B: __key_curr |= BUTTON_B; break;
            case SC_SELECT: __key_curr |= BUTTON_SELECT; break;
            case SC_START: __key_curr |= BUTTON_START; break;
            case SC_RIGHT: __key_curr |= BUTTON_RIGHT; break;
            case SC_LEFT: __key_curr |= BUTTON_LEFT; break;
            case SC_UP: __key_curr |= BUTTON_UP; break;
            case SC_DOWN: __key_curr |= BUTTON_DOWN; break;
            case SC_R: __key_curr |= BUTTON_R; break;
            case SC_L: __key_curr |= BUTTON_L; break;

            case (SC_BREAK | SC_A): __key_curr &= ~BUTTON_A; break;
            case (SC_BREAK | SC_B): __key_curr &= ~BUTTON_B; break;
            case (SC_BREAK | SC_SELECT): __key_curr &= ~BUTTON_SELECT; break;
            case (SC_BREAK | SC_START): __key_curr &= ~BUTTON_START; break;
            case (SC_BREAK | SC_RIGHT): __key_curr &= ~BUTTON_RIGHT; break;
            case (SC_BREAK | SC_LEFT): __key_curr &= ~BUTTON_LEFT; break;
            case (SC_BREAK | SC_UP): __key_curr &= ~BUTTON_UP; break;
            case (SC_BREAK | SC_DOWN): __key_curr &= ~BUTTON_DOWN; break;
            case (SC_BREAK | SC_R): __key_curr &= ~BUTTON_R; break;
            case (SC_BREAK | SC_L): __key_curr &= ~BUTTON_L; break;
            default: break;
        }
    }
    __key_curr = __key_curr & KEY_MASK;
}

// Basic state checks
static inline u32 key_curr_state(void) { return __key_curr; }
static inline u32 key_prev_state(void) { return __key_prev; }
static inline u32 key_is_down(u32 key) { return __key_curr & key; }
static inline u32 key_is_up(u32 key) { return ~__key_curr & key; }
static inline u32 key_was_down(u32 key) { return __key_prev & key; }
static inline u32 key_was_up(u32 key) { return ~__key_prev & key; }

// Key is changing state.
static inline u32 key_transit(u32 key) {
    return (__key_curr ^ __key_prev) & key;
}

// Key is held (down now and before).
static inline u32 key_held(u32 key) { return (__key_curr & __key_prev) & key; }

// Key is being hit (down now, but not before).
static inline u32 key_hit(u32 key) { return (__key_curr & ~__key_prev) & key; }

// Key is not held but was held before
static inline u32 key_released(u32 key) {
    return (~__key_curr & __key_prev) & key;
}

// ---------------------------------------------------------------------------
//                       VBLANK
// ---------------------------------------------------------------------------

// Use this variable to count vBlanks. Initialized in gba.c and to be
// manipulated by waitForVBlank()
extern u32 vBlankCounter;

/*
 * Runs a blocking loop until the start of next VBlank.
 */
void waitForVBlank(void);
void mWaitForVBlank(void);

// ---------------------------------------------------------------------------
//                       MISC
// ---------------------------------------------------------------------------
#define UNUSED(param) ((void)((param)))

/*
 * Generates a pseudo-random number between min and max.
 *
 * @param  min bottom end of range (inclusive).
 * @param  max top end of range (exclusive).
 * @return random number in the given range.
 */
int randint(int min, int max);

// ---------------------------------------------------------------------------
//                       DRAWING
// ---------------------------------------------------------------------------
void setPixel(int row, int col, u16 color);
void drawRectDMA(int row, int col, int width, int height, volatile u16 color);
void drawRectDMA(int row, int col, int width, int height, volatile u16 color);
void drawFullScreenImageDMA(const u16 *image);
void drawImageDMA(int row, int col, int width, int height, const u16 *image);
void drawImageDMA(int row, int col, int width, int height, const u16 *image);
void undrawImageDMA(int row, int col, int width, int height, const u16 *image);
void fillScreenDMA(volatile u16 color);
void drawChar(int row, int col, char ch, u16 color);
void drawString(int row, int col, char *str, u16 color);
void drawCenteredString(int row, int col, int width, int height, char *str,
                        u16 color);

/* Contains the pixels of each character from a 6x8 font */
// This is in the font.c file. You can replace the font if you want.
extern const unsigned char fontdata_6x8[12288];

#endif
