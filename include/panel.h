#ifndef PANEL_H
#define PANEL_H

#include "config.h"
#include "types.h"
#include "panel_item.h"

typedef struct {
    int x;
    int y;
    int w;
    int h;

    const char *title;
    const char *current_dir;

    PanelItem *items;
    int items_count;

    int cursor;
    int anchor;
    int shift_active;
    int scroll;
    int active;
} Panel;

void panel_init(Panel *panel, int x, int y, int w, int h, const char *title,
                const char *current_dir, PanelItem *items, int items_count);

void panel_draw(Panel *panel);
void panel_draw_frame(Panel *panel);
void panel_draw_items(Panel *panel);

void panel_clear_selection(Panel *panel);
void panel_select_only(Panel *panel, int index);
void panel_toggle_selection(Panel *panel, int index);
void panel_select_range(Panel *panel, int from, int to, int keep_existing);

int  panel_count_selected(Panel *panel);
void panel_move_active(Panel *panel, int delta, int shift, int ctrl);
void panel_space(Panel *panel, int shift, int ctrl);

void panel_select_none(Panel *panel);
void panel_fix_scroll(Panel *panel);

int  panel_refresh(Panel *panel);

#endif