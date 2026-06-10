#ifndef TNFS_CLIENT_H
#define TNFS_CLIENT_H

#include <stdint.h>
#include "config.h"
#include "panel_item.h"

typedef struct {
    uint8_t connected;
    uint8_t use_tcp;
    char server[MAX_ARGV_LEN];
    int port;
    char mount[MAX_ARGV_LEN];
} TnfsClient;

int  tnfs_client_connect(TnfsClient *client);
void tnfs_client_disconnect();
int  tnfs_client_dir(PanelItem *items, const char *path);

#endif