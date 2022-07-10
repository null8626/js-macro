#pragma once

#include "hooks.h"

typedef struct KEYBOARD_DATA
{
    DWORD dwCode;
} KEYBOARD_DATA;

void KeyboardHookStart(HookCallback lpfnCallback, HookAsyncCallback lpfnAsyncCallback,
                       HookCleanupCallback lpfnCleanupCallback);

LPVOID *KeyboardHookData(void);
void KeyboardHookStop(void);