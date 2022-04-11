#include "async.hpp"

Resolver::Resolver(Isolate * isol, PersistentFunction * cb): isolate(isol), scope(HandleScope(isol)), callback(cb) { }

void Resolver::Resolve(uint32_t argc, Local<Value> args[]) {
  Local<Context> ctx = isolate->GetCurrentContext();
  
  callback->Get(isolate)->Call(ctx, ctx->Global(), argc, args);
}

static void AsyncWorkerWorker(uv_work_t * request) {
  AsyncWorker worker = reinterpret_cast<AsyncWorker>(request->data);
  worker->work(worker->data);
}

static void AsyncWorkerAfter(uv_work_t * request, int _status) {
  AsyncWorker worker = reinterpret_cast<AsyncWorker>(request->data);

  Resolver resolver(Isolate::GetCurrent(), &worker->callback);
  worker->after(worker->data, &resolver);
  
  delete worker;
}

_AsyncWorker::_AsyncWorker(PersistentFunction func, void * _data): callback(func), data(_data) {
  request.data = reinterpret_cast<void *>(this);
}

void _AsyncWorker::Run(AsyncProcedure _work, AfterAsyncProcedure _after) {
  work = _work;
  after = _after;
  
  uv_queue_work(uv_default_loop(), &request, ::AsyncWorkerWorker, ::AsyncWorkerAfter);
}