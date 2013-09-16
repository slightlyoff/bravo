// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "bravo/bindings/v8_url_loader.h"

#include "base/strings/stringprintf.h"
#include "bravo/bindings/macros.h"
#include "bravo/bindings/util.h"
#include "bravo/bindings/v8_completion_callback.h"
#include "bravo/bindings/v8_url_response_info.h"
#include "bravo/plugin/instance.h"
#include "bravo/plugin/ppb.h"

namespace bravo {

namespace {

// int32_t (*Open)(PP_Resource loader,
//                 PP_Resource request_info,
//                 struct PP_CompletionCallback callback);
void Open(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2)
    return;

  if (!info[0]->IsObject())
    return;

  PP_Resource request_info = GetPPResource(info[0].As<v8::Object>());
  if (ppb.url_request_info->IsURLRequestInfo(request_info) != PP_TRUE)
    return;

  if (!info[1]->IsFunction())
    return;

  // TODO: Need to make holder persistent and release it in close?

  v8::Handle<v8::Function> callback = v8::Handle<v8::Function>::Cast(info[1]);
  info.GetReturnValue().Set(v8::Integer::New(ppb.url_loader->Open(
      GetPPResource(info),
      request_info,
      V8CompletionCallback::Create(info.GetIsolate(), callback))));
}

// int32_t (*FollowRedirect)(PP_Resource loader,
//                           struct PP_CompletionCallback callback);

// PP_Bool (*GetUploadProgress)(PP_Resource loader,
//                              int64_t* bytes_sent,
//                              int64_t* total_bytes_to_be_sent);

// PP_Bool (*GetDownloadProgress)(PP_Resource loader,
//                                int64_t* bytes_received,
//                                int64_t* total_bytes_to_be_received);

// PP_Resource (*GetResponseInfo)(PP_Resource loader);
void GetResponseInfo(const v8::FunctionCallbackInfo<v8::Value>& info) {
  // TODO: Needs to do ToV8 since this is the first time we see this object.
  info.GetReturnValue().Set(
      URLResponseInfoToV8(
          ppb.url_loader->GetResponseInfo(GetPPResource(info))));
}

// int32_t (*ReadResponseBody)(PP_Resource loader,
//                             void* buffer,
//                             int32_t bytes_to_read,
//                             struct PP_CompletionCallback callback);
void ReadResponseBody(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2)
    return;

  if (!info[1]->IsFunction())
    return;

  v8::Handle<v8::Function> callback = v8::Handle<v8::Function>::Cast(info[1]);

  if (info[0]->IsArrayBuffer()) {
    v8::Handle<v8::ArrayBuffer> buffer =
        v8::Handle<v8::ArrayBuffer>::Cast(info[0]);

    // TODO: Need to associate the external Contents with this array buffer so
    // that we can use it again.
    // TODO: Need to clear the memory manually when the buffer is no longer
    // reachable. A weak persistent handle with a callback should do it.
    v8::ArrayBuffer::Contents contents = buffer->Externalize();

    info.GetReturnValue().Set(v8::Integer::New(
        ppb.url_loader->ReadResponseBody(
            GetPPResource(info),
            contents.Data(),
            contents.ByteLength(),
            V8CompletionCallback::Create(info.GetIsolate(), callback))));
    return;
  }

  if (info[0]->IsArrayBufferView()) {
    v8::Handle<v8::ArrayBufferView> view =
        v8::Handle<v8::ArrayBufferView>::Cast(info[0]);
    info.GetReturnValue().Set(v8::Integer::New(
        ppb.url_loader->ReadResponseBody(
            GetPPResource(info),
            view->BaseAddress(),
            view->ByteLength(),
            V8CompletionCallback::Create(info.GetIsolate(), callback))));
    return;
  }

}

// int32_t (*FinishStreamingToFile)(PP_Resource loader,
//                                  struct PP_CompletionCallback callback);

// void (*Close)(PP_Resource loader);
void Close(const v8::FunctionCallbackInfo<v8::Value>& info) {
  PP_Resource resource = GetPPResource(info);
  ppb.url_loader->Close(resource);
  // TODO: Make weak
}

static const size_t kMethodCount = 4;

static const MethodConfiguration g_methods[kMethodCount] = {
  { "close", Close },
  { "getResponseInfo", GetResponseInfo },
  { "readResponseBody", ReadResponseBody },
  { "open", Open },
};

static const size_t kConstantCount = 0;

static const ConstantConfiguration g_constants[kConstantCount] = { };

}

V8_CREATE_BINDING(URLLoader, url_loader,
                  g_constants, kConstantCount,
                  g_methods, kMethodCount);

}
