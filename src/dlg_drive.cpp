#include <dos.h>
#include <direct.h>
#include "dlg_drive.h"
#include "screen.h"
#include "input.h"

#define COL_DLG_BG      0x70
#define COL_DLG_TITLE   0x7F
#define COL_DLG_CURSOR  0x17
#define COL_DLG_HINTS   0x78

#define DLG_W  28
#define DLG_X  ((80 - DLG_W) / 2)

#define MAX_DRIVES 16

static char drive_letters[MAX_DRIVES];
static int  drive_count;

static void scan_drives(void)
{
    struct _diskfree_t df;
    int d;

    drive_count = 0;

    drive_letters[drive_count++] = 'A';
    drive_letters[drive_count++] = 'B';

    for (d = 3; d <= 26 && drive_count < MAX_DRIVES; d++) {
        if (_dos_getdiskfree(d, &df) == 0)
            drive_letters[drive_count++] = 'A' + d - 1;
    }
}

static void draw_item(int x, int y, char letter, int selected)
{
    unsigned char color = selected ? COL_DLG_CURSOR : COL_DLG_BG;
    const char *label   = (letter == 'A' || letter == 'B') ? "Floppy" : "Drive ";

    screen_fill(x, y, DLG_W - 2, 1, ' ', color);
    screen_printf(x + 1, y, color, " %-6s %c:", label, (int)letter);
}

static void dlg_draw(int cursor)
{
    int dlg_h  = drive_count + 5;
    int dlg_y  = (25 - dlg_h) / 2;
    int item_y = dlg_y + 1;
    int sep_y  = item_y + drive_count;
    int can_y  = sep_y + 1;
    int hint_y = can_y + 1;
    unsigned char color;
    int i;

    screen_box(DLG_X, dlg_y, DLG_W, dlg_h, COL_DLG_BG);
    screen_fill(DLG_X + 1, dlg_y + 1, DLG_W - 2, dlg_h - 2, ' ', COL_DLG_BG);
    screen_text(DLG_X + 2, dlg_y, " Select drive ", COL_DLG_TITLE);

    for (i = 0; i < drive_count; i++)
        draw_item(DLG_X + 1, item_y + i, drive_letters[i], cursor == i);

    screen_fill(DLG_X + 1, sep_y, DLG_W - 2, 1, ' ', COL_DLG_BG);

    color = (cursor == drive_count) ? COL_DLG_CURSOR : COL_DLG_BG;
    screen_fill(DLG_X + 1, can_y, DLG_W - 2, 1, ' ', color);
    screen_text(DLG_X + 2, can_y, "Cancel", color);

    screen_fill(DLG_X + 1, hint_y, DLG_W - 2, 1, ' ', COL_DLG_HINTS);
    screen_text(DLG_X + 2, hint_y, "Enter=Select  Esc=Cancel", COL_DLG_HINTS);
}

void dlg_drive_select(DlgDriveResult *result)
{
    int cursor  = 0;
    int running = 1;
    InputEvent ev;

    result->action = DLG_DRIVE_CANCEL;
    result->drive  = 0;

    scan_drives();

    /* start cursor op de huidige drive */
    {
        char current = 'A' + _getdrive() - 1;
        int i;
        for (i = 0; i < drive_count; i++) {
            if (drive_letters[i] == current) {
                cursor = i;
                break;
            }
        }
    }

    dlg_draw(cursor);

    while (running) {
        ev = input_read_event();

        switch (ev.key) {
            case KEY_UP:
                if (cursor > 0) {
                    cursor--;
                    dlg_draw(cursor);
                }
                break;

            case KEY_DOWN:
                if (cursor < drive_count) {
                    cursor++;
                    dlg_draw(cursor);
                }
                break;

            case KEY_ENTER:
                if (cursor < drive_count) {
                    result->action = DLG_DRIVE_SELECT;
                    result->drive  = drive_letters[cursor];
                }
                running = 0;
                break;

            case KEY_ESC:
                running = 0;
                break;

            default:
                break;
        }
    }
}
