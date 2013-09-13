// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "bravo/bindings/v8_url_request_info.h"

#include "base/strings/stringprintf.h"
#include "bravo/bindings/macros.h"
#include "bravo/bindings/util.h"
#include "bravo/bindings/v8_completion_callback.h"
#include "bravo/plugin/instance.h"
#include "bravo/plugin/ppb.h"
#include <string>

namespace bravo {

namespace {

// PP_Bool (*SetProperty)(PP_Resource request,
//                        PP_URLRequestProperty property,
//                        struct PP_Var value);
void SetProperty(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2)
    return;

  v8::Handle<v8::String> value = info[1]->ToString();
  v8::String::Utf8Value str(value);
  PP_Var str_var = ppb.var->VarFromUtf8(*str, str.length());

  info.GetReturnValue().Set(ppb.url_request_info->SetProperty(
      GetPPResource(info),
      static_cast<PP_URLRequestProperty>(info[0]->Int32Value()),
      str_var));
}

  // PP_Bool (*AppendDataToBody)(PP_Resource request,
  //                             const void* data,
  //                             uint32_t len);

  // PP_Bool (*AppendFileToBody)(PP_Resource request,
  //                             PP_Resource file_ref,
  //                             int64_t start_offset,
  //                             int64_t number_of_bytes,
  //                             PP_Time expected_last_modified_time);

static const size_t kMethodCount = 1;

static const MethodConfiguration g_methods[kMethodCount] = {
  { "setProperty", SetProperty },
};


// https://code.google.com/p/chromium/codesearch#chromium/src/ppapi/c/ppb_url_request_info.h&q=PP_URLREQUESTPROPERTY_RECORDDOWNLOADPROGRESS&sq=package:chromium&type=cs&l=36
static const size_t kConstantCount = 0;

static const ConstantConfiguration g_constants[kConstantCount] = { };

}

V8_CREATE_BINDING(URLRequestInfo, url_request_info,
                  g_constants, kConstantCount,
                  g_methods, kMethodCount);

}
