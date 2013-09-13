// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BRAVO_BINDINGS_V8_TOUCH_POINT_H_
#define BRAVO_BINDINGS_V8_TOUCH_POINT_H_

#include "ppapi/c/pp_touch_point.h"
#include "v8/include/v8.h"

namespace bravo {

v8::Handle<v8::Object> ToV8(const PP_TouchPoint& touch_point) {
  v8::Handle<v8::Object> result = v8::Object::New();
  result->Set(v8::String::NewSymbol("positionX"), v8::Number::New(touch_point.position.x));
  result->Set(v8::String::NewSymbol("positionY"), v8::Number::New(touch_point.position.y));
  result->Set(v8::String::NewSymbol("radiusX"), v8::Number::New(touch_point.radius.x));
  result->Set(v8::String::NewSymbol("radiusY"), v8::Number::New(touch_point.radius.y));
  result->Set(v8::String::NewSymbol("rotationAngle"), v8::Number::New(touch_point.rotation_angle));
  result->Set(v8::String::NewSymbol("pressure"), v8::Number::New(touch_point.pressure));
  return result;
}

}

#endif  // BRAVO_BINDINGS_V8_TOUCH_POINT_H_
