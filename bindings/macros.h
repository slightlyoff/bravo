// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BRAVO_BINDINGS_MACROS_H_
#define BRAVO_BINDINGS_MACROS_H_

#define V8_CTOR_CALLBACK(name, ppb_name)                             \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {       \
     if (!info.IsConstructCall())                                    \
      return;                                                        \
    PP_Resource instance = GetPPResource(v8::Context::GetCurrent()); \
    PP_Resource ppb_name = ppb.ppb_name->Create(instance);           \
    SetPPResource(info.Holder(), ppb_name);                          \
    ppb.core->ReleaseResource(ppb_name);                             \
    info.GetReturnValue().Set(info.Holder());                        \
  }

#define V8_CREATE_BINDING(js_name, ppb_name,                         \
    constants, constantCount,                                        \
    methods, methodCount)                                            \
  namespace {                                                        \
    V8_CTOR_CALLBACK(CtorCallback, ppb_name)                         \
  }                                                                  \
  v8::Handle<v8::FunctionTemplate> Create##js_name##Bindings() {     \
    return CreateBindings(CtorCallback,                              \
        constants, constantCount,                                    \
        methods, methodCount);                                       \
  }

#endif  // BRAVO_BINDINGS_MACROS_H_
