// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BRAVO_PPB_H_
#define BRAVO_PPB_H_

#include "bravo/bravo_export.h"
#include "ppapi/c/dev/ppb_crypto_dev.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppb_audio.h"
#include "ppapi/c/ppb_audio_config.h"
#include "ppapi/c/ppb_console.h"
#include "ppapi/c/ppb_core.h"
#include "ppapi/c/ppb_file_io.h"
#include "ppapi/c/ppb_file_ref.h"
#include "ppapi/c/ppb_file_system.h"
#include "ppapi/c/ppb_graphics_2d.h"
#include "ppapi/c/ppb_graphics_3d.h"
#include "ppapi/c/ppb_image_data.h"
#include "ppapi/c/ppb_input_event.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/c/ppb_message_loop.h"
#include "ppapi/c/ppb_opengles2.h"
#include "ppapi/c/ppb_url_loader.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/c/ppb_view.h"

namespace bravo {

struct PPB {
  const PPB_Audio* audio;
  const PPB_AudioConfig* audio_config;
  const PPB_Console* console;
  const PPB_Core* core;
  const PPB_Crypto_Dev* crypto;
  const PPB_FileIO* file_io;
  const PPB_FileRef* file_ref;
  const PPB_FileSystem* file_system;
  const PPB_Graphics2D* graphics_2d;
  const PPB_Graphics3D* graphics_3d;
  const PPB_ImageData* image_data;
  const PPB_InputEvent* input_event;
  const PPB_Instance* instance;
  const PPB_KeyboardInputEvent* keyboard_input_event;
  const PPB_MessageLoop* message_loop;
  const PPB_MouseInputEvent* mouse_input_event;
  const PPB_OpenGLES2* opengl_es2;
  const PPB_TouchInputEvent* touch_input_event;
  const PPB_URLLoader* url_loader;
  const PPB_Var* var;
  const PPB_View* view;
};

extern PPB ppb;

bool PPB_Init(PPB_GetInterface get_browser_interface);

}

#endif  // BRAVO_PPB_H_
