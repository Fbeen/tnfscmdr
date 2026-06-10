#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <direct.h>
#include "ui.h"
#include "screen.h"
#include "input.h"
#include "panel.h"
#include "local_client.h"
#include "tnfs_client.h"
#include "config.h"


#define COL_BG      0x17
#define COL_HEADER  0x1F
#define COL_STATUS  0x70

static void draw_layout(Panel *left, Panel *right)
{
    screen_clear(COL_BG);

    screen_fill(0, 0, 80, 1, ' ', COL_HEADER);
    screen_text(2, 0, "TNFS DOS Client", COL_HEADER);

    panel_draw(left);
    panel_draw(right);

    screen_fill(0, 24, 80, 1, ' ', COL_STATUS);
    screen_text(2, 24, "Tab Switch  Arrows Move  Enter Open  D Drive  S Server  Esc Quit", COL_STATUS);
}

static void build_titles(char *left_title, int lt_size,
                         char *right_title, int rt_size,
                         const char *remote_server, const char *remote_mount)
{
    char srv[34], mnt[34];
    int  i;

    /* Left: " LOCAL X: " */
    snprintf(left_title, lt_size, " LOCAL %c: ", 'A' + _getdrive() - 1);

    /* Right: uppercase server, and mount only if it fits within 36 chars */
    if (remote_server && remote_server[0]) {
        for (i = 0; i < 33 && remote_server[i]; i++)
            srv[i] = (char)toupper((unsigned char)remote_server[i]);
        srv[i] = '\0';
        for (i = 0; i < 33 && remote_mount && remote_mount[i]; i++)
            mnt[i] = (char)toupper((unsigned char)remote_mount[i]);
        mnt[i] = '\0';

        if ((int)(strlen(srv) + strlen(mnt) + 5) <= 36)
            snprintf(right_title, rt_size, " %s - %s ", srv, mnt);
        else
            snprintf(right_title, rt_size, " %s ", srv);
    } else {
        snprintf(right_title, rt_size, " REMOTE ");
    }
}

static void path_navigate(char *path, int maxlen, const char *dirname)
{
    int plen;
    char *last;

    if (strcmp(dirname, "..") == 0) {
        last = strrchr(path, '/');
        if (last && last != path)
            *last = '\0';
        else { path[0] = '/'; path[1] = '\0'; }
    } else {
        plen = strlen(path);
        if (path[plen - 1] != '/' && plen + 1 < maxlen) {
            path[plen] = '/'; path[plen + 1] = '\0';
        }
        strncat(path, dirname, maxlen - strlen(path) - 1);
    }
}

int ui_run(PanelItem *local_items, int *local_items_count,
           PanelItem *tnfs_items,  int tnfs_items_count,
           const char *remote_server, const char *remote_mount)
{
    Panel left;
    Panel right;
    Panel *active;
    InputEvent ev;
    int shift;
    int ctrl;
    int rc_left;
    int rc_right;
    uint8_t running = 1;
    int     return_code = UI_QUIT;
    char    left_title[16];
    char    right_title[40];
    char    local_dir[68];
    char    tnfs_dir[MAX_PATH_LEN];

    build_titles(left_title, sizeof(left_title),
                 right_title, sizeof(right_title),
                 remote_server, remote_mount);

    if (_getcwd(local_dir, sizeof(local_dir)) == NULL)
        local_dir[0] = '\0';

    if (remote_server && remote_server[0])
        strcpy(tnfs_dir, "/");
    else
        tnfs_dir[0] = '\0';

    panel_init(&left,  0,  2, 40, 21, left_title,  local_dir,
               local_items, *local_items_count);
    panel_init(&right, 40, 2, 40, 21, right_title,
               tnfs_dir[0] ? tnfs_dir : NULL,
               tnfs_items, tnfs_items_count);

    rc_left = panel_refresh(&left);
    rc_right = panel_refresh(&right);

    left.active = 0;
    right.active = 1;
    active = &right;

    draw_layout(&left, &right);

    while (running) {
        ev = input_read_event();

        shift = (ev.modifiers & MOD_SHIFT) != 0;
        ctrl  = (ev.modifiers & MOD_CTRL) != 0;

        switch (ev.key) {
            case KEY_ESC:
                running = 0;
                break;

            case 'S':
            case 's':
                return_code = UI_CHANGE_SERVER;
                running = 0;
                break;

            case 'D':
            case 'd':
                return_code = UI_CHANGE_DRIVE;
                running = 0;
                break;

            case KEY_ENTER:
                if (active == &left) {
                    int idx = left.cursor;
                    if (idx >= 0 && idx < left.items_count
                            && left.items[idx].is_dir) {
                        if (chdir(left.items[idx].name) == 0) {
                            *local_items_count = local_client_dir(local_items);
                            if (_getcwd(local_dir, sizeof(local_dir)) == NULL)
                                local_dir[0] = '\0';
                            build_titles(left_title, sizeof(left_title),
                                         right_title, sizeof(right_title),
                                         remote_server, remote_mount);
                            panel_init(&left, 0, 2, 40, 21, left_title,
                                       local_dir, local_items,
                                       *local_items_count);
                            left.active = 1;
                            panel_refresh(&left);
                            draw_layout(&left, &right);
                        }
                    }
                } else if (active == &right && remote_server && remote_server[0]) {
                    int idx = right.cursor;
                    if (idx >= 0 && idx < right.items_count
                            && right.items[idx].is_dir) {
                        char new_dir[MAX_PATH_LEN];
                        int  new_count;
                        strcpy(new_dir, tnfs_dir);
                        path_navigate(new_dir, sizeof(new_dir),
                                      right.items[idx].name);
                        new_count = tnfs_client_dir(tnfs_items, new_dir);
                        if (new_count >= 0) {
                            strcpy(tnfs_dir, new_dir);
                            panel_init(&right, 40, 2, 40, 21, right_title,
                                       tnfs_dir, tnfs_items, new_count);
                            right.active = 1;
                            panel_refresh(&right);
                            draw_layout(&left, &right);
                        }
                    }
                }
                break;

            case KEY_TAB:
                if (active == &left) {
                    left.active = 0;
                    right.active = 1;
                    active = &right;
                } else {
                    right.active = 0;
                    left.active = 1;
                    active = &left;
                }

                panel_draw(&left);
                panel_draw(&right);
                break;

            case KEY_UP:
                panel_move_active(active, -1, shift, ctrl);
                panel_draw_items(active);
                break;

            case KEY_DOWN:
                panel_move_active(active, 1, shift, ctrl);
                panel_draw_items(active);
                break;

            case KEY_SPACE:
                panel_space(active, shift, ctrl);
                panel_draw_items(active);
                break;

            default:
                break;
        }
    }

    screen_clear(0x07);

    return return_code;
}