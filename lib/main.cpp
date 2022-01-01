#include "main.hpp"

Binding::Binding(Local<Object> _exports, Local<Context> _context):
    isolate(Isolate::GetCurrent()),
    exports(_exports),
    handlescope(HandleScope(isolate)),
    context(_context) { }

void Binding::Export(const char * name, FunctionCallback callback) {
    ctxFuncTemplate = FunctionTemplate::New(isolate, callback);
    ctxFunc = ctxFuncTemplate->GetFunction(context).ToLocalChecked();
    
    funcname = String::NewFromUtf8(isolate, name, NewStringType::kInternalized).ToLocalChecked();
    ctxFunc->SetName(funcname);
    exports->Set(context, funcname, ctxFunc);
}