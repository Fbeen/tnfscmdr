#ifndef INPUT_H
#define INPUT_H

#define KEY_NONE   0
#define KEY_ESC    27
#define KEY_ENTER  13
#define KEY_TAB    9

#define KEY_UP     1001
#define KEY_DOWN   1002
#define KEY_LEFT   1003
#define KEY_RIGHT  1004
#define KEY_F1     1011
#define KEY_F5     1015
#define KEY_F10    1020

#define KEY_SPACE       32

#define KEY_CTRL_UP     1101
#define KEY_CTRL_DOWN   1102
#define KEY_SHIFT_UP    1201
#define KEY_SHIFT_DOWN  1202

#define MOD_SHIFT  0x01
#define MOD_CTRL   0x02
#define MOD_ALT    0x04

typedef struct {
    int key;
    unsigned char modifiers;
    unsigned char dummy;
} InputEvent;

InputEvent input_read_event(void);

#endif