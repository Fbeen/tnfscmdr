#include <conio.h>
#include <i86.h>
#include "input.h"
#include "screen.h"

static unsigned char input_get_modifiers(void)
{
    union REGS r;
    unsigned char mods;

    r.h.ah = 0x02;
    int86(0x16, &r, &r);

    mods = 0;

    if (r.h.al & 0x03) {
        mods |= MOD_SHIFT;
    }

    if (r.h.al & 0x04) {
        mods |= MOD_CTRL;
    }

    if (r.h.al & 0x08) {
        mods |= MOD_ALT;
    }

    return mods;
}

InputEvent input_read_event(void)
{
    InputEvent ev;
    union REGS r;
    int ascii, scan;

    ev.key = KEY_NONE;
    ev.modifiers = 0;

    /* Read key via BIOS INT 16h — bypasses DOS entirely so ^C is never
       echoed to the screen and INT 23h is never triggered. */
    r.h.ah = 0x00;
    int86(0x16, &r, &r);
    ascii = (unsigned char)r.h.al;
    scan  = (unsigned char)r.h.ah;

    ev.modifiers = input_get_modifiers();

    if (ascii == 0 || ascii == 0xE0) {
        switch (scan) {
            case 72:
            case 141:
                ev.key = KEY_UP;
                break;

            case 80:
            case 145:
                ev.key = KEY_DOWN;
                break;

            case 75:
            case 115:
                ev.key = KEY_LEFT;
                break;

            case 77:
            case 116:
                ev.key = KEY_RIGHT;
                break;

            case 59:
                ev.key = KEY_F1;
                break;

            case 63:
                ev.key = KEY_F5;
                break;

            case 68:
                ev.key = KEY_F10;
                break;

            default:
                ev.key = KEY_NONE;
                screen_printf(2, 23, 0x1E, "KEY_NONE scan=%d mods=%d   ", scan, ev.modifiers);
                break;
        }

        return ev;
    }

    ev.key = ascii;
    return ev;
}