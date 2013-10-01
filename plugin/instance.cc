// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "bravo/plugin/instance.h"

#include "base/strings/stringprintf.h"
#include "bravo/bindings/util.h"
#include "bravo/bindings/v8_graphics_3d.h"
#include "bravo/bindings/v8_ppb.h"
#include "bravo/bindings/v8_touch_point.h"
#include "bravo/bindings/v8_view.h"
#include "bravo/bindings/v8_opengl_es2.h"
#include "bravo/bindings/v8_ppb.h"
#include "bravo/plugin/instance.h"
#include "bravo/plugin/ppb.h"
#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_var.h"
#include "ppapi/c/ppb_url_loader.h"
#include "ppapi/c/ppp.h"
#include "ppapi/c/ppp_input_event.h"
#include "ppapi/c/ppp_instance.h"
#include "v8/include/v8.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <limits>
#include <map>
#include <string>
#include <vector>

namespace bravo {

namespace {

class DummyArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
  virtual void* Allocate(size_t length) OVERRIDE { return malloc(length); }
  virtual void* AllocateUninitialized(size_t length) OVERRIDE { return malloc(length); }
  virtual void Free(void* data, size_t length) OVERRIDE { free(data); }
  virtual void Free(void* data) { CHECK(false); }
};

}

PP_Resource debug_instance = 0;

void Log(std::string msg) {
  PP_Var msg_var = ppb.var->VarFromUtf8(msg.data(), msg.size());
  ppb.console->Log(debug_instance, PP_LOGLEVEL_LOG, msg_var);
}

void Log(int i) {
  Log(base::StringPrintf("%i", i));
}

void Log(size_t i) {
  Log(base::StringPrintf("%zu", i));
}

void Log(double f) {
  Log(base::StringPrintf("%f", f));
}

Instance::Instance(PP_Instance instance)
  : buffer_pos_(0),
    instance_(instance),
    v8_initialized_(false) {
}

Instance::~Instance() {
}

typedef std::map<PP_Instance, Instance*> DataMap;

static DataMap& Map() {
  static DataMap* map = new DataMap;
  return *map;
}

static Instance* ToData(PP_Instance instance) {
  return Map()[instance];
}

//
// Static trampolines
//

PP_Bool Instance::HandleDocumentLoad(PP_Instance instance, PP_Resource url_loader) {
  if (url_loader) {
    return Map()[instance]->HandleDocumentLoad(url_loader);
  }
  return PP_TRUE;
}

void Instance::DidChangeView(PP_Instance instance, PP_Resource view) {
  Map()[instance]->DidChangeView(view);
}

PP_Bool Instance::DidCreate(PP_Instance instance,
                            uint32_t argc,
                            const char* argn[],
                            const char* argv[]) {
  DCHECK(instance);
  DCHECK(!ToData(instance));
  debug_instance = instance;
  Instance* inst = new Instance(instance);
  Map()[instance] = inst;
  return inst->DidCreate();
}

void Instance::DidDestroy(PP_Instance instance) {
  Map()[instance]->DidDestroy();
  DataMap::iterator iter = Map().find(instance);
  delete iter->second;
  Map().erase(iter);
}

void Instance::DidChangeFocus(PP_Instance instance, PP_Bool has_focus) {
  Map()[instance]->DidChangeFocus(has_focus);
}

void Instance::Graphics3DContextLost(PP_Instance instance) {
  Map()[instance]->Graphics3DContextLost();
}

//
// Static helpers
//
void Instance::ReadCallback(void* user_data, int32_t pp_error_or_bytes) {
  Instance* instance = reinterpret_cast<Instance*>(user_data);

  if (pp_error_or_bytes < PP_OK) {
    ppb.core->ReleaseResource(instance->url_loader_);
    return;
  }

  if (PP_OK == pp_error_or_bytes) {
    /*
    Log(std::string(instance->accumulated_.begin(),
                    instance->accumulated_.end()));
                    */

    instance->ParseAndRunScript(std::string(instance->accumulated_.begin(),
                                            instance->accumulated_.end()));

    ppb.url_loader->Close(instance->url_loader_);
    ppb.core->ReleaseResource(instance->url_loader_);
  } else {
    PP_CompletionCallback callback =
        PP_MakeCompletionCallback(ReadCallback, user_data);

    instance->accumulated_.insert(instance->accumulated_.end(),
                                  instance->buffer_,
                                  instance->buffer_ + pp_error_or_bytes);

    instance->buffer_pos_ += pp_error_or_bytes;
    ppb.url_loader->ReadResponseBody(instance->url_loader_,
                                     instance->buffer_,
                                     kMaxFileSize,
                                     callback);
  }
}

bool GenerateEntropy(unsigned char* buffer, size_t amount) {
  size_t min_amount = std::min<size_t>(amount,
                                       std::numeric_limits<size_t>::max());
  size_t final_amount = std::max<size_t>(0, min_amount);
  ppb.crypto->GetRandomBytes(reinterpret_cast<char*>(buffer), final_amount);
  return true;
}

//
// Instance methods
//
void Instance::DidChangeView(PP_Resource view) {
  if (view_ && ppb.view->IsView(view_)) {
    ppb.core->ReleaseResource(view_);
  }

  view_ = view;
  ppb.core->AddRefResource(view_);
  RunChangeViewCallback();
}

void Instance::RunChangeViewCallback() {
  if (pepper_.IsEmpty())
    return;

  if (!isolate_)
    return;

  v8::HandleScope handle_scope(isolate_);
  v8::Handle<v8::Context> local_context = v8::Handle<v8::Context>::New(isolate_, context_);
  v8::Context::Scope context_scope(local_context);

  v8::Handle<v8::Object> local_pepper = v8::Handle<v8::Object>::New(isolate_, pepper_);

  // FIXME(slightlyoff): CRASH!! not resiliant to resizing!!!
  if (view_bindings_.IsEmpty()) {
    view_bindings_.Reset(isolate_, CreateViewBindings());
  }
  v8::Handle<v8::FunctionTemplate> local_view_bindings = v8::Handle<v8::FunctionTemplate>::New(isolate_, view_bindings_);
  v8::Handle<v8::Object> viewInst = local_view_bindings->InstanceTemplate()->NewInstance();
  SetPPResource(viewInst, view_);
  // Extract event handlers to be called.
  v8::Handle<v8::Value> onchangeviewValue =
      local_pepper->Get(v8::String::NewSymbol("onchangeview"));

  if (onchangeviewValue->IsFunction()) {
    v8::TryCatch try_catch;
    v8::Handle<v8::Function> onchangeiew = onchangeviewValue.As<v8::Function>();
    v8::Handle<v8::Value> onchangeviewArgs[] = {
      viewInst,
    };
    onchangeiew->Call(local_context->Global(), arraysize(onchangeviewArgs), onchangeviewArgs);
    if (try_catch.HasCaught()) {
      Log("EXCEPTION in onviewchange");
      v8::Handle<v8::Message> message = try_catch.Message();
      Log(base::StringPrintf("Line: %i", message->GetLineNumber()));
      v8::Handle<v8::String> sourceLine = message->GetSourceLine();
      v8::String::Utf8Value source(sourceLine);
      Log(base::StringPrintf("  %s", *source));

      int startColumn = message->GetStartColumn();
      Log(base::StringPrintf("  %s^",std::string(startColumn,'-').c_str()));

      /*
      v8::Handle<v8::StackTrace> trace = message->GetStackTrace();
      // Log(base::StringPrintf("trace: %p", &trace));
      int frame_count = trace->GetFrameCount();
      Log(base::StringPrintf("frame_count: %i", frame_count));

      for (uint32_t i = 0; i < frame_count; ++i) {
        v8::Handle<v8::StackFrame> frame = trace->GetFrame(i);
        v8::Handle<v8::String> functionNameString = frame->GetFunctionName();
        v8::String::Utf8Value functionName(functionNameString);
        Log(base::StringPrintf("\t%i:%i::%s", frame->GetLineNumber(),
                                              frame->GetColumn(),
                                              *functionName));
      }
      */
    }
  }
}

PP_Bool Instance::HandleDocumentLoad(PP_Resource url_loader) {
  if (url_loader) {
    url_loader_ = url_loader;
    ppb.core->AddRefResource(url_loader);

    PP_CompletionCallback callback =
        PP_MakeCompletionCallback(ReadCallback, reinterpret_cast<void*>(this));
    // callback.flags = callback.flags | PP_COMPLETIONCALLBACK_FLAG_OPTIONAL;

    int32_t pp_error_or_bytes = ppb.url_loader->ReadResponseBody(url_loader,
                                                                 buffer_,
                                                                 kMaxFileSize,
                                                                 callback);
    CHECK(pp_error_or_bytes == PP_OK_COMPLETIONPENDING);
  }

  return PP_TRUE;
}

PP_Bool Instance::HandleTouchEvent(PP_Resource touch_event) {
  // FIXME(slightlyoff): wire our isolate from the instance in the map
  v8::HandleScope handle_scope(v8::Isolate::GetCurrent());
  // TODO(abarth): We need to enter the script's context here insteadd of
  // returning.
  return PP_TRUE;

  uint32_t count = ppb.touch_input_event->GetTouchCount(
      touch_event, PP_TOUCHLIST_TYPE_TOUCHES);
  v8::Handle<v8::Array> touches = v8::Array::New();
  for (uint32_t i = 0; i < count; ++i) {
    PP_TouchPoint touch_point = ppb.touch_input_event->GetTouchByIndex(
        touch_event, PP_TOUCHLIST_TYPE_TOUCHES, i);
    touches->Set(i, ToV8(touch_point));
  }
  // TODO(abarth): Give the touches array to JavaScript.
  return PP_TRUE;
}

void Instance::DidChangeFocus(PP_Bool has_focus) {
  Log("DidChangeFocus");
}

PP_Bool Instance::HandleInputEvent(PP_Instance instance,
                                   PP_Resource input_event) {
  if (ppb.touch_input_event->IsTouchInputEvent(input_event))
    return Map()[instance]->HandleTouchEvent(input_event);

  return PP_TRUE;
}

PP_Bool Instance::DidCreate() {
  ppb.input_event->RequestInputEvents(instance_,
                                      PP_INPUTEVENT_CLASS_MOUSE |
                                      PP_INPUTEVENT_CLASS_KEYBOARD);

  // Log("DidCreate");
  // Log(base::StringPrintf("INSTANCE: %i", instance_));
  return PP_TRUE;
}

void Instance::DidDestroy() {
  Log("DidDestroy");
}

void Instance::InitializeV8() {
  if (v8_initialized_)
    return;

  DummyArrayBufferAllocator array_buffer_allocator;
  v8::V8::SetArrayBufferAllocator(&array_buffer_allocator);
  v8::V8::InitializeICU();

  // Debugging v8 flags
  static const char* kv8Flags = "--use_strict --harmony --expose_natives_as=v8natives --expose_debug_as=v8debug --expose_gc_as=v8gc --expose_externalize_string";
  // Prod flags
  // const char* kv8Flags = "--use_strict --harmony";
  // Some flags we might want to dig into:
  //    --concurrent_sweeping
  //    --sweeper_threads
  //    --parallel_marking
  //    --marking_threads
  //    --allow_natives_syntax // for our own methods + snapshotting
  v8::V8::SetFlagsFromString(kv8Flags, strlen(kv8Flags));
  v8::V8::SetEntropySource(&bravo::GenerateEntropy);
  v8::V8::Initialize();
  isolate_ = v8::Isolate::GetCurrent();
  v8_initialized_ = true;
}

void Instance::ParseAndRunScript(std::string source) {
  InitializeV8();
  v8::HandleScope handle_scope(isolate_);
  v8::Handle<v8::ObjectTemplate> global_template = v8::ObjectTemplate::New();
  v8::Handle<v8::Context> context = v8::Context::New(isolate_, NULL, global_template);
  SetPPResource(context, instance_);
  context_.Reset(isolate_, context);
  v8::Context::Scope context_scope(context);

  CHECK(!context.IsEmpty());

  v8::TryCatch try_catch;
  v8::Handle<v8::String> v8source = v8::String::New(source.data(), source.size());
  v8::Handle<v8::Script> script = v8::Script::Compile(v8source);
  if (script.IsEmpty()) {
    Log("COMPILE ERROR.");
    return;
  }
  // v8::Handle<v8::Value> result =
  script->Run();

  if (try_catch.HasCaught()) {
    Log("EXCEPTION in top-level script.");
    return;
  }
  Main(context);
}

void Instance::Main(v8::Handle<v8::Context> context) {
  if (!isolate_)
    return;

  if (!instance_)
    return;

  v8::Handle<v8::Object> local_pepper = CreatePPBBindings()->NewInstance();
  pepper_.Reset(isolate_, local_pepper);

  // FIXME(slightlyoff): this blows up in debug. instance_ isn't a PP_Resource
  // SetPPResource(local_pepper, instance_);
  v8::Handle<v8::Value> mainValue =
      context->Global()->Get(v8::String::NewSymbol("main"));

  if (mainValue.IsEmpty() || !mainValue->IsFunction()) {
    Log("ERROR: Main empty or not a function.");
    return;
  }
  v8::Handle<v8::Function> main = mainValue.As<v8::Function>();
  if (!main->IsFunction()) {
    Log("ERROR: Main is not a function.");
    return;
  }

  v8::Handle<v8::Value> argv[] = {
    local_pepper,
  };
  main->Call(context->Global(), arraysize(argv), argv);

  RunChangeViewCallback();
}

void Instance::Graphics3DContextLost() {
  Log("Instance::Graphics3DContextLost");
}

}
