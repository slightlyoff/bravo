// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "bravo/bindings/util.h"

#include "ppapi/c/pp_var.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/shared_impl/var.h"
#include <string>

namespace bravo {

v8::Handle<v8::FunctionTemplate> CreateBindings(
    v8::FunctionCallback ctorCallback,
    const ConstantConfiguration* constants, size_t constantCount,
    const MethodConfiguration* methods, size_t methodCount) {
  v8::HandleScope handle_scope(v8::Isolate::GetCurrent());
  v8::Handle<v8::FunctionTemplate> templ = v8::FunctionTemplate::New();
  templ->SetCallHandler(ctorCallback);

  v8::Handle<v8::ObjectTemplate> instTempl = templ->InstanceTemplate();
  instTempl->SetInternalFieldCount(kNumberOfInternalFields);

  v8::Handle<v8::ObjectTemplate> proto = templ->PrototypeTemplate();

  InstallConstants(proto, constants, constantCount);
  InstallMethods(proto, methods, methodCount);

  return handle_scope.Close(templ);
}

void InstallConstants(v8::Handle<v8::ObjectTemplate> proto,
    const ConstantConfiguration* constants, size_t constantCount) {
  for (size_t i = 0; i < constantCount; ++i) {
    proto->Set(v8::String::NewSymbol(constants[i].name),
               v8::Uint32::New(constants[i].value));
  }
}

void InstallMethods(v8::Handle<v8::ObjectTemplate> proto,
    const MethodConfiguration* methods, size_t methodCount) {
  for (size_t i = 0; i < methodCount; ++i) {
    proto->Set(v8::String::NewSymbol(methods[i].name),
               v8::FunctionTemplate::New(methods[i].callback));
  }
}

v8::Handle<v8::Value> PPVarToV8Value(v8::Isolate* isolate, PP_Var var) {
  switch (var.type) {
    case PP_VARTYPE_UNDEFINED:
      return v8::Undefined(isolate);
    case PP_VARTYPE_NULL:
      return v8::Null(isolate);
    case PP_VARTYPE_BOOL:
      return var.value.as_bool == PP_TRUE ? v8::True(isolate) : v8::False(isolate);
    case PP_VARTYPE_INT32:
      return v8::Integer::New(var.value.as_int);
    case PP_VARTYPE_DOUBLE:
      return v8::Number::New(var.value.as_double);
    case PP_VARTYPE_STRING:
      {
        ppapi::StringVar* string = ppapi::StringVar::FromPPVar(var);
        if (string) {
          const std::string& value = string->value();
          return v8::String::New(value.c_str(), value.size());
        }
      }
      break;
    case PP_VARTYPE_ARRAY_BUFFER:
      // ppapi::ArrayBufferVar* buffer = ArrayBufferVar::FromPPVar(var);
      // if (!buffer) {
      //   result->Clear();
      //   return false;
      // }
      // HostArrayBufferVar* host_buffer =
      //     static_cast<HostArrayBufferVar*>(buffer);
      // *result =
      //     v8::Local<v8::Value>::New(host_buffer->webkit_buffer().toV8Value());
      // break;
    case PP_VARTYPE_OBJECT:
    case PP_VARTYPE_ARRAY:
    case PP_VARTYPE_DICTIONARY:
    case PP_VARTYPE_RESOURCE:
      NOTIMPLEMENTED();
  }

  return v8::Handle<v8::Value>();
}

}
