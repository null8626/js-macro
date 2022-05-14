#include "main.hpp"

#include <cstdlib>
#include <cstdio>

using namespace node;

Binding::Binding(Local<Object> _exports, Local<Context> _context):
  isolate(Isolate::GetCurrent()), exports(_exports), handlescope(HandleScope(isolate)), context(_context) { }

void Binding::Export(const char * name, FunctionCallback callback, const size_t size) {
  ctxFuncTemplate = FunctionTemplate::New(isolate, callback);
  ctxFunc = ctxFuncTemplate->GetFunction(context).ToLocalChecked();
  
  funcname = String::NewFromUtf8(isolate, name, NewStringType::kInternalized, size - 1).ToLocalChecked();
  ctxFunc->SetName(funcname);
  exports->Set(context, funcname, ctxFunc);
}