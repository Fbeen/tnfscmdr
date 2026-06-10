#ifndef UI_H
#define UI_H

#include "panel_item.h"

#define UI_QUIT          0
#define UI_CHANGE_SERVER 1
#define UI_CHANGE_DRIVE  2

int ui_run(PanelItem *local_items, int *local_items_count,
           PanelItem *tnfs_items,  int tnfs_items_count,
           const char *remote_server, const char *remote_mount);

#endif