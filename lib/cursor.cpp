#include "main.hpp"

static void MoveCursor(const FunctionCallbackInfo<Value> & args) {
    Local<Context> ctx = args.GetIsolate()->GetCurrentContext();
    
    SetCursorPos(ARG_INT(args[0], ctx), ARG_INT(args[1], ctx));
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
    
    if (type == 0 || type == 2) {
        mouse_event(down, 0, 0, 0, 0);
    }
    
    if (type >= 1) {
        mouse_event(up, 0, 0, 0, 0);
    }
}

BINDING_MAIN(exports, module, context) {
    Binding binding(exports, context);
    
    binding.Export("moveCursor",      MoveCursor);
    binding.Export("sendCursorEvent", SendCursorEvent);
}