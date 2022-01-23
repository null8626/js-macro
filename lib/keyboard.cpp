#include "main.hpp"

void KeyboardType(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    if (args[0]->IsNumber()) {
        const int vk   = ARG_INT(args[0], ctx);
        const int type = ARG_INT(args[1], ctx);
        
        if (type == 42069) {
            INPUT keyboard[2];
            memset(&keyboard, 0, 2 * sizeof(INPUT));
        
            keyboard[0].type = INPUT_KEYBOARD;
            keyboard[0].ki.wVk = vk;
            
            keyboard[1].type = INPUT_KEYBOARD;
            keyboard[1].ki.wVk = vk;
            keyboard[1].ki.dwFlags = KEYEVENTF_KEYUP;
            
            ::SendInput(2, &keyboard[0], sizeof(INPUT));
            return;
        }
        
        INPUT keyboard;
        memset(&keyboard, 0, sizeof(INPUT));
    
        keyboard.type = INPUT_KEYBOARD;
        keyboard.ki.wVk = vk;
        keyboard.ki.dwFlags = type;
        
        ::SendInput(1, &keyboard, sizeof(INPUT));
        return;
    }
    
    const String::Value value(isolate, args[0]);
    const wchar_t * str = (wchar_t *)(*value);
    const int len = value.length();
    
    INPUT * inp = new INPUT[len];
    ::memset(inp, 0, sizeof(INPUT) * len);

    for (int i = 0; i < len; i++) {
        inp[i].type = INPUT_KEYBOARD;
        inp[i].ki.dwFlags = KEYEVENTF_UNICODE;
        inp[i].ki.wScan = str[i];
    }

    ::SendInput(len, inp, sizeof(INPUT));
    delete[] inp;
}

BINDING_MAIN(exports, module, context) {
    Binding binding(exports, context);
    ConstantBindingExport(binding, "type", KeyboardType);
}