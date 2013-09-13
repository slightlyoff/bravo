// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BRAVO_BINDINGS_V8_COMPLETION_CALLBACK_H_
#define BRAVO_BINDINGS_V8_COMPLETION_CALLBACK_H_

#include "ppapi/c/pp_completion_callback.h"
#include "v8/include/v8.h"

namespace bravo {

class V8CompletionCallback {
 public:
  static PP_CompletionCallback Create(v8::Isolate* isolate,
                                      v8::Handle<v8::Function> callback);

 private:
  explicit V8CompletionCallback(v8::Isolate* isolate,
                                v8::Handle<v8::Function> callback);
  ~V8CompletionCallback();

  static void Callback(void* user_data, int32_t result);

  v8::Isolate* isolate_;
  v8::Persistent<v8::Function> callback_;
};

}

#endif  // BRAVO_BINDINGS_V8_COMPLETION_CALLBACK_H_
