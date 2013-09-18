// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BRAVO_MODULE_H_
#define BRAVO_MODULE_H_

#include "bravo/bravo_export.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/ppb.h"
#include "ppapi/shared_impl/ppapi_permissions.h"

namespace bravo {

class Module {
 public:
  static const void* PPP_GetInterface(const char*);
  static int PPP_InitializeModule(PP_Module, PPB_GetInterface);

  static const char kMimeDescription[];
  static const char kMimeExtension[];
  static const char kMimeType[];
  static const char kPluginName[];

  static const uint32 kPermissions = ppapi::PERMISSION_PRIVATE |
                                     ppapi::PERMISSION_DEV;
};

}  // namespace bravo

#endif  // BRAVO_MODULE_H_
