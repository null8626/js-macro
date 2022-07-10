#pragma once

#include <stdbool.h>
#include <windows.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef bool (*HookCallback)(LPVOID, LPVOID);
    typedef void (*HookAsyncCallback)(LPVOID, LPVOID);
    typedef void (*HookCleanupCallback)(LPVOID);

#include "keyboard.h"
#include "mouse.h"

    typedef struct HOOK_ELEMENT
    {
        HANDLE hThread;
        const HOOKPROC lpHookProc;
        const int dwHookId;
        LPVOID lpvData;
        DWORD dwThreadId;
        HHOOK hHook;
        HookCallback lpfnCallback;
        HookAsyncCallback lpfnAsyncCallback;
        HookCleanupCallback lpfnCleanupCallback;
    } HOOK_ELEMENT;

#define HOOK_ELEMENT_NEW(hookProc, hookId)                                                                             \
    {                                                                                                                  \
        NULL, hookProc, hookId, NULL                                                                                   \
    }

    void HookElemStart(HOOK_ELEMENT *lpElem, HookCallback lpfnCallback, HookAsyncCallback lpfnAsyncCallback,
                       HookCleanupCallback lpfnCleanupCallback);

    void HookElemDispatch(HOOK_ELEMENT *lpElem, LPVOID lpData, SIZE_T qwDataSize);
    void HookElemStop(HOOK_ELEMENT *lpElem);

    void InitHooks();
    void CleanupHooks();

#ifdef __cplusplus
}
#endif