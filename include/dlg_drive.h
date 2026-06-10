#ifndef DLG_DRIVE_H
#define DLG_DRIVE_H

#define DLG_DRIVE_SELECT  0
#define DLG_DRIVE_CANCEL  1

typedef struct {
    int  action;
    char drive;
} DlgDriveResult;

void dlg_drive_select(DlgDriveResult *result);

#endif
