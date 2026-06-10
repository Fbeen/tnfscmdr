#ifndef TNFS_CFG_H
#define TNFS_CFG_H

#include <stdint.h>

#define CFG_FILENAME        "TNFSCMDR.CFG"

#define CFG_MAX_SERVERS     8
#define CFG_MAX_NAME_LEN    16
#define CFG_MAX_SERVER_LEN  64
#define CFG_MAX_MOUNT_LEN   64

typedef struct {
    char     name[CFG_MAX_NAME_LEN];
    char     server[CFG_MAX_SERVER_LEN];
    int      port;
    char     mount[CFG_MAX_MOUNT_LEN];
    uint8_t  use_tcp;
} CfgServer;

typedef struct {
    char      lastserver[CFG_MAX_NAME_LEN];
    CfgServer servers[CFG_MAX_SERVERS];
    int       server_count;
} Config;

int cfg_load(Config *cfg, const char *filename);
int cfg_find_server(Config *cfg, const char *name);

#endif /* TNFS_CFG_H */
