// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "bravo/plugin/module.h"

#include "bravo/plugin/instance.h"
#include "bravo/plugin/ppb.h"
#include "bravo/bravo_export.h"
#include "ppapi/c/pp_errors.h"
#include "ppapi/c/ppp.h"
#include "ppapi/c/ppp_graphics_3d.h"
#include "ppapi/c/ppp_input_event.h"
#include "ppapi/c/ppp_instance.h"
#include "v8/include/v8.h"

namespace bravo {

const char Module::kMimeType[] = "application/bravo";
const char Module::kMimeExtension[] = "bravo";
const char Module::kMimeDescription[] = "Bravo Framework";

const void* Module::PPP_GetInterface(const char* interface_name) {
  if (strcmp(interface_name, PPP_INSTANCE_INTERFACE) == 0) {
    static PPP_Instance instance_interface = {
      &Instance::DidCreate,
      &Instance::DidDestroy,
      &Instance::DidChangeView,
      &Instance::DidChangeFocus,
      &Instance::HandleDocumentLoad,
    };
    return &instance_interface;
  }

  if (strcmp(interface_name, PPP_INPUT_EVENT_INTERFACE) == 0) {
    static PPP_InputEvent input_event_interface = {
      &Instance::HandleInputEvent,
    };
    return &input_event_interface;
  }

  if (strcmp(interface_name, PPP_GRAPHICS_3D_INTERFACE) == 0) {
    static PPP_Graphics3D graphics_3d_client_interface = {
      &Instance::Graphics3DContextLost,
    };
    return &graphics_3d_client_interface;
  }

  return 0;
}

int Module::PPP_InitializeModule(PP_Module module,
                                 PPB_GetInterface get_browser_interface) {
  if (!PPB_Init(get_browser_interface))
    return PP_ERROR_FAILED;
  return PP_OK;
}

}  // namespace bravo
