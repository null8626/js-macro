#pragma once

#include "hooks.h"

typedef struct MOUSE_DATA
{
    POINT pt;
    WPARAM wParam;
} MOUSE_DATA;

void MouseHookStart(HookCallback lpfnCallback, HookAsyncCallback lpfnAsyncCallback,
                    HookCleanupCallback lpfnCleanupCallback);

LPVOID *MouseHookData(void);
void MouseHookStop(void);