#include "screen.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define SCREEN_W 80
#define SCREEN_H 25

static unsigned short far *vram = (unsigned short far *)0xB8000000L;

void screen_put(int x, int y, char c, unsigned char color)
{
    if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) {
        return;
    }

    vram[y * SCREEN_W + x] = ((unsigned short)color << 8) | (unsigned char)c;
}

void screen_text(int x, int y, const char *text, unsigned char color)
{
    int i = 0;

    while (text[i] != 0) {
        screen_put(x + i, y, text[i], color);
        i++;
    }
}

void screen_fill(int x, int y, int w, int h, char c, unsigned char color)
{
    int xx;
    int yy;

    for (yy = 0; yy < h; yy++) {
        for (xx = 0; xx < w; xx++) {
            screen_put(x + xx, y + yy, c, color);
        }
    }
}

void screen_clear(unsigned char color)
{
    screen_fill(0, 0, SCREEN_W, SCREEN_H, ' ', color);
}

void screen_box(int x, int y, int w, int h, unsigned char color)
{
    int i;

    screen_put(x, y, 218, color);
    screen_put(x + w - 1, y, 191, color);
    screen_put(x, y + h - 1, 192, color);
    screen_put(x + w - 1, y + h - 1, 217, color);

    for (i = 1; i < w - 1; i++) {
        screen_put(x + i, y, 196, color);
        screen_put(x + i, y + h - 1, 196, color);
    }

    for (i = 1; i < h - 1; i++) {
        screen_put(x, y + i, 179, color);
        screen_put(x + w - 1, y + i, 179, color);
    }
}

void screen_printf(int x, int y, unsigned char color, const char *fmt, ...)
{
    char buf[256];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    screen_text(x, y, buf, color);
}