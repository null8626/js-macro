#include "main.hpp"

static void MoveCursor(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    SetCursorPos(ARG_INT(args[0], ctx), ARG_INT(args[1], ctx));
}

static void SendCursorEvent(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
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

static void GetCursorPosition(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    POINT pnt;
    
    if (!GetCursorPos(&pnt)) {
        pnt.x = 0;
        pnt.y = 0;
    }
    
    Local<Object> output = Object::New(isolate);

    Local<String> xKey = STRING(isolate, "x", 1);
    Local<String> yKey = STRING(isolate, "y", 1);
    
    output->Set(ctx, xKey, NUMBER(isolate, pnt.x));
    output->Set(ctx, yKey, NUMBER(isolate, pnt.y));
    
    ARG(args, output);
}

BINDING_MAIN(exports, module, context) {
    Binding binding(exports, context);
    
    binding.Export("moveCursor",      MoveCursor);
    binding.Export("sendCursorEvent", SendCursorEvent);
    binding.Export("getCursorPos",    GetCursorPosition);
}