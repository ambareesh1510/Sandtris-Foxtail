#include "gba.h"
#include "util.h"
#include "malloc.h"
#include "syscall_defs.h"

u16 __key_curr = 0, __key_prev = 0;

volatile u32 *videoBuffer;
void initVideoBuffer(void) {
    videoBuffer = malloc(WIDTH * HEIGHT * sizeof(u32));
}
void presentVideoBuffer(void) {
    draw_pixels((unsigned int *) videoBuffer, HEIGHT, WIDTH);
}
u32 vBlankCounter = 0;

/*
  Wait until the start of the next VBlank. This is useful to avoid tearing.
  Completing this function is required.
*/
void waitForVBlank(void) {
    // (1)
    // Write a while loop that loops until we're NOT in vBlank anymore:
    // (This prevents counting one VBlank more than once if your app is too
    // fast)
    // while (SCANLINECOUNTER >= 160)
    //     ;

    // (2)
    // Write a while loop that keeps going until we're in vBlank:
    // while (SCANLINECOUNTER < 160)
    //     ;

    // (3)
    // Finally, increment the vBlank counter:
    vBlankCounter++;
}

static int __qran_seed = 42;
static inline int qran(void) {
    __qran_seed = 1664525 * __qran_seed + 1013904223;
    return (__qran_seed >> 16) & 0x7FFF;
}

inline int randint(int min, int max) { return (qran() * (max - min) >> 15) + min; }

u32 gbaColorToRGB32(u16 gba)
{
    u32 r5 = (gba >> 10) & 0x1F;
    u32 g5 = (gba >> 5)  & 0x1F;
    u32 b5 =  gba        & 0x1F;

    u32 r8 = (r5 << 3) | (r5 >> 2);
    u32 g8 = (g5 << 3) | (g5 >> 2);
    u32 b8 = (b5 << 3) | (b5 >> 2);

    return 0xFF000000 | (r8 << 16) | (g8 << 8) | b8;
}

/*
  Sets a pixel in the video buffer to a given color.
  Using DMA is NOT recommended. (In fact, using DMA with this function would be
  really slow!)
*/
inline void setPixel(int row, int col, u16 color) {
    videoBuffer[row * WIDTH + col] = gbaColorToRGB32(color);
}

void drawRectDMA(int row, int col, int width, int height, volatile u16 color) {
    for (int x = 0; x < height; x++) {
        memset((void *) &videoBuffer[OFFSET(row + x, col, WIDTH)], gbaColorToRGB32(color), width * 4);
    }
}

void drawFullScreenImageDMA(const u16 *image) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            videoBuffer[OFFSET(y, x, WIDTH)] = gbaColorToRGB32(image[OFFSET(y, x, WIDTH)]);
        }
    }
}

void drawImageDMA(int row, int col, int width, int height, const u16 *image) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int ay = y + row;
            int ax = x + col;
            if (ay < 0 || ay >= HEIGHT || ax < 0 || ax >= WIDTH) {
                continue;
            }
            videoBuffer[OFFSET(y + row, x + col, WIDTH)] = gbaColorToRGB32(image[OFFSET(y, x, width)]);
        }
    }
}

/* STRING-DRAWING FUNCTIONS (provided) */
void drawChar(int row, int col, char ch, u16 color) {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
            if (fontdata_6x8[OFFSET(j, i, 6) + ch * 48]) {
                setPixel(row + j, col + i, color);
            }
        }
    }
}

void drawString(int row, int col, char *str, u16 color) {
    while (*str) {
        drawChar(row, col, *str++, color);
        col += 6;
    }
}

void drawCenteredString(int row, int col, int width, int height, char *str,
                        u16 color) {
    u32 len = 0;
    char *strCpy = str;
    while (*strCpy) {
        len++;
        strCpy++;
    }

    u32 strWidth = 6 * len;
    u32 strHeight = 8;

    int new_row = row + ((height - strHeight) >> 1);
    int new_col = col + ((width - strWidth) >> 1);
    drawString(new_row, new_col, str, color);
}
