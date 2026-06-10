#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cfg.h"

#define LINE_MAX 128

static void trim(char *s)
{
    int len;
    int i;

    len = strlen(s);
    while (len > 0 && (s[len-1] == '\r' || s[len-1] == '\n' || s[len-1] == ' '))
        s[--len] = '\0';

    i = 0;
    while (s[i] == ' ') i++;
    if (i > 0) memmove(s, s + i, len - i + 1);
}

static void parse_key_value(const char *line, char *key, char *value)
{
    const char *eq;

    eq = strchr(line, '=');
    if (eq == NULL) {
        key[0] = '\0';
        value[0] = '\0';
        return;
    }

    strncpy(key, line, eq - line);
    key[eq - line] = '\0';
    trim(key);

    strncpy(value, eq + 1, LINE_MAX - 1);
    value[LINE_MAX - 1] = '\0';
    trim(value);
}

int cfg_load(Config *cfg, const char *filename)
{
    FILE   *f;
    char    line[LINE_MAX];
    char    key[LINE_MAX];
    char    value[LINE_MAX];
    char    section[CFG_MAX_NAME_LEN];
    int     in_misc;
    CfgServer *cur;

    memset(cfg, 0, sizeof(Config));

    f = fopen(filename, "r");
    if (f == NULL) {
        return -1;
    }

    section[0] = '\0';
    in_misc = 0;
    cur = NULL;

    while (fgets(line, sizeof(line), f) != NULL) {
        trim(line);

        if (line[0] == '\0' || line[0] == ';' || line[0] == '#') {
            continue;
        }

        if (line[0] == '[') {
            int len = strlen(line);
            if (line[len-1] == ']') {
                strncpy(section, line + 1, CFG_MAX_NAME_LEN - 1);
                section[CFG_MAX_NAME_LEN - 1] = '\0';
                section[strlen(section) - 1] = '\0';

                if (strcmp(section, "misc") == 0) {
                    in_misc = 1;
                    cur = NULL;
                } else {
                    in_misc = 0;
                    if (cfg->server_count < CFG_MAX_SERVERS) {
                        cur = &cfg->servers[cfg->server_count];
                        memset(cur, 0, sizeof(CfgServer));
                        strncpy(cur->name, section, CFG_MAX_NAME_LEN - 1);
                        cur->port = 16384;
                        cur->use_tcp = 0;
                        cfg->server_count++;
                    } else {
                        cur = NULL;
                    }
                }
            }
            continue;
        }

        parse_key_value(line, key, value);
        if (key[0] == '\0') continue;

        if (in_misc) {
            if (strcmp(key, "lastserver") == 0) {
                strncpy(cfg->lastserver, value, CFG_MAX_NAME_LEN - 1);
            }
        } else if (cur != NULL) {
            if (strcmp(key, "server") == 0) {
                strncpy(cur->server, value, CFG_MAX_SERVER_LEN - 1);
            } else if (strcmp(key, "port") == 0) {
                cur->port = atoi(value);
            } else if (strcmp(key, "mount") == 0) {
                strncpy(cur->mount, value, CFG_MAX_MOUNT_LEN - 1);
            } else if (strcmp(key, "protocol") == 0) {
                cur->use_tcp = (strcmp(value, "TCP") == 0) ? 1 : 0;
            }
        }
    }

    fclose(f);
    return 0;
}

int cfg_find_server(Config *cfg, const char *name)
{
    int i;

    for (i = 0; i < cfg->server_count; i++) {
        if (strcmp(cfg->servers[i].name, name) == 0) {
            return i;
        }
    }

    return -1;
}
