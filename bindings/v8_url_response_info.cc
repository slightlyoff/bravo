// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "bravo/bindings/v8_url_response_info.h"

#include "base/logging.h"
#include "bravo/bindings/util.h"
#include "bravo/plugin/ppb.h"
#include "ppapi/c/pp_resource.h"

namespace bravo {

namespace {

// struct PP_Var (*GetProperty)(PP_Resource response,
//                              PP_URLResponseProperty property);
void GetProperty(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1)
    return;

  PP_URLResponseProperty property =
      static_cast<PP_URLResponseProperty>(info[0]->Int32Value());

  PP_Var pp_value = ppb.url_response_info->GetProperty(
      GetPPResource(info),
      property);

  v8::Handle<v8::Value> v8_value = bravo::PPVarToV8Value(info.GetIsolate(), pp_value);

  info.GetReturnValue().Set(v8_value);
}

// PP_Resource (*GetBodyAsFileRef)(PP_Resource response);

static const size_t kMethodCount = 1;

static const MethodConfiguration g_methods[kMethodCount] = {
  { "getProperty", GetProperty },
};


// https://code.google.com/p/chromium/codesearch#chromium/src/ppapi/c/ppb_url_response_info.h&q=PP_URLREQUESTPROPERTY_RECORDDOWNLOADPROGRESS&sq=package:chromium&type=cs&l=36
static const size_t kConstantCount = 0;

static const ConstantConfiguration g_constants[kConstantCount] = { };

}

v8::Handle<v8::Object> URLResponseInfoToV8(PP_Resource resource) {
  DCHECK(ppb.url_response_info->IsURLResponseInfo(resource) == PP_TRUE);

  v8::HandleScope handle_scope(v8::Isolate::GetCurrent());
  v8::Handle<v8::FunctionTemplate> templ = v8::FunctionTemplate::New();
  v8::Handle<v8::ObjectTemplate> instTempl = templ->InstanceTemplate();
  instTempl->SetInternalFieldCount(kNumberOfInternalFields);

  v8::Handle<v8::ObjectTemplate> proto = templ->PrototypeTemplate();

  InstallConstants(proto, g_constants, kConstantCount);
  InstallMethods(proto, g_methods, kMethodCount);

  return handle_scope.Close(InstantiateObject(instTempl, resource));
}

}
