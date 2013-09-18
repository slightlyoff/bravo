// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BRAVO_BINDINGS_UTIL_H_
#define BRAVO_BINDINGS_UTIL_H_

#include "base/logging.h"
#include "bravo/plugin/ppb.h"
#include "ppapi/c/pp_resource.h"
#include "v8/include/v8.h"

namespace bravo {

enum InternalFields {
  kPPResource = 0,
  // Add new internal fields avoid this line.
  kNumberOfInternalFields,
};

enum EmbedderFields {
  kDebuggerData = 0, // See v8.h ~L5017 re: SetEmbedderData. Le sigh.
  kResourceRef,
  kNumberOfEmbedderFields,
};

inline PP_Resource UnpackResource(v8::Handle<v8::Value> external) {
  void* packed = v8::Handle<v8::External>::Cast(external)->Value();
  return static_cast<PP_Resource>(reinterpret_cast<uintptr_t>(packed));
}

inline v8::Handle<v8::External> PackResource(PP_Resource resource) {
  v8::Handle<v8::External> external = v8::External::New(
      reinterpret_cast<void*>(resource));
  DCHECK(resource == UnpackResource(external));
  return external;
}

inline PP_Resource GetPPResource(v8::Handle<v8::Object> holder) {
  DCHECK(holder->InternalFieldCount() == kNumberOfInternalFields);
  return UnpackResource(holder->GetInternalField(kPPResource));
}

inline PP_Resource GetPPResource(v8::Handle<v8::Context> context) {
  return UnpackResource(context->GetEmbedderData(kResourceRef));
}

template<typename T>
inline PP_Resource GetPPResource(const v8::FunctionCallbackInfo<T>& info) {
  return GetPPResource(info.Holder());
}

inline void PPResourceDeRefObjectCallback(v8::Isolate* isolate,
                                          v8::Persistent<v8::Object>* obj,
                                          void* param) {
  v8::HandleScope handle_scope(isolate);
  v8::Handle<v8::Object> local = v8::Handle<v8::Object>::New(isolate, *obj);
  PP_Resource resource = GetPPResource(local);
  ppb.core->ReleaseResource(resource);
  obj->Dispose();
  obj->Clear();
}

inline void SetPPResource(v8::Handle<v8::Object> holder, PP_Resource resource) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  DCHECK(holder->InternalFieldCount() == kNumberOfInternalFields);
  holder->SetInternalField(kPPResource, PackResource(resource));
  ppb.core->AddRefResource(resource);
  v8::Persistent<v8::Object> resource_ref(isolate, holder);
  resource_ref.MakeWeak<void>(0, &PPResourceDeRefObjectCallback);
}

inline void SetPPResource(v8::Handle<v8::Context> context, PP_Resource resource) {
  context->SetEmbedderData(kResourceRef, PackResource(resource));
}

inline v8::Handle<v8::Object> InstantiateObject(v8::Handle<v8::ObjectTemplate> templ, PP_Resource resource)
{
    v8::Handle<v8::Object> instance = templ->NewInstance();
    SetPPResource(instance, resource);
    return instance;
}

struct MethodConfiguration {
  const char* const name;
  v8::FunctionCallback callback;
};

struct ConstantConfiguration {
  const char* const name;
  unsigned int value;
};

v8::Handle<v8::FunctionTemplate> CreateBindings(
    v8::FunctionCallback ctorCallback,
    const ConstantConfiguration* constants, size_t constantCount,
    const MethodConfiguration* methods, size_t methodCount);

void InstallConstants(v8::Handle<v8::ObjectTemplate> proto,
    const ConstantConfiguration* constants, size_t constantCount);

void InstallMethods(v8::Handle<v8::ObjectTemplate> proto,
    const MethodConfiguration* methods, size_t methodCount);

v8::Handle<v8::Value> PPVarToV8Value(v8::Isolate*, PP_Var);
PP_Var V8ValueToPPVar(v8::Handle<v8::Value>);

}

#endif  // BRAVO_BINDINGS_UTIL_H_
