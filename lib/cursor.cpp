#include "main.hpp"

typedef struct {
    const int x;
    const int y;
    unsigned short index;
    HWND hwnd[1024];
} EnumWindowsDataHover;

static BOOL CALLBACK EnumerateWindowsCallbackHover(HWND hwnd, LPARAM ptr) {
    EnumWindowsDataHover * out = reinterpret_cast<EnumWindowsDataHover *>(ptr);
    
    RECT rect;
    if (!GetWindowRect(hwnd, &rect)) {
        return TRUE;
    }
    
    else if (out->x > rect.left && out->y > rect.top && out->x < rect.right && out->y < rect.bottom) {
        out->hwnd[out->index] = hwnd;
        out->index++;
    }
    
    return out->index != 1024;
}

static void IsHoveringOn(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    POINT pnt;
    if (!GetCursorPos(&pnt)) {
        pnt.x = 0;
        pnt.y = 0;
    }
    
    EnumWindowsDataHover data = { pnt.x, pnt.y, 0 };
    EnumWindows(EnumerateWindowsCallbackHover, reinterpret_cast<LPARAM>(&data));
 
    Local<Array> array = Array::New(isolate, data.index);
    
    for (unsigned short i = 0; i < data.index; i++) {
        array->Set(ctx, i, BigInt::NewFromUnsigned(isolate, reinterpret_cast<uint64_t>(data.hwnd[i])));
    }
    
    ARG(args, array);
}

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
    
    ConstantBindingExport(binding, "moveCursor",              MoveCursor);
    ConstantBindingExport(binding, "sendCursorEvent",         SendCursorEvent);
    ConstantBindingExport(binding, "getCursorPos",            GetCursorPosition);
    ConstantBindingExport(binding, "isHoveringOn",            IsHoveringOn);
}