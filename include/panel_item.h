#ifndef PANEL_ITEM_H
#define PANEL_ITEM_H

#include "types.h"

typedef struct {
    char name[64];
    uint32_t size;
    uint16_t date;
    uint16_t time;
    uint8_t is_dir;
    uint8_t selected;
} PanelItem;

#endif