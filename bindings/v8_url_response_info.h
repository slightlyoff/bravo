// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BRAVO_BINDINGS_V8_URL_REQUEST_INFO_H_
#define BRAVO_BINDINGS_V8_URL_REQUEST_INFO_H_

#include "v8/include/v8.h"
#include "ppapi/c/pp_resource.h"

namespace bravo {

v8::Handle<v8::FunctionTemplate> CreateURLResponseInfoBindings();
v8::Handle<v8::Object> URLResponseInfoToV8(PP_Resource);

}

#endif  // BRAVO_BINDINGS_V8_URL_REQUEST_INFO_H_
