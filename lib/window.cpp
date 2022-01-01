#include "main.hpp"

typedef struct {
    unsigned short index;
    HWND hwnd[1024];
} EnumWindowsData;

static HWND desktop = nullptr;

static BOOL CALLBACK EnumerateWindowsCallback(HWND hwnd, LPARAM ptr) {
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
        return TRUE;
    }
    
    EnumWindowsData * out = reinterpret_cast<EnumWindowsData *>(ptr);
    
    out->hwnd[out->index] = hwnd;
    out->index++;
    
    return out->index != 1024;
}

static void EnumerateWindows(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    EnumWindowsData data = { 0 };
    EnumWindows(EnumerateWindowsCallback, reinterpret_cast<LPARAM>(&data));
    
    Local<Array> array = Array::New(isolate, data.index);
    
    for (unsigned short i = 0; i < data.index; i++) {
        array->Set(ctx, i, BigInt::NewFromUnsigned(isolate, reinterpret_cast<unsigned __int64>(data.hwnd[i])));
    }
    
    ARG(args, array);
}

static void SetForeground(const FunctionCallbackInfo<Value> & args) {
    Local<Context> ctx = args.GetIsolate()->GetCurrentContext();
    
    HWND input = reinterpret_cast<HWND>(args[0]->ToBigInt(ctx).ToLocalChecked()->Uint64Value());
    if (input == GetForegroundWindow()) {
        return;
    }
    
    SetForegroundWindow(input);
}

static void GetForeground(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    ARG(args, BigInt::New(isolate, reinterpret_cast<unsigned __int64>(GetForegroundWindow())));
}

static void Find(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    
    if (!args.Length()) {
        if (desktop == nullptr) {
            desktop = GetDesktopWindow();
        }
        
        ARG(args, BigInt::New(isolate, reinterpret_cast<unsigned __int64>(desktop)));
        return;
    }
    
    Local<Context> ctx = isolate->GetCurrentContext();
    
    char * str = *(String::Utf8Value(isolate, args[0]));
    ARG(args, BigInt::New(isolate, reinterpret_cast<unsigned __int64>(FindWindowA(nullptr, str))));
}

static void Close(const FunctionCallbackInfo<Value> & args) {
    Local<Context> ctx = args.GetIsolate()->GetCurrentContext();
    PostMessage(reinterpret_cast<HWND>(args[0]->ToBigInt(ctx).ToLocalChecked()->Uint64Value()), WM_CLOSE, 0, 0);
}
/* wip
static void SendKeyboard(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    DWORD processId, thread;
    thread = GetWindowThreadProcessId(reinterpret_cast<HWND>(args[0]->ToBigInt(ctx).ToLocalChecked()->Uint64Value()), &processId);
    
    int length = args[1]->ToString(ctx).ToLocalChecked()->Length();
    const char * str = *(String::Utf8Value(isolate, args[1]));
    
    printf("%d '%s'\n", length, str);
    
    for (int i = 0; i < length; i++) {
        PostThreadMessageA(thread, WM_KEYDOWN, str[i], 1);
    }
}
*/
BINDING_MAIN(exports, module, context) {
    Binding binding(exports, context);
    
    binding.Export("enumerateWindows", EnumerateWindows);
    binding.Export("setForeground",    SetForeground);
    binding.Export("getForeground",    GetForeground);
    binding.Export("find",             Find);
    binding.Export("close",            Close);
    // binding.Export("sendKeyboard",     SendKeyboard);
}