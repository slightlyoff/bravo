// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "bravo/bindings/v8_graphics_3d.h"

#include "bravo/bindings/util.h"
#include "bravo/bindings/v8_completion_callback.h"
#include "bravo/bindings/v8_opengl_es2.h"
#include "bravo/plugin/instance.h"
#include "bravo/plugin/ppb.h"
#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_errors.h"

namespace bravo {

namespace {

#define V8_BIND_MAX_VALUE(name, attr)                                         \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    int32_t value;                                                            \
    int32_t ret = ppb.graphics_3d->GetAttribMaxValue(                         \
      GetPPResource(info), attr, &value);                                     \
                                                                              \
    if (ret != PP_TRUE) {                                                     \
      if (ret == PP_ERROR_BADRESOURCE)                                        \
        Log("Called for an invalid resource.");                               \
      if (ret == PP_ERROR_BADARGUMENT)                                        \
        Log("Invalid attribute or value is 0!");                              \
      if (ret == PP_ERROR_FAILED)                                             \
        Log("Generic failure. Probably unimplemented!");                      \
                                                                              \
      info.GetReturnValue().Set(v8::Uint32::New(0));                          \
      return;                                                                 \
    }                                                                         \
    info.GetReturnValue().Set(v8::Uint32::New(value));                        \
  }

V8_BIND_MAX_VALUE(MaxAlphaSizeCallback,     PP_GRAPHICS3DATTRIB_ALPHA_SIZE)
V8_BIND_MAX_VALUE(MaxBlueSizeCallback,      PP_GRAPHICS3DATTRIB_BLUE_SIZE)
V8_BIND_MAX_VALUE(MaxGreenSizeCallback,     PP_GRAPHICS3DATTRIB_GREEN_SIZE)
V8_BIND_MAX_VALUE(MaxRedSizeCallback,       PP_GRAPHICS3DATTRIB_RED_SIZE)
V8_BIND_MAX_VALUE(MaxDepthSizeCallback,     PP_GRAPHICS3DATTRIB_DEPTH_SIZE)
V8_BIND_MAX_VALUE(MaxStencilSizeCallback,   PP_GRAPHICS3DATTRIB_STENCIL_SIZE)
V8_BIND_MAX_VALUE(MaxSampleCallback,        PP_GRAPHICS3DATTRIB_SAMPLES)
V8_BIND_MAX_VALUE(MaxSampleBuffersCallback, PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS)
V8_BIND_MAX_VALUE(MaxWidthCallback,         PP_GRAPHICS3DATTRIB_WIDTH)
V8_BIND_MAX_VALUE(MaxHeightCallback,        PP_GRAPHICS3DATTRIB_HEIGHT)


void ResizeBuffers(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() != 1)
    return;
  if (!info[0]->IsObject())
    return;
  v8::Handle<v8::Object> args = v8::Handle<v8::Object>::Cast(info[0]);
  // FIXME: check that args has "width" and "height" before using them
  ppb.graphics_3d->ResizeBuffers(
    GetPPResource(info),
    args->Get(v8::String::NewSymbol("width"))->Int32Value(),
    args->Get(v8::String::NewSymbol("height"))->Int32Value());
}

void SwapBuffers(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() != 1)
    return;
  if (!info[0]->IsFunction())
    return;
  v8::Handle<v8::Function> callback = v8::Handle<v8::Function>::Cast(info[0]);
  ppb.graphics_3d->SwapBuffers(
    GetPPResource(info),
    V8CompletionCallback::Create(info.GetIsolate(), callback));
}

void CtorCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
   if (!info.IsConstructCall())
    return;

  if (info.Length() == 0)
    return;

  if (!info[0]->IsObject())
    return;

  v8::Handle<v8::Object> options = v8::Handle<v8::Object>::Cast(info[0]);

  int32_t attribs[] = {
    PP_GRAPHICS3DATTRIB_WIDTH,
        options->Get(v8::String::NewSymbol("width"))->Int32Value(),
    PP_GRAPHICS3DATTRIB_HEIGHT,
        options->Get(v8::String::NewSymbol("height"))->Int32Value(),
    PP_GRAPHICS3DATTRIB_NONE
  };

  PP_Resource instance = GetPPResource(v8::Context::GetCurrent());

  PP_Resource graphics_context = ppb.graphics_3d->Create(
      instance, 0, attribs);
  SetPPResource(info.Holder(), graphics_context);
  ppb.core->ReleaseResource(graphics_context);
  ppb.instance->BindGraphics(instance, graphics_context);
  info.GetReturnValue().Set(info.Holder());
}

}

v8::Handle<v8::FunctionTemplate> CreateGraphics3DBindings() {
  v8::HandleScope handle_scope;

  v8::Handle<v8::FunctionTemplate> templ = v8::FunctionTemplate::New();
  templ->Inherit(CreateWebGLBindings());
  templ->SetCallHandler(CtorCallback);

  v8::Handle<v8::ObjectTemplate> instTempl = templ->InstanceTemplate();
  instTempl->SetInternalFieldCount(kNumberOfInternalFields);
  v8::Handle<v8::ObjectTemplate> proto = templ->PrototypeTemplate();
  proto->Set(v8::String::NewSymbol("resizeBuffers"),
             v8::FunctionTemplate::New(ResizeBuffers));
  proto->Set(v8::String::NewSymbol("swapBuffers"),
             v8::FunctionTemplate::New(SwapBuffers));

  // FIXME(slightlyoff): sooo...it seems these are a lie: crbug.com/224973
  /*
  proto->SetAccessorProperty(v8::String::NewSymbol("maxAlphaSize"),
                             v8::FunctionTemplate::New(MaxAlphaSizeCallback));
  proto->SetAccessorProperty(v8::String::NewSymbol("maxBlueSize"),
                             v8::FunctionTemplate::New(MaxBlueSizeCallback));
  proto->SetAccessorProperty(v8::String::NewSymbol("maxGreenSize"),
                             v8::FunctionTemplate::New(MaxGreenSizeCallback));
  proto->SetAccessorProperty(v8::String::NewSymbol("maxRedSize"),
                             v8::FunctionTemplate::New(MaxRedSizeCallback));
  proto->SetAccessorProperty(v8::String::NewSymbol("maxDepthSize"),
                             v8::FunctionTemplate::New(MaxDepthSizeCallback));
  proto->SetAccessorProperty(v8::String::NewSymbol("maxStencilSize"),
                             v8::FunctionTemplate::New(MaxStencilSizeCallback));
  proto->SetAccessorProperty(v8::String::NewSymbol("maxSample"),
                             v8::FunctionTemplate::New(MaxSampleCallback));
  proto->SetAccessorProperty(v8::String::NewSymbol("maxSampleBuffers"),
                             v8::FunctionTemplate::New(MaxSampleBuffersCallback));
  proto->SetAccessorProperty(v8::String::NewSymbol("maxWidth"),
                             v8::FunctionTemplate::New(MaxWidthCallback));
  proto->SetAccessorProperty(v8::String::NewSymbol("maxHeight"),
                             v8::FunctionTemplate::New(MaxHeightCallback));
  */

  // TODO(slightlyoff):
  //   int32_t (*GetAttribs)(PP_Resource context, int32_t attrib_list[]);
  //   int32_t (*SetAttribs)(PP_Resource context, const int32_t attrib_list[]);
  //   int32_t (*GetError)(PP_Resource context);

  return handle_scope.Close(templ);
}

}
