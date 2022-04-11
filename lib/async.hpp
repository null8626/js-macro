#ifndef V8_ASYNC_HPP
#define V8_ASYNC_HPP

#include <functional>
#include <v8.h>
#include <uv.h>

using namespace v8;
using std::function;

#define NewAsyncWorker(isolate, func, data) new _AsyncWorker(Persistent<Function>(isolate, Local<Function>::Cast(func)), reinterpret_cast<void *>(data))

typedef Persistent<Function, CopyablePersistentTraits<Function>> PersistentFunction;
typedef function<void(void *)> AsyncProcedure;

class Resolver {
  HandleScope scope;
  Isolate * isolate;
  PersistentFunction * callback;
  
  public:
    Resolver(Isolate * isol, PersistentFunction * cb);
    void Resolve(uint32_t argc, Local<Value> args[]);
    
    inline Isolate * GetIsolate() const {
      return isolate;
    }
};

typedef function<void(void *, Resolver *)> AfterAsyncProcedure;

class _AsyncWorker {
  uv_work_t request;
  PersistentFunction callback;
  AsyncProcedure work;
  AfterAsyncProcedure after;
  void * data;
  
  friend static void AsyncWorkerWorker(uv_work_t * request);
  friend static void AsyncWorkerAfter(uv_work_t * request, int _status);
  
public:
  _AsyncWorker(PersistentFunction func, void * _data);
  void Run(AsyncProcedure _work, AfterAsyncProcedure _after);
};

typedef _AsyncWorker * AsyncWorker;

#endif