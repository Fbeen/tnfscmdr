#include <string.h>
#include <dos.h>
#include <direct.h>
#include "config.h"
#include "local_client.h"
#include "panel_item.h"

static int item_before(const PanelItem *a, const PanelItem *b)
{
    if (a->is_dir != b->is_dir)
        return a->is_dir > b->is_dir;
    return strcmp(a->name, b->name) < 0;
}

static void sort_items(PanelItem *items, int count)
{
    PanelItem tmp;
    int i, j;

    for (i = 1; i < count; i++) {
        tmp = items[i];
        j = i - 1;
        while (j >= 0 && item_before(&tmp, &items[j])) {
            items[j + 1] = items[j];
            j--;
        }
        items[j + 1] = tmp;
    }
}

int local_client_dir(PanelItem *items)
{
    struct _find_t info;
    unsigned rc;
    char cwd[128];
    int start = 0;
    int count = 0;

    /* ".." only when not at root (root = "C:\", length 3) */
    if (_getcwd(cwd, sizeof(cwd)) != NULL && strlen(cwd) > 3) {
        memset(&items[count], 0, sizeof(PanelItem));
        strcpy(items[count].name, "..");
        items[count].is_dir = 1;
        count++;
        start = 1;
    }

    rc = _dos_findfirst("*.*",
        _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_SUBDIR | _A_ARCH,
        &info);

    while (rc == 0 && count < PANEL_MAX_ITEMS) {
        if (info.name[0] != '.') {
            memset(&items[count], 0, sizeof(PanelItem));
            strncpy(items[count].name, info.name, sizeof(items[count].name) - 1);
            items[count].is_dir = (info.attrib & _A_SUBDIR) ? 1 : 0;
            items[count].size   = info.size;
            count++;
        }
        rc = _dos_findnext(&info);
    }

    sort_items(items + start, count - start);

    return count;
}
