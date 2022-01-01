#include "main.hpp"

static HWND desktop = NULL;

static void MoveCursor(const FunctionCallbackInfo<Value> & args) {
    Local<Context> ctx = args.GetIsolate()->GetCurrentContext();
    
    if (desktop == NULL) {
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
    
    const unsigned char type = static_cast<unsigned char>(ARG_INT(args[1], ctx));
    
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
    
    if (type == 0) {
        mouse_event(down, 0, 0, 0, 0);
    }
    
    if (type == 1) {
        mouse_event(up, 0, 0, 0, 0);
    }
}

BINDING_MAIN(exports, module, context) {
    Binding binding(exports, context);
    
    binding.Export("moveCursor", MoveCursor);
    binding.Export("sendCursorEvent", SendCursorEvent);
}