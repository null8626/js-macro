#include "keyboard.h"

static LRESULT CALLBACK InnerKeyboardHookCallback(int code, WPARAM wParam, LPARAM lParam);

static HOOK_ELEMENT Keyboard = HOOK_ELEMENT_NEW(InnerKeyboardHookCallback, WH_KEYBOARD_LL);

static LRESULT CALLBACK InnerKeyboardHookCallback(int code, WPARAM wParam, LPARAM lParam)
{
    if (code == HC_ACTION && (wParam == WM_KEYUP || wParam == WM_SYSKEYUP))
    {
        KEYBOARD_DATA kbData = {((KBDLLHOOKSTRUCT *)lParam)->vkCode};

        HookElemDispatch(&Keyboard, (LPVOID)&kbData, sizeof(KEYBOARD_DATA));
    }

    return CallNextHookEx(Keyboard.hHook, code, wParam, lParam);
}

void KeyboardHookStart(HookCallback lpfnCallback, HookAsyncCallback lpfnAsyncCallback,
                       HookCleanupCallback lpfnCleanupCallback)
{
    HookElemStart(&Keyboard, lpfnCallback, lpfnAsyncCallback, lpfnCleanupCallback);
}

LPVOID *KeyboardHookData(void)
{
    if (Keyboard.hThread == NULL)
    {
        return &Keyboard.lpvData;
    }
    else
    {
        return NULL;
    }
}

void KeyboardHookStop(void)
{
    HookElemStop(&Keyboard);
}