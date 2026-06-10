#include <stdio.h>
#include <string.h>
#include "config.h"
#include "netw.h"
#include "tnfs.h"
#include "tnfs_client.h"
#include "log.h"

int tnfs_client_connect(TnfsClient *client)
{
    int rc;

    log_printf("Connecting to %s...\n", client->server);

    netw_connect((char*)client->server, client->port, client->use_tcp);

    rc = tnfs_mount(client->mount, "", "");
    log_printf("mount rc=%d\n", rc);

    return rc;
}

void tnfs_client_disconnect()
{
    int rc;
    log_printf("Umounting...\n");
    rc = tnfs_umount();
    log_printf("Umount rc=%d\n", rc);
    netw_disconnect();
    log_printf("Disconnected.\n");
}


int tnfs_client_dir(PanelItem *items, const char *path)
{
    int rc;
    int count = 0;
    int start = 0;
    struct dirx_data dx_data;
    struct dirx_item dx_item;

    /* add ".." when not at root */
    if (path && !(path[0] == '/' && path[1] == '\0')) {
        memset(&items[0], 0, sizeof(PanelItem));
        strcpy(items[0].name, "..");
        items[0].is_dir = 1;
        count = 1;
        start = 1;
    }

    memset(&dx_data, 0, sizeof(dx_data));
    memset(&dx_item, 0, sizeof(dx_item));

    rc = tnfs_opendirx((char*)path, (char*)"*", 0, 0, &dx_data);
    if (rc != 0) {
        return start;
    }

    while (count < PANEL_MAX_ITEMS) {
        rc = tnfs_nextdirx(&dx_data, &dx_item);

        if (rc == TNFS_EOF) {
            break;
        }

        if (rc != 0) {
            tnfs_closedir((char)dx_data.handle);
            return start;
        }

        memset(&items[count], 0, sizeof(PanelItem));

        strncpy(items[count].name, dx_item.name, sizeof(items[count].name) - 1);
        items[count].name[sizeof(items[count].name) - 1] = 0;

        items[count].is_dir = (dx_item.flags & 1) ? 1 : 0;
        items[count].size = dx_item.size;

        count++;
    }

    tnfs_closedir((char)dx_data.handle);

    return count;
}