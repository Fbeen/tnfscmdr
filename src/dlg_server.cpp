#include <string.h>
#include "dlg_server.h"
#include "screen.h"
#include "input.h"

#define COL_DLG_BG      0x70   /* lightgrey bg, black text        */
#define COL_DLG_TITLE   0x7F   /* lightgrey bg, bright white text */
#define COL_DLG_CURSOR  0x17   /* blue bg, lightgrey text         */
#define COL_DLG_HINTS   0x78   /* lightgrey bg, dark grey text    */

#define DLG_W   44
#define DLG_X   ((80 - DLG_W) / 2)

#define IDX_ADDNEW(cfg)  ((cfg)->server_count)
#define IDX_CANCEL(cfg)  ((cfg)->server_count + 1)
#define TOTAL_ITEMS(cfg) ((cfg)->server_count + 2)

static void draw_server_item(int x, int y, CfgServer *srv, int selected)
{
    unsigned char color = selected ? COL_DLG_CURSOR : COL_DLG_BG;
    screen_fill(x, y, DLG_W - 2, 1, ' ', color);
    screen_printf(x + 1, y, color, "%-12s  %-25s", srv->name, srv->server);
}

static void draw_extra_item(int x, int y, const char *label, int selected)
{
    unsigned char color = selected ? COL_DLG_CURSOR : COL_DLG_BG;
    screen_fill(x, y, DLG_W - 2, 1, ' ', color);
    screen_text(x + 1, y, label, color);
}

static void dlg_draw(Config *cfg, int cursor)
{
    int i;
    int dlg_h  = cfg->server_count + 6;
    int dlg_y  = (25 - dlg_h) / 2;
    int item_y = dlg_y + 1;
    int sep_y  = item_y + cfg->server_count;
    int add_y  = sep_y + 1;
    int can_y  = add_y + 1;
    int hint_y = can_y + 1;

    screen_box(DLG_X, dlg_y, DLG_W, dlg_h, COL_DLG_BG);
    screen_fill(DLG_X + 1, dlg_y + 1, DLG_W - 2, dlg_h - 2, ' ', COL_DLG_BG);
    screen_text(DLG_X + 2, dlg_y, " Select server ", COL_DLG_TITLE);

    for (i = 0; i < cfg->server_count; i++) {
        draw_server_item(DLG_X + 1, item_y + i, &cfg->servers[i], cursor == i);
    }

    screen_fill(DLG_X + 1, sep_y, DLG_W - 2, 1, ' ', COL_DLG_BG);

    draw_extra_item(DLG_X + 1, add_y, "Add new", cursor == IDX_ADDNEW(cfg));
    draw_extra_item(DLG_X + 1, can_y, "Cancel",  cursor == IDX_CANCEL(cfg));

    screen_fill(DLG_X + 1, hint_y, DLG_W - 2, 1, ' ', COL_DLG_HINTS);
    screen_text(DLG_X + 2, hint_y,
        "Enter=Select  E=Edit  A=Add  Esc=Cancel",
        COL_DLG_HINTS);
}

void dlg_server_select(Config *cfg, DlgServerResult *result, int current_idx)
{
    int cursor  = (current_idx >= 0 && current_idx < cfg->server_count) ? current_idx : 0;
    int total   = TOTAL_ITEMS(cfg);
    int running = 1;
    InputEvent ev;

    result->action = DLG_ACTION_CANCEL;
    result->index  = 0;

    if (cfg->server_count == 0) {
        result->action = DLG_ACTION_ADDNEW;
        return;
    }

    dlg_draw(cfg, cursor);

    while (running) {
        ev = input_read_event();

        switch (ev.key) {
            case KEY_UP:
                if (cursor > 0) {
                    cursor--;
                    dlg_draw(cfg, cursor);
                }
                break;

            case KEY_DOWN:
                if (cursor < total - 1) {
                    cursor++;
                    dlg_draw(cfg, cursor);
                }
                break;

            case KEY_ENTER:
                if (cursor == IDX_ADDNEW(cfg)) {
                    result->action = DLG_ACTION_ADDNEW;
                } else if (cursor == IDX_CANCEL(cfg)) {
                    result->action = DLG_ACTION_CANCEL;
                } else {
                    result->action = DLG_ACTION_CONNECT;
                    result->index  = cursor;
                }
                running = 0;
                break;

            case 'E':
            case 'e':
                if (cursor < cfg->server_count) {
                    result->action = DLG_ACTION_EDIT;
                    result->index  = cursor;
                    running = 0;
                }
                break;

            case 'A':
            case 'a':
                result->action = DLG_ACTION_ADDNEW;
                running = 0;
                break;

            case KEY_ESC:
                result->action = DLG_ACTION_CANCEL;
                running = 0;
                break;

            default:
                break;
        }
    }
}
