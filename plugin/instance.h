// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BRAVO_INSTANCE_H_
#define BRAVO_INSTANCE_H_

#include "base/memory/scoped_ptr.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppp_instance.h"
#include "v8/include/v8.h"

#include <string>
#include <vector>

namespace bravo {

extern PP_Resource debug_instance;

void Log(std::string);
void Log(int);
void Log(size_t);
void Log(double);

class Instance {
 public:
  Instance(PP_Instance);
  virtual ~Instance();

  //
  // Static trampolines
  //
  static PP_Bool DidCreate(PP_Instance,
                           uint32_t argc,
                           const char* argn[],
                           const char* argv[]);
  static void DidDestroy(PP_Instance);
  static void DidChangeView(PP_Instance, PP_Resource view);
  static void DidChangeFocus(PP_Instance, PP_Bool has_focus);
  static PP_Bool HandleDocumentLoad(PP_Instance, PP_Resource url_loader);
  static PP_Bool HandleInputEvent(PP_Instance, PP_Resource input_event);
  static void Graphics3DContextLost(PP_Instance);

  // Helpers
  static void ReadCallback(void*, int32_t);
  static bool GenerateEntropy(unsigned char*, size_t);

 private:
  //
  // Storage
  //
  static const uint32_t kMaxFileSize = 4096;
  static const char* kv8Flags;  // make non static!
  char buffer_[kMaxFileSize];
  uint32_t buffer_pos_;
  std::vector<char> accumulated_;
  PP_Resource url_loader_;
  PP_Resource view_;
  v8::Persistent<v8::Object> pepper_;
  v8::Persistent<v8::FunctionTemplate> view_bindings_;
  v8::Persistent<v8::Context> context_;
  PP_Instance instance_;
  v8::Isolate* isolate_;
  bool v8_initialized_;

  //
  // Helper Methods
  //
  void InitializeV8();
  void ParseAndRunScript(std::string);
  void Main(v8::Handle<v8::Context> context);
  void RunChangeViewCallback();

  //
  // Real Entry Points
  //
  PP_Bool HandleTouchEvent(PP_Resource touch_event);
  PP_Bool DidCreate(uint32_t argc,
                           const char* argn[],
                           const char* argv[]);
  void DidDestroy();
  void DidChangeView(PP_Resource view);
  void DidChangeFocus(PP_Bool has_focus);
  PP_Bool DidCreate();
  PP_Bool HandleDocumentLoad(PP_Resource url_loader);
  void Graphics3DContextLost();
  // PP_Bool HandleInputEvent(PP_Resource input_event);
};

}  // namespace bravo

#endif  // BRAVO_INSTANCE_H_
