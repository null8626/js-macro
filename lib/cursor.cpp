#include "main.hpp"

static HWND desktop = nullptr;

static void MoveCursor(const FunctionCallbackInfo<Value> & args) {
    Local<Context> ctx = args.GetIsolate()->GetCurrentContext();
    
    if (desktop == nullptr) {
        desktop = GetDesktopWindow();
    }
    
    const int x = ARG_INT(args[0], ctx);
    const int y = ARG_INT(args[1], ctx);
    
    POINT point = { x, y };
    
    ClientToScreen(desktop, &point);
    SetCursorPos(x, y);
}

static void SendCursorEvent(const FunctionCallbackInfo<Value> & args) {
    Local<Context> ctx = args.GetIsolate()->GetCurrentContext();
    DWORD down, up;
    
    switch (ARG_INT(args[0], ctx)) {
        case 0: {
            down = MOUSEEVENTF_LEFTDOWN;
            up   = MOUSEEVENTF_LEFTUP;
            break;
        }
        
        case 1: {
            down = MOUSEEVENTF_RIGHTDOWN;
            up   = MOUSEEVENTF_RIGHTUP;
            break;
        }
        
        default: {
            down = MOUSEEVENTF_MIDDLEDOWN;
            up   = MOUSEEVENTF_MIDDLEUP;
        }
    }
    
    const int type = ARG_INT(args[1], ctx);
    
    if (type == 0) {
        mouse_event(down, 0, 0, 0, 0);
    }
    
    if (type == 1) {
        mouse_event(up, 0, 0, 0, 0);
    }
}

BINDING_MAIN(exports, module, context) {
    Binding binding(exports, context);
    
    binding.Export("moveCursor",      MoveCursor);
    binding.Export("sendCursorEvent", SendCursorEvent);
}