#include "mouse.h"

static LRESULT CALLBACK InnerMouseHookCallback(int code, WPARAM wParam, LPARAM lParam);

static HOOK_ELEMENT Mouse = HOOK_ELEMENT_NEW(InnerMouseHookCallback, WH_MOUSE_LL);

static LRESULT CALLBACK InnerMouseHookCallback(int code, WPARAM wParam, LPARAM lParam)
{
    if (code == HC_ACTION && (wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP || wParam == WM_MBUTTONUP))
    {
        MSLLHOOKSTRUCT *lpData = (MSLLHOOKSTRUCT *)lParam;
        MOUSE_DATA mData = {lpData->pt, wParam};

        HookElemDispatch(&Mouse, (LPVOID)&mData, sizeof(MOUSE_DATA));
    }

    return CallNextHookEx(Mouse.hHook, code, wParam, lParam);
}

void MouseHookStart(HookCallback lpfnCallback, HookAsyncCallback lpfnAsyncCallback,
                    HookCleanupCallback lpfnCleanupCallback)
{
    HookElemStart(&Mouse, lpfnCallback, lpfnAsyncCallback, lpfnCleanupCallback);
}

LPVOID *MouseHookData(void)
{
    if (Mouse.hThread == NULL)
    {
        return &Mouse.lpvData;
    }
    else
    {
        return NULL;
    }
}

void MouseHookStop(void)
{
    HookElemStop(&Mouse);
}