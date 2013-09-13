// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "bravo/bindings/v8_completion_callback.h"

#include "base/basictypes.h"

namespace bravo {

V8CompletionCallback::V8CompletionCallback(v8::Isolate* isolate,
                                           v8::Handle<v8::Function> callback)
  : isolate_(isolate),
    callback_(isolate, callback) {
}

V8CompletionCallback::~V8CompletionCallback() {
  callback_.Dispose();
  callback_.Clear();
}

PP_CompletionCallback V8CompletionCallback::Create(
    v8::Isolate* isolate,
    v8::Handle<v8::Function> callback) {
  V8CompletionCallback* v8_callback =
      new V8CompletionCallback(isolate, callback);
  PP_CompletionCallback pp_callback = {
    &V8CompletionCallback::Callback,
    v8_callback,
    PP_COMPLETIONCALLBACK_FLAG_NONE,
  };
  return pp_callback;
}

void V8CompletionCallback::Callback(void* user_data, int32_t result) {
  V8CompletionCallback* callback =
      static_cast<V8CompletionCallback*>(user_data);

  v8::Isolate* isolate = callback->isolate_;
  v8::HandleScope handle_scope(isolate);

  v8::Handle<v8::Function> function =
      v8::Local<v8::Function>::New(isolate, callback->callback_);

  delete callback;
  callback = 0;

  v8::Handle<v8::Context> context = function->CreationContext();
  // TODO(abarth): Do we need to enter the function's context or
  // will v8 do that for us?
  v8::Context::Scope scope(context);
  v8::Handle<v8::Value> argv[] = {
    v8::Integer::New(result),
  };
  v8::TryCatch block;
  function->Call(context->Global(), arraysize(argv), argv);
}

}
