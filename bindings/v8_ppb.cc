// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "bravo/bindings/v8_ppb.h"

#include "base/strings/stringprintf.h"
#include "bravo/bindings/util.h"
#include "bravo/bindings/v8_graphics_3d.h"
#include "bravo/bindings/v8_url_loader.h"
#include "bravo/bindings/v8_url_request_info.h"
#include "bravo/plugin/instance.h"

namespace bravo {

namespace {

void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() < 1)
    return;

  v8::String::Utf8Value str(args[0]->ToString());
  bravo::Log(*str);
}

v8::Handle<v8::FunctionTemplate> CreateLogBinding() {
  return v8::FunctionTemplate::New(LogCallback);
}

}

v8::Handle<v8::ObjectTemplate> CreatePPBBindings() {
  v8::HandleScope handle_scope(v8::Isolate::GetCurrent());
  v8::Handle<v8::ObjectTemplate> templ = v8::ObjectTemplate::New();
  templ->SetInternalFieldCount(kNumberOfInternalFields);
  templ->Set(v8::String::NewSymbol("log"), CreateLogBinding());
  templ->Set(v8::String::NewSymbol("Graphics3D"), CreateGraphics3DBindings());
  templ->Set(v8::String::NewSymbol("URLLoader"), CreateURLLoaderBindings());
  templ->Set(v8::String::NewSymbol("URLRequestInfo"), CreateURLRequestInfoBindings());
  // TODO: Add URLResponseInfo?
  return handle_scope.Close(templ);
}

}
