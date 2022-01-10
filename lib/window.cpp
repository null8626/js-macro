#include "screenshot.hpp"
#include "main.hpp"

typedef struct {
    unsigned short index;
    HWND hwnd[1024];
} EnumWindowsData;

typedef struct {
    const char * input;
    const unsigned short input_size;
    unsigned short index;
    HWND hwnd[1024];
} EnumWindowsFilePathData;

static HWND desktop = nullptr;

static void GetWindowStyles(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    HWND hwnd = reinterpret_cast<HWND>(args[0]->ToBigInt(ctx).ToLocalChecked()->Uint64Value());
    
    LONG style   = GetWindowLongPtrA(hwnd, GWL_STYLE);
    LONG exStyle = GetWindowLongPtrA(hwnd, GWL_EXSTYLE);
    
    Local<Array> array = Array::New(isolate, 2);
   
    array->Set(ctx, 0, Number::New(isolate, static_cast<double>(style)));
    array->Set(ctx, 1, Number::New(isolate, static_cast<double>(exStyle)));
    
    ARG(args, array);
}

static bool filepathcmp(char * a, DWORD size_a, const char * b, const unsigned short size_b) {
    if (size_b > size_a) {
        return false;
    }
    
    unsigned short size_b_copy = size_b - 1;
    unsigned short matches = 0;
    size_a--;
    
    do {
        if (a[size_a] == b[size_b_copy]) {
            matches++;
        } else {
            return false;
        }
        
        size_a--;
        size_b_copy--;
    } while (size_b && size_a && a[size_a] != '\\');

    return matches == size_b;
}

static BOOL CALLBACK EnumWindowsFilePathCallback(HWND hwnd, LPARAM ptr) {
    EnumWindowsFilePathData * out = reinterpret_cast<EnumWindowsFilePathData *>(ptr);
    
    DWORD processId, size;
    GetWindowThreadProcessId(hwnd, &processId);
    size = 261;
    
    char path[261];
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (!QueryFullProcessImageNameA(process, 0, path, &size)) {
        CloseHandle(process);
        return TRUE;
    }

    CloseHandle(process);    
    if (filepathcmp(path, size, out->input, out->input_size)) {
        out->hwnd[out->index] = hwnd;
        out->index++;
    }
    
    return out->index != 1024;
}

static void GetHwndFromPath(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    const String::Utf8Value stringValue(isolate, args[0]);
    
    EnumWindowsFilePathData data = { *stringValue, static_cast<unsigned short>(stringValue.length()), 0 };    
    EnumWindows(EnumWindowsFilePathCallback, reinterpret_cast<LPARAM>(&data));

    Local<Array> array = Array::New(isolate, data.index);
    
    for (unsigned short i = 0; i < data.index; i++) {
        array->Set(ctx, i, BigInt::NewFromUnsigned(isolate, reinterpret_cast<uint64_t>(data.hwnd[i])));
    }
    
    ARG(args, array);
}

static BOOL CALLBACK EnumerateWindowsCallback(HWND hwnd, LPARAM ptr) {
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
        EnumChildWindows(reinterpret_cast<HWND>(args[0]->ToBigInt(ctx).ToLocalChecked()->Uint64Value()),
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

static void GetDesktop(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    
    ARG(args, BigInt::New(isolate, reinterpret_cast<uint64_t>(GetDesktopWindow())));
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
    
    int size = GetClassNameW(hwnd, (wchar_t *)(&ptr[0]), 1024);
    if (size < 1) {
        ARG(args, String::Empty(isolate));
        return;
    }
    
    ptr[size] = 0;
    
    ARG(args, String::NewFromTwoByte(isolate, const_cast<const uint16_t *>(&ptr[0]), NewStringType
::kNormal, size).ToLocalChecked());
}

static void GetTitle(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    HWND hwnd = reinterpret_cast<HWND>(args[0]->ToBigInt(ctx).ToLocalChecked()->Uint64Value());
    
    int length = GetWindowTextLengthW(hwnd) + 1;
    
    if (length <= 1) {
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

static void WindowBoundaries(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    HWND hwnd = reinterpret_cast<HWND>(args[0]->ToBigInt(ctx).ToLocalChecked()->Uint64Value());
    
    RECT rect;
    GetWindowRect(hwnd, &rect);

    Local<Object> object = Object::New(isolate);
    object->Set(ctx, STRING(isolate, "width", 5),  NUMBER(isolate, rect.right - rect.left));
    object->Set(ctx, STRING(isolate, "height", 6), NUMBER(isolate, rect.bottom - rect.top));
    
    ARG(args, object);
}

static void SetWindowPosition(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    HWND hwnd = reinterpret_cast<HWND>(args[0]->ToBigInt(ctx).ToLocalChecked()->Uint64Value());
    
    int x      = ARG_INT(args[1], ctx);
    int y      = ARG_INT(args[2], ctx);
    int width  = ARG_INT(args[3], ctx);
    int height = ARG_INT(args[4], ctx);
    
    RECT rect;
    GetWindowRect(hwnd, &rect);
    
    if (x == 0) {
        x = rect.left;
        y = rect.top;
    }
    
    if (width == 0) {
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }
    
    MoveWindow(hwnd, x, y, width, height, TRUE);
}

static void ScreenshotWindow(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    HWND hwnd = reinterpret_cast<HWND>(args[0]->ToBigInt(ctx).ToLocalChecked()->Uint64Value());
    
    const int x      = ARG_INT(args[1], ctx);
    const int y      = ARG_INT(args[2], ctx);
    const int width  = ARG_INT(args[3], ctx);
    const int height = ARG_INT(args[4], ctx);
    
    Screenshot sc(x, y, width, height);

    if (args.Length() == 6) {
        const char * file = *(String::Utf8Value(isolate, args[5]));
        sc.save(file);
    } else {
        ScreenshotBuffer buf = { nullptr, 0 };
        sc.buffer(&buf);
        
        Local<Array> arr = Array::New(isolate, buf.size);
        
        // why node why
        for (size_t i = 0; i < buf.size; i++) {
            arr->Set(ctx, i, NUMBER(isolate, buf.buffer[i]));
        }
        
        ARG(args, arr);
        delete[] buf.buffer;
    }
}

BINDING_MAIN(exports, module, context) {
    Binding binding(exports, context);
    
    ConstantBindingExport(binding, "enumerateWindows", EnumerateWindows);
    ConstantBindingExport(binding, "setForeground",    SetForeground);
    ConstantBindingExport(binding, "getForeground",    GetForeground);
    ConstantBindingExport(binding, "getDesktop",       GetDesktop);
    ConstantBindingExport(binding, "close",            Close);
    ConstantBindingExport(binding, "sendKeyboard",     SendKeyboard);
    ConstantBindingExport(binding, "getTitle",         GetTitle);
    ConstantBindingExport(binding, "getClass",         GetClass);
    ConstantBindingExport(binding, "console",          Console);
    ConstantBindingExport(binding, "boundaries",       WindowBoundaries);
    ConstantBindingExport(binding, "screenshot",       ScreenshotWindow);
    ConstantBindingExport(binding, "setWindowPos",     SetWindowPosition);
    ConstantBindingExport(binding, "getHwndFromPath",  GetHwndFromPath);
    ConstantBindingExport(binding, "getWindowStyles",  GetWindowStyles);
}