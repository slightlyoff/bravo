// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "bravo/bindings/v8_message_loop.h"

#include "bravo/bindings/util.h"
#include "bravo/bindings/v8_completion_callback.h"
#include "bravo/plugin/ppb.h"

namespace bravo {

namespace {

void PostWork(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() != 2)
    return;
  if (!info[0]->IsFunction())
    return;
  v8::Handle<v8::Function> callback = v8::Handle<v8::Function>::Cast(info[0]);
  ppb.message_loop->PostWork(
    GetPPResource(info),
    V8CompletionCallback::Create(info.GetIsolate(), callback),
    info[1]->Int32Value());
}

}

v8::Handle<v8::ObjectTemplate> CreateMessageLoopBindings() {
  v8::HandleScope handle_scope;
  v8::Handle<v8::ObjectTemplate> templ = v8::ObjectTemplate::New();
  templ->SetInternalFieldCount(kNumberOfInternalFields);

  templ->Set(v8::String::NewSymbol("postWork"), v8::FunctionTemplate::New(PostWork), v8::ReadOnly);

  return handle_scope.Close(templ);
}

}
