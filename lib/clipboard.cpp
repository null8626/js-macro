#include "main.hpp"

#include <shlobj_core.h>

typedef struct {
    size_t size;
    wchar_t * str;
} ClipboardWideString;

void Open(const FunctionCallbackInfo<Value> & args) {
    OpenClipboard(NULL);
}

void Copy(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    String::Value value(isolate, args[0]);
    const wchar_t * str = (wchar_t *)(*value);
    const int length = value.length();
    
    HGLOBAL glob_str = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(wchar_t) * length);
    ::memcpy(::GlobalLock(glob_str), str, sizeof(wchar_t) * length);
    ::GlobalUnlock(glob_str);
    ::OpenClipboard(NULL);
    
    ::EmptyClipboard();
    ::SetClipboardData(CF_UNICODETEXT, glob_str);
    ::CloseClipboard();
}

void CopyFiles(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    
    const int args_size = args.Length();
    ClipboardWideString * input_arr = new ClipboardWideString[args_size];
    
    size_t hdrop_size = sizeof(DROPFILES) + 1;
    int i;
    
    for (i = 0; i < args_size; i++) {
        String::Value str_value(isolate, args[i]);
        
        input_arr[i].size = str_value.length() * sizeof(wchar_t);
        hdrop_size += input_arr[i].size;
        
        input_arr[i].str = (wchar_t *)(*str_value);
    }
    
    HGLOBAL hdrop = ::GlobalAlloc(GHND, hdrop_size);
    
    unsigned char * ptr = reinterpret_cast<unsigned char *>(::GlobalLock(hdrop));
    DROPFILES * df = reinterpret_cast<DROPFILES *>(ptr);

    df->pFiles = sizeof(DROPFILES);
    df->fWide  = TRUE;
    
    hdrop_size = sizeof(DROPFILES);
    for (i = 0; i < args_size; i++) {
        ::memcpy(ptr + hdrop_size, input_arr[i].str, input_arr[i].size);
        hdrop_size += input_arr[i].size;
    }

    ::GlobalUnlock(hdrop);
    ::OpenClipboard(NULL);
    ::EmptyClipboard();
    ::SetClipboardData(CF_HDROP, hdrop);
    ::CloseClipboard();
    
    delete[] input_arr;
}

static wchar_t * GetFileNameFromPath(wchar_t * str, short i) {
    while (i >= 0 && str[i] != L'\\') {
        i--;
    }
    
    return str + i + 1;
}

static void CombinePaths(wchar_t ** res, unsigned short * size, wchar_t * str, short i, wchar_t * append, unsigned short j) {    
    const unsigned short supposed_size = i + j + 2;
    
    if (*res != NULL && supposed_size > *size) {
        *res = reinterpret_cast<wchar_t *>(::realloc(*res, supposed_size * sizeof(wchar_t)));
    } else if (*res == NULL) {
        *res = reinterpret_cast<wchar_t *>(::malloc(supposed_size * sizeof(wchar_t)));
    }
    
    *size = supposed_size;
    
    wcscpy(*res, str);
    res[0][i] = L'\\';
    wcscpy(*res + i + 1, append);
    res[0][i + j + 1] = 0;
}

static Local<String> v8_GetClipboardFormatName(Isolate * isolate, const UINT format) {
    switch (format) {
        case CF_TEXT:
            return STRING_LITERAL(isolate, "CF_TEXT");
        case CF_BITMAP:
            return STRING_LITERAL(isolate, "CF_BITMAP");
        case CF_METAFILEPICT:
            return STRING_LITERAL(isolate, "CF_METAFILEPICT");
        case CF_SYLK:
            return STRING_LITERAL(isolate, "CF_SYLK");
        case CF_DIF:
            return STRING_LITERAL(isolate, "CF_DIF");
        case CF_TIFF:
            return STRING_LITERAL(isolate, "CF_TIFF");
        case CF_OEMTEXT:
            return STRING_LITERAL(isolate, "CF_OEMTEXT");
        case CF_DIB:
            return STRING_LITERAL(isolate, "CF_DIB");
        case CF_PALETTE:
            return STRING_LITERAL(isolate, "CF_PALETTE");
        case CF_PENDATA:
            return STRING_LITERAL(isolate, "CF_PENDATA");
        case CF_RIFF:
            return STRING_LITERAL(isolate, "CF_RIFF");
        case CF_WAVE:
            return STRING_LITERAL(isolate, "CF_WAVE");
        case CF_UNICODETEXT:
            return STRING_LITERAL(isolate, "CF_UNICODETEXT");
        case CF_ENHMETAFILE:
            return STRING_LITERAL(isolate, "CF_ENHMETAFILE");
        case CF_HDROP:
            return STRING_LITERAL(isolate, "CF_HDROP");
        case CF_LOCALE:
            return STRING_LITERAL(isolate, "CF_LOCALE");
        case CF_DIBV5:
            return STRING_LITERAL(isolate, "CF_DIBV5");
        case CF_DSPTEXT:
            return STRING_LITERAL(isolate, "CF_DSPTEXT");
        case CF_DSPBITMAP:
            return STRING_LITERAL(isolate, "CF_DSPBITMAP");
        case CF_DSPMETAFILEPICT:
            return STRING_LITERAL(isolate, "CF_DSPMETAFILEPICT");
        case CF_DSPENHMETAFILE:
            return STRING_LITERAL(isolate, "CF_DSPENHMETAFILE");
        default: {
            wchar_t string[513];
            unsigned short len = ::GetClipboardFormatNameW(format, string, 512);
        
            return String::NewFromTwoByte(isolate, (uint16_t *)(&string[0]), NewStringType::kNormal, len).ToLocalChecked();
        }
    }
}

void Paste(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> ctx = isolate->GetCurrentContext();
    ::OpenClipboard(NULL);
    
    if (args[0]->IsFunction()) {
        Local<Object> result = Object::New(isolate);
        Local<Object> allocUnsafe = args[0]->ToObject(ctx).ToLocalChecked();
        Local<Object> global = ctx->Global();
        UINT format = 0;
        
        Local<Value> _args[1];
        unsigned char * ptr;
        HANDLE handle;
        uint32_t len;
        
        while (format = ::EnumClipboardFormats(format)) {
            Local<String> str = ::v8_GetClipboardFormatName(isolate, format);
            handle = ::GetClipboardData(format);
            
            if ((ptr = reinterpret_cast<unsigned char *>(::GlobalLock(handle))) == NULL) {
                result->Set(ctx, str, Null(isolate));
            } else {
                len = static_cast<uint32_t>(::GlobalSize(handle));
                
                _args[0] = Number::New(isolate, static_cast<double>(len));
                Local<Object> buffer = allocUnsafe->CallAsFunction(ctx, global, 1, _args).ToLocalChecked()->ToObject(ctx).ToLocalChecked();
                
                len--;
                while (1) {
                    buffer->Set(ctx, len, Number::New(isolate, static_cast<double>(ptr[len])));
                    
                    if (len == 0) {
                        break;
                    }
                    
                    len--;
                }
                
                result->Set(ctx, str, buffer);
            }
            
            ::GlobalUnlock(handle);
        }
        
        ::CloseClipboard();
        ARG(args, result);
        return;
    }
    
    const int type = ARG_INT(args[0], ctx);
    
    HANDLE handle = ::GetClipboardData(type);
    if (handle == NULL) {
        ::CloseClipboard();
        return;
    }
    
    switch (type) {
        case CF_UNICODETEXT: {
            size_t len = 0;
            uint16_t * str = reinterpret_cast<uint16_t *>(::GlobalLock(handle));
            
            while (str[len] != 0) {
                len++;
            }
            
            ARG(args, String::NewFromTwoByte(isolate, str, NewStringType::kNormal, len).ToLocalChecked());
            break;
        }
        
        case CF_HDROP: {
            HDROP hdrop = reinterpret_cast<HDROP>(::GlobalLock(handle));
            
            String::Value value(isolate, args[1]);
            wchar_t * dest_dir = (wchar_t *)(*value);
            const int dest_dir_len = value.length();
            
            wchar_t * src_file_name;
            
            wchar_t * dest_path = NULL;
            unsigned short dest_path_len;
            
            wchar_t src_path[262];
            short src_path_len;
            
            const uint32_t count = ::DragQueryFileW(hdrop, 0xFFFFFFFF, NULL, 0);
            
            for (uint32_t i = 0; i < count; i++) {
                src_file_name = ::GetFileNameFromPath(src_path, (src_path_len = static_cast<short>(::DragQueryFileW(hdrop, i, src_path, 262))));
                
                ::CombinePaths(&dest_path, &dest_path_len, dest_dir, dest_dir_len, src_file_name, &src_path[0] + src_path_len - src_file_name);
                ::CopyFileW(src_path, dest_path, FALSE);
            }
        }
    }
    
    ::GlobalUnlock(handle);
    ::CloseClipboard();
}

void Empty(const FunctionCallbackInfo<Value> & args) {
    ::OpenClipboard(NULL);
    ::EmptyClipboard();
    ::CloseClipboard();
}

BINDING_MAIN(exports, module, context) {
    Binding binding(exports, context);
    ConstantBindingExport(binding, "copy", Copy);
    ConstantBindingExport(binding, "copyFiles", CopyFiles);
    ConstantBindingExport(binding, "paste", Paste);
    ConstantBindingExport(binding, "empty", Empty);
}