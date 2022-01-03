#include "main.hpp"

typedef struct {
    unsigned short index;
    HWND hwnd[1024];
} EnumWindowsData;

static HWND desktop = nullptr;

static BOOL CALLBACK EnumerateWindowsCallback(HWND hwnd, LPARAM ptr) {
    if (!IsWindow(hwnd)) {
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
    
    if (!args.Length()) {
        EnumWindows(EnumerateWindowsCallback, reinterpret_cast<LPARAM>(&data));
    } else {
        EnumChildWindows(reinterpret_cast<HWND>(args[1]->ToBigInt(ctx).ToLocalChecked()->Uint64Value()),
                         EnumerateWindowsCallback, reinterpret_cast<LPARAM>(&data));
    }
    
    Local<Array> array = Array::New(isolate, data.index);
    
    for (unsigned short i = 0; i < data.index; i++) {
        array->Set(ctx, i, BigInt::NewFromUnsigned(isolate, reinterpret_cast<uint64_t>(data.hwnd[i])));
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
    ARG(args, BigInt::New(isolate, reinterpret_cast<uint64_t>(GetForegroundWindow())));
}

static void Find(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    
    if (!args.Length()) {
        if (desktop == nullptr) {
            desktop = GetDesktopWindow();
        }
        
        ARG(args, BigInt::New(isolate, reinterpret_cast<uint64_t>(desktop)));
        return;
    }
    
    Local<Context> ctx = isolate->GetCurrentContext();
    
    char * str = *(String::Utf8Value(isolate, args[0]));
    ARG(args, BigInt::New(isolate, reinterpret_cast<uint64_t>(FindWindowA(nullptr, str))));
}

static void Console(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    
    ARG(args, BigInt::New(isolate, reinterpret_cast<uint64_t>(GetConsoleWindow())));
}

static void Close(const FunctionCallbackInfo<Value> & args) {
    Local<Context> ctx = args.GetIsolate()->GetCurrentContext();
    PostMessage(reinterpret_cast<HWND>(args[0]->ToBigInt(ctx).ToLocalChecked()->Uint64Value()), WM_CLOSE, 0, 0);
}

static void SendKeyboard(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    HWND hwnd = reinterpret_cast<HWND>(args[0]->ToBigInt(ctx).ToLocalChecked()->Uint64Value());
    
    int length = args[1]->ToString(ctx).ToLocalChecked()->Length();
    const char * str = *(String::Utf8Value(isolate, args[1]));
    
    for (int i = 0; i < length; i++) {
        PostMessageA(hwnd, WM_CHAR, static_cast<WPARAM>(str[i]), (LPARAM)1);
    }
}

static void GetClass(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    HWND hwnd = reinterpret_cast<HWND>(args[0]->ToBigInt(ctx).ToLocalChecked()->Uint64Value());
    
    uint16_t ptr[1024];
    
    int size = GetWindowTextW(hwnd, (wchar_t *)(&ptr[0]), 1024);
    ptr[size] = 0;
    
    ARG(args, String::NewFromTwoByte(isolate, const_cast<const uint16_t *>(&ptr[0]), NewStringType
::kNormal, size).ToLocalChecked());
}

static void GetTitle(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    HWND hwnd = reinterpret_cast<HWND>(args[0]->ToBigInt(ctx).ToLocalChecked()->Uint64Value());
    
    int length = GetWindowTextLengthW(hwnd) + 1;
    
    if (!length) {
        ARG(args, String::Empty(isolate));
        return;
    }
    
    uint16_t * ptr = new uint16_t[length];
    ptr[length - 1] = 0;
    
    GetWindowTextW(hwnd, (wchar_t *)(&ptr[0]), length);
    
    ARG(args, String::NewFromTwoByte(isolate, const_cast<const uint16_t *>(ptr), NewStringType
::kNormal, length - 1).ToLocalChecked());
    
    delete[] ptr;
}

BINDING_MAIN(exports, module, context) {
    Binding binding(exports, context);
    
    binding.Export("enumerateWindows", EnumerateWindows);
    binding.Export("setForeground",    SetForeground);
    binding.Export("getForeground",    GetForeground);
    binding.Export("find",             Find);
    binding.Export("close",            Close);
    binding.Export("sendKeyboard",     SendKeyboard);
    binding.Export("getTitle",         GetTitle);
    binding.Export("getClass",         GetClass);
    binding.Export("console",          Console);
}