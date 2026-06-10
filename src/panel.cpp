#include <string.h>
#include <stdio.h>
#include "panel.h"
#include "screen.h"

#define COL_NORMAL    0x17  // 1 = blue background,       7 = lightgrey text
#define COL_FRAME     0x1F  // 1 = blue background,       F = white text
#define COL_TITLE     0x1E  // 1 = blue background,       E = yellow text
#define COL_SELECTED  0x70  // 7 = lightgrey background,  0 = black text
#define COL_CURSOR    0x1F  // 1 = blue background,       F = white text
#define COL_CURSOR_SELECTED 0x76  // 7 = lightgrey background,       6 = brown text

void panel_init(Panel *panel, int x, int y, int w, int h, const char *title,
                const char *current_dir, PanelItem *items, int items_count)
{
    panel->x = x;
    panel->y = y;
    panel->w = w;
    panel->h = h;
    panel->title = title;
    panel->current_dir = current_dir;
    panel->items = items;
    panel->items_count = items_count;
    panel->cursor = 0;
    panel->anchor = 0;
    panel->scroll = 0;
    panel->active = 0;
}

void panel_draw_frame(Panel *panel)
{
    unsigned char color;

    color = panel->active ? COL_FRAME : COL_NORMAL;

    screen_box(panel->x, panel->y, panel->w, panel->h, color);
    screen_text(panel->x + 2, panel->y, panel->title, COL_TITLE);

    screen_fill(panel->x + 1, panel->y + 1, panel->w - 2, 1, ' ', COL_NORMAL);
    if (panel->current_dir)
        screen_text(panel->x + 1, panel->y + 1, panel->current_dir, COL_TITLE);
}

static void panel_draw_scrollbar(Panel *panel)
{
    int visible_rows = panel->h - 3;
    int scr_x        = panel->x + panel->w - 2;
    int track_top    = panel->y + 2;
    int max_scroll   = panel->items_count - visible_rows;
    int thumb_h, thumb_start, i;

    /* proportional thumb; at least 1, at most visible_rows */
    if (panel->items_count <= 0) {
        thumb_h = visible_rows;
    } else {
        thumb_h = (visible_rows * visible_rows) / panel->items_count;
        if (thumb_h < 1) thumb_h = 1;
        if (thumb_h > visible_rows) thumb_h = visible_rows;
    }

    thumb_start = (max_scroll > 0)
        ? (panel->scroll * (visible_rows - thumb_h)) / max_scroll
        : 0;

    for (i = 0; i < visible_rows; i++) {
        if (i >= thumb_start && i < thumb_start + thumb_h)
            screen_fill(scr_x, track_top + i, 1, 1, '\xDB', 0x1F);  /* █ */
        else
            screen_fill(scr_x, track_top + i, 1, 1, '\xB0', 0x17);  /* ░ */
    }
}

/* Format size as Dutch dot-separated bytes, right-aligned in 13 chars.
   Max uint32 = 4.294.967.295 = 13 chars with dots. */
static void format_size(char *buf, uint32_t size)
{
    char tmp[16];
    char dotted[16];
    int raw_len, i, j, out_len, pad;

    sprintf(tmp, "%lu", size);
    raw_len = strlen(tmp);

    j = 0;
    for (i = 0; i < raw_len; i++) {
        int remaining = raw_len - i;
        if (i > 0 && remaining % 3 == 0)
            dotted[j++] = '.';
        dotted[j++] = tmp[i];
    }
    dotted[j] = '\0';

    out_len = j;
    pad = 13 - out_len;
    for (i = 0; i < pad; i++) buf[i] = ' ';
    strcpy(buf + pad, dotted);
}

void panel_draw_items(Panel *panel)
{
    int i;
    int row;
    int visible_rows;
    int item_index;
    unsigned char color;

    visible_rows = panel->h - 3;

    for (i = 0; i < visible_rows; i++) {
        row = panel->y + 2 + i;
        item_index = panel->scroll + i;

        color = COL_NORMAL;

        if (item_index < panel->items_count) {
            if (panel->items[item_index].selected) {
                color = COL_SELECTED;
            }

            if (item_index == panel->cursor && panel->active) {
                if (panel->items[item_index].selected) {
                    color = COL_CURSOR_SELECTED;
                } else {
                    color = COL_CURSOR;
                }
            }

            screen_fill(panel->x + 1, row, panel->w - 2, 1, ' ', color);
            screen_text(panel->x + 2, row, panel->items[item_index].name, color);
            if (panel->items[item_index].is_dir) {
                screen_text(panel->x + 15, row, "<DIR>", color);
            } else {
                char szbuf[12];
                format_size(szbuf, panel->items[item_index].size);
                screen_text(panel->x + 24, row, szbuf, color);
            }
        } else {
            screen_fill(panel->x + 1, row, panel->w - 2, 1, ' ', COL_NORMAL);
        }
    }

    panel_draw_scrollbar(panel);
}

void panel_draw(Panel *panel)
{
    panel_draw_frame(panel);
    panel_draw_items(panel);
}

void panel_clear_selection(Panel *panel)
{
    int i;

    for (i = 0; i < panel->items_count; i++) {
        panel->items[i].selected = 0;
    }
}

void panel_select_only(Panel *panel, int index)
{
    if (index < 0 || index >= panel->items_count) {
        return;
    }

    panel_clear_selection(panel);
    panel->items[index].selected = 1;
    panel->cursor = index;
    panel->anchor = index;
}

void panel_toggle_selection(Panel *panel, int index)
{
    if (index < 0 || index >= panel->items_count) {
        return;
    }

    panel->items[index].selected = !panel->items[index].selected;
}

void panel_select_range(Panel *panel, int from, int to, int keep_existing)
{
    int i;
    int start;
    int end;

    if (from < 0 || from >= panel->items_count) {
        return;
    }

    if (to < 0 || to >= panel->items_count) {
        return;
    }

    if (from < to) {
        start = from;
        end = to;
    } else {
        start = to;
        end = from;
    }

    if (!keep_existing) {
        panel_select_none(panel);
    }

    for (i = start; i <= end; i++) {
        panel->items[i].selected = 1;
    }
}

int panel_count_selected(Panel *panel)
{
    int i;
    int count;

    count = 0;

    for (i = 0; i < panel->items_count; i++) {
        if (panel->items[i].selected) {
            count++;
        }
    }

    return count;
}

void panel_move_active(Panel *panel, int delta, int shift, int ctrl)
{
    int old_active;
    int new_active;

    old_active = panel->cursor;
    new_active = old_active + delta;

    if (new_active < 0) new_active = 0;
    if (new_active >= panel->items_count) new_active = panel->items_count - 1;
    if (new_active == old_active) return;

    if (shift) {
        /*
         * First moment Shift is used: old active becomes anchor.
         * Daarna blijft anchor staan zolang Shift-range loopt.
         */
        if (!panel->shift_active) {
            panel->anchor = old_active;
            panel->shift_active = 1;
        }

        panel->cursor = new_active;
        panel_select_range(panel, panel->anchor, panel->cursor, ctrl);
    } else {

        panel->shift_active = 0;
        panel->cursor = new_active;
        panel->anchor = new_active;

        if (!ctrl) {
            panel_select_none(panel);
        }
    }

    panel_fix_scroll(panel);
}

void panel_space(Panel *panel, int shift, int ctrl)
{
    if (shift) {
        return;
    }

    if (ctrl) {
        panel->items[panel->cursor].selected =
            !panel->items[panel->cursor].selected;
        return;
    }

    if (panel_count_selected(panel) == 0) {
        panel->items[panel->cursor].selected = 1;
    }
}

void panel_select_none(Panel *panel)
{
    int i;

    for (i = 0; i < panel->items_count; i++) {
        panel->items[i].selected = 0;
    }
}

void panel_fix_scroll(Panel *panel)
{
    int visible_rows;

    visible_rows = panel->h - 3;

    if (panel->cursor < panel->scroll) {
        panel->scroll = panel->cursor;
    }

    if (panel->cursor >= panel->scroll + visible_rows) {
        panel->scroll = panel->cursor - visible_rows + 1;
    }
}

int panel_refresh(Panel *panel)
{
    int count;

    if (panel->items_count < 0) {
        return 0;
    }

    panel->cursor = 0;
    panel->anchor = 0;
    panel->scroll = 0;
    panel->shift_active = 0;

    panel_select_none(panel);

    return 0;
}