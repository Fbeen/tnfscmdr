#ifndef DLG_SERVER_H
#define DLG_SERVER_H

#include "cfg.h"

#define DLG_ACTION_CONNECT  0
#define DLG_ACTION_CANCEL   1
#define DLG_ACTION_ADDNEW   2
#define DLG_ACTION_EDIT     3

typedef struct {
    int action;
    int index;   /* server index; geldig bij CONNECT en EDIT */
} DlgServerResult;

void dlg_server_select(Config *cfg, DlgServerResult *result, int current_idx);

#endif
