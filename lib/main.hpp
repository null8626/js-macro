#ifndef MAIN_HPP
#define MAIN_HPP

#include <windows.h>
#include <node_buffer.h>
#include <node.h>

using namespace v8;

#define BINDING_MAIN(arg1, arg2, arg3) extern "C" NODE_MODULE_EXPORT void NODE_MODULE_INITIALIZER(Local<Object> arg1, Local<Value> arg2, Local<Context> arg3)

#define ARG_INT(argsindex, ctx) argsindex->IntegerValue(ctx).FromJust()

#define ARG(args, output) args.GetReturnValue().Set(output)

#define NUMBER(isolate, num) Number::New(isolate, static_cast<double>(num))

#define STRING(isolate, str, length) String::NewFromUtf8(isolate, str, NewStringType::kNormal, length).ToLocalChecked()

#define STRING_LITERAL(isolate, str) String::NewFromUtf8(isolate, str, NewStringType::kNormal, sizeof(str) - 1).ToLocalChecked()

#define ConstantBindingExport(a, b, c) a.Export(b, c, sizeof(b))

class Binding {
  Isolate * isolate;
  const HandleScope handlescope;
  Local<Context> context;
  Local<Object> exports;
  
  Local<String> funcname;
  Local<FunctionTemplate> ctxFuncTemplate;
  Local<Function> ctxFunc;
  
  public:
    Binding(Local<Object> exports, Local<Context> ctx);
    void Export(const char * name, FunctionCallback callback, const size_t size);
    
    static void DeleterCallback(char * data, void * deleter_data) {
      ::free(reinterpret_cast<void *>(data));
    }

    static inline Local<Object> NewBuffer(Isolate * isolate, void * data, size_t data_size) {
      return node::Buffer::New(isolate, reinterpret_cast<char *>(data), data_size, Binding::DeleterCallback, nullptr).ToLocalChecked();
    }
};

#endif