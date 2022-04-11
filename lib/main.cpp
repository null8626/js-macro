#include "main.hpp"
#include <cstdlib>
#include <cstdio>

Binding::Binding(Local<Object> _exports, Local<Context> _context):
  isolate(Isolate::GetCurrent()), exports(_exports), handlescope(HandleScope(isolate)), context(_context) { }