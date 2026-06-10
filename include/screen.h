#ifndef SCREEN_H
#define SCREEN_H

void screen_clear(unsigned char color);
void screen_put(int x, int y, char c, unsigned char color);
void screen_text(int x, int y, const char *text, unsigned char color);
void screen_box(int x, int y, int w, int h, unsigned char color);
void screen_fill(int x, int y, int w, int h, char c, unsigned char color);
void screen_printf(int x, int y, unsigned char color, const char *fmt, ...);

#endif