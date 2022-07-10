#include "hooks.h"

#define STOP_EVENT (WM_USER + 1)

static HANDLE hHeap;

void InitHooks(void)
{
    hHeap = GetProcessHeap();
}

static DWORD WINAPI InnerHookThreadProc(LPVOID lpvData)
{
    HOOK_ELEMENT *lpElem = (HOOK_ELEMENT *)lpvData;

    lpElem->hHook = SetWindowsHookExW(lpElem->dwHookId, lpElem->lpHookProc, NULL, 0);
    MSG msg;

    while (GetMessageW(&msg, NULL, 0, 0))
    {
        if (msg.message == STOP_EVENT)
        {
            PostQuitMessage(0);
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    UnhookWindowsHookEx(lpElem->hHook);
    return 0;
}

static DWORD WINAPI DispatchThreadProc(LPVOID lpvData)
{
    HOOK_ELEMENT *lpElem = *((HOOK_ELEMENT **)lpvData);

    lpElem->lpfnAsyncCallback(lpElem->lpvData, (LPVOID)(((unsigned char *)lpvData) + sizeof(HOOK_ELEMENT *)));

    HeapFree(hHeap, 0, lpvData);

    return 0;
}

void HookElemStart(HOOK_ELEMENT *lpElem, HookCallback lpfnCallback, HookAsyncCallback lpfnAsyncCallback,
                   HookCleanupCallback lpfnCleanupCallback)
{
    if (lpElem->hThread == NULL)
    {
        lpElem->lpfnCallback = lpfnCallback;
        lpElem->lpfnAsyncCallback = lpfnAsyncCallback;
        lpElem->lpfnCleanupCallback = lpfnCleanupCallback;
        lpElem->hThread = CreateThread(NULL, 0, InnerHookThreadProc, (LPVOID)lpElem, 0, &lpElem->dwThreadId);
    }
}

void HookElemDispatch(HOOK_ELEMENT *lpElem, LPVOID lpData, SIZE_T qwDataSize)
{
    if (!lpElem->lpfnCallback(lpElem->lpvData, lpData))
    {
        return;
    }

    unsigned char *lpDispatchedPointer = (unsigned char *)HeapAlloc(hHeap, 0, sizeof(HOOK_ELEMENT *) + qwDataSize);

    memcpy((LPVOID)lpDispatchedPointer, (LPVOID)&lpElem, sizeof(HOOK_ELEMENT *));
    memcpy((LPVOID)(lpDispatchedPointer + sizeof(HOOK_ELEMENT *)), lpData, qwDataSize);

    CreateThread(NULL, 0, DispatchThreadProc, (LPVOID)lpDispatchedPointer, 0, NULL);
}

void HookElemStop(HOOK_ELEMENT *lpElem)
{
    if (lpElem->hThread == NULL)
    {
        return;
    }

    PostThreadMessageW(lpElem->dwThreadId, STOP_EVENT, 0, 0);
    WaitForSingleObject(lpElem->hThread, INFINITE);
    CloseHandle(lpElem->hThread);

    lpElem->hThread = NULL;
    lpElem->lpfnCleanupCallback(lpElem->lpvData);
    lpElem->lpvData = NULL;
}

void CleanupHooks(void)
{
    MouseHookStop();
    KeyboardHookStop();
}