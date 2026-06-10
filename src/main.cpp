#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <direct.h>
#include <dos.h>

#include "ui.h"
#include "config.h"
#include "cfg.h"
#include "screen.h"
#include "dlg_server.h"
#include "dlg_drive.h"
#include "local_client.h"
#include "tnfs_client.h"
#include "panel_item.h"
#include "netw.h"
#include "log.h"

static void connect_server(Config *cfg, int idx, TnfsClient *client,
                           PanelItem *tnfs_items, int *tnfs_items_count)
{
    CfgServer *srv = &cfg->servers[idx];

    memset(client, 0, sizeof(TnfsClient));
    strncpy(client->server, srv->server, sizeof(client->server) - 1);
    strncpy(client->mount,  srv->mount,  sizeof(client->mount)  - 1);
    client->port    = srv->port;
    client->use_tcp = srv->use_tcp;

    tnfs_client_connect(client);
    *tnfs_items_count = tnfs_client_dir(tnfs_items, "/");
}

static void __interrupt ctrl_handler(void) { /* ignore Ctrl+C and Ctrl+Break */ }

static Config    cfg;
static PanelItem local_items[PANEL_MAX_ITEMS];
static PanelItem tnfs_items[PANEL_MAX_ITEMS];
static TnfsClient client;

int main()
{
    DlgServerResult dlg_result;
    DlgDriveResult  dlg_drive;
    int             local_items_count;
    int             tnfs_items_count = 0;
    int             srv_idx;
    int             connected = 0;
    int             action;
    char            saved_cwd[128];

    if (_getcwd(saved_cwd, sizeof(saved_cwd)) == NULL)
        saved_cwd[0] = '\0';

    /* Hook INT 23h (Ctrl+C) and INT 1Bh (Ctrl+Break) before mTCP starts.
       Without this, an abrupt exit leaves the packet driver ISR dangling
       in memory, causing the machine to freeze on the next incoming packet. */
    _dos_setvect(0x23, ctrl_handler);
    _dos_setvect(0x1B, ctrl_handler);

    log_open("TNFSCMDR.LOG");

    if (cfg_load(&cfg, CFG_FILENAME) != 0) {
        printf("Cannot open %s\n", CFG_FILENAME);
        return 1;
    }

    srv_idx = cfg_find_server(&cfg, cfg.lastserver);
    if (srv_idx < 0 && cfg.server_count > 0)
        srv_idx = 0;

    screen_clear(0x17);

    local_items_count = local_client_dir(local_items);

    if (srv_idx >= 0) {
        connect_server(&cfg, srv_idx, &client, tnfs_items, &tnfs_items_count);
        connected = 1;
    }

    do {
        action = ui_run(local_items, &local_items_count, tnfs_items, tnfs_items_count,
                        connected ? client.server : NULL,
                        connected ? client.mount  : NULL);

        if (action == UI_CHANGE_DRIVE) {
            dlg_drive_select(&dlg_drive);
            if (dlg_drive.action == DLG_DRIVE_SELECT) {
                _chdrive(dlg_drive.drive - 'A' + 1);
                local_items_count = local_client_dir(local_items);
            }
        }

        if (action == UI_CHANGE_SERVER) {
            dlg_server_select(&cfg, &dlg_result, connected ? srv_idx : -1);

            if (dlg_result.action == DLG_ACTION_CONNECT) {
                if (connected)
                    tnfs_client_disconnect();

                srv_idx = dlg_result.index;
                connect_server(&cfg, srv_idx, &client, tnfs_items, &tnfs_items_count);
                connected = 1;
            }
        }
    } while (action != UI_QUIT);

    if (connected)
        tnfs_client_disconnect();

    netw_shutdown();

    if (saved_cwd[0]) {
        _chdrive(saved_cwd[0] - 'A' + 1);
        chdir(saved_cwd);
    }

    log_close();

    return 0;
}
