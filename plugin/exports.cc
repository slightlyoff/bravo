// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This is the simplest possible C Pepper plugin that does nothing. If you're
// using C++, you will want to look at stub.cc which uses the more convenient
// C++ wrappers.

#include <stddef.h>

#include "base/logging.h"
#include "bravo/bravo_export.h"
#include "bravo/plugin/module.h"
#include "bravo/plugin/exports.h"
#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppp.h"

extern "C" {

BRAVO_EXPORT int32_t PPP_InitializeModule(PP_Module module,
                                          PPB_GetInterface get_interface) {
  return bravo::Module::PPP_InitializeModule(module, get_interface);
}

BRAVO_EXPORT const void* PPP_GetInterface(const char* name) {
  return bravo::Module::PPP_GetInterface(name);
}

}
