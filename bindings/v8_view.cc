// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "bravo/bindings/v8_view.h"

#include "bravo/bindings/util.h"
#include "bravo/bindings/v8_rect.h"
#include "bravo/plugin/ppb.h"

namespace bravo {

namespace {

void IsFullscreenCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(v8::BooleanObject::New(ppb.view->IsFullscreen(GetPPResource(info))));
}

void IsVisibleCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(v8::BooleanObject::New(ppb.view->IsVisible(GetPPResource(info))));
}

void IsPageVisibleCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(v8::BooleanObject::New(ppb.view->IsPageVisible(GetPPResource(info))));
}

void DeviceScaleCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(v8::NumberObject::New(ppb.view->GetDeviceScale(GetPPResource(info))));
}

void CSSScaleCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(v8::NumberObject::New(ppb.view->GetCSSScale(GetPPResource(info))));
}

void RectCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  PP_Rect view_rect;
  ppb.view->GetRect(GetPPResource(info), &view_rect);
  info.GetReturnValue().Set(ToV8(view_rect));
}

void ClipRectCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  PP_Rect clip_rect;
  ppb.view->GetClipRect(GetPPResource(info), &clip_rect);
  info.GetReturnValue().Set(ToV8(clip_rect));
}

}

v8::Handle<v8::FunctionTemplate> CreateViewBindings() {
  v8::HandleScope handle_scope(v8::Isolate::GetCurrent());
  v8::Handle<v8::FunctionTemplate> templ = v8::FunctionTemplate::New();
  v8::Handle<v8::ObjectTemplate> instTempl = templ->InstanceTemplate();
  instTempl->SetInternalFieldCount(kNumberOfInternalFields);

  v8::Handle<v8::ObjectTemplate> proto = templ->PrototypeTemplate();
  proto->SetAccessorProperty(v8::String::NewSymbol("isFullscreen"), v8::FunctionTemplate::New(IsFullscreenCallback));
  proto->SetAccessorProperty(v8::String::NewSymbol("isVisible"), v8::FunctionTemplate::New(IsVisibleCallback));
  proto->SetAccessorProperty(v8::String::NewSymbol("isPageVisible"), v8::FunctionTemplate::New(IsPageVisibleCallback));
  proto->SetAccessorProperty(v8::String::NewSymbol("deviceScale"), v8::FunctionTemplate::New(DeviceScaleCallback));
  proto->SetAccessorProperty(v8::String::NewSymbol("cssScale"), v8::FunctionTemplate::New(CSSScaleCallback));

  proto->SetAccessorProperty(v8::String::NewSymbol("rect"), v8::FunctionTemplate::New(RectCallback));
  proto->SetAccessorProperty(v8::String::NewSymbol("clipRect"), v8::FunctionTemplate::New(ClipRectCallback));
  return handle_scope.Close(templ);
}

}
