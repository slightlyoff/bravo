// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BRAVO_BINDINGS_V8_RECT_H_
#define BRAVO_BINDINGS_V8_RECT_H_

#include "ppapi/c/pp_rect.h"
#include "v8/include/v8.h"

namespace bravo {

v8::Handle<v8::Object> ToV8(const PP_Rect& rect) {
  v8::Handle<v8::Object> result = v8::Object::New();
  result->Set(v8::String::NewSymbol("x"), v8::Uint32::New(rect.point.x));
  result->Set(v8::String::NewSymbol("y"), v8::Uint32::New(rect.point.y));
  result->Set(v8::String::NewSymbol("width"), v8::Uint32::New(rect.size.width));
  result->Set(v8::String::NewSymbol("height"), v8::Uint32::New(rect.size.height));
  return result;
}

}

#endif  // BRAVO_BINDINGS_V8_RECT_H_
