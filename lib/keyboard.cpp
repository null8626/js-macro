#include "main.hpp"

typedef struct {
    const WORD vk;
    const char normal;
    const char shifted;
} SpecialCharacters;

static const SpecialCharacters special_characters[] = {
    { 0x30,          '0', ')'  },
    { 0x31,          '1', '!'  },
    { 0x32,          '2', '@'  },
    { 0x33,          '3', '#'  },
    { 0x34,          '4', '$'  },
    { 0x35,          '5', '%'  },
    { 0x36,          '6', '^'  },
    { 0x37,          '7', '&'  },
    { 0x38,          '8', '*'  },
    { 0x39,          '9', '('  },
    { VK_OEM_MINUS,  '-', '_'  },
    { VK_OEM_PLUS,   '=', '+'  }, // what???
    { VK_OEM_COMMA,  ',', '<'  },
    { VK_OEM_PERIOD, '.', '>'  },
    { VK_OEM_1,      ';', ':'  },
    { VK_OEM_2,      '/', '?'  },
    { VK_OEM_3,      '`', '~'  },
    { VK_OEM_4,      '[', '{'  },
    { VK_OEM_5,      '\\', '|' },
    { VK_OEM_6,      ']', '}'  },
    { VK_OEM_7,      '\'', '"' }
};

typedef struct {
    bool capslock;
    bool shift;
} KeyboardSnapshot;

static INPUT keyboard;

static void SingleEvent(const WORD word, const DWORD flags) {
    keyboard.ki.wVk = word;
    keyboard.ki.dwFlags = flags;
    ::SendInput(1, &keyboard, sizeof(INPUT));
}

static void Press(const WORD word) {
    keyboard.ki.wVk = word;
    keyboard.ki.dwFlags = 0;
    ::SendInput(1, &keyboard, sizeof(INPUT));

    keyboard.ki.dwFlags = KEYEVENTF_KEYUP;
    ::SendInput(1, &keyboard, sizeof(INPUT));
}

static void PressChar(char c, KeyboardSnapshot * snapshot) {
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
        const bool is_uppercase = c <= 'Z';
        
        if ((is_uppercase && !snapshot->capslock) || (!is_uppercase && snapshot->capslock)) {
            ::Press(VK_CAPITAL);
            
            snapshot->capslock = !snapshot->capslock;
        }
        
        if (c >= 'a') {
            c -= 32;
        }
        
        return ::Press(c - 'A' + 0x41);
    }
    
    switch (c) {
        case ' ':
            return ::Press(VK_SPACE);
        case '\t':
            return ::Press(VK_TAB);
        case '\n':
            return ::Press(VK_RETURN);
    }
    
    const bool is_shift = (0x8000 & GetAsyncKeyState(VK_SHIFT)) != 0;
    
    for (uint8_t i = 0; i < sizeof(special_characters) / sizeof(SpecialCharacters); i++) {
        if (c != special_characters[i].normal && c != special_characters[i].shifted) {
            continue;
        }
        
        if (c == special_characters[i].normal && snapshot->shift) {
            ::SingleEvent(VK_SHIFT, KEYEVENTF_KEYUP);
            snapshot->shift = false;
        } else if (c == special_characters[i].shifted && !snapshot->shift) {
            ::SingleEvent(VK_SHIFT, 0);
            snapshot->shift = true;
        }
        
        return ::Press(special_characters[i].vk);
    }
}

void KeyboardType(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    if (args[0]->IsNumber()) {
        const int vk   = ARG_INT(args[0], ctx);
        const int type = ARG_INT(args[1], ctx);
        
        if (type == 42069) {
            return ::Press(vk);
        }
        
        return ::SingleEvent(vk, type);
    }
    
    const bool caps_state  = (0x8000 & GetAsyncKeyState(VK_CAPITAL)) != 0;
    const bool shift_state = (0x8000 & GetAsyncKeyState(VK_SHIFT)) != 0;
    KeyboardSnapshot snapshot = { caps_state, shift_state };
    
    const String::Utf8Value value(isolate, args[0]);
    const char * str = *value;
    
    for (int i = 0; i < value.length(); i++) {
        ::PressChar(str[i], &snapshot);
    }
    
    if (snapshot.capslock != caps_state) {
        ::Press(VK_CAPITAL);
    }
    
    if (snapshot.shift != shift_state) {
        if (snapshot.shift) {
            ::SingleEvent(VK_SHIFT, KEYEVENTF_KEYUP);
        } else {
            ::SingleEvent(VK_SHIFT, 0);
        }
    }
}

BINDING_MAIN(exports, module, context) {
    ::memset(&keyboard, 0, sizeof(INPUT));
    keyboard.type = INPUT_KEYBOARD;
    
    Binding binding(exports, context);
    ConstantBindingExport(binding, "type", KeyboardType);
}