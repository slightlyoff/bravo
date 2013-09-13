// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "bravo/plugin/ppb.h"

namespace bravo {

PPB ppb;

bool PPB_Init(PPB_GetInterface get_interface) {
  ppb.audio = reinterpret_cast<const PPB_Audio*>(
      get_interface(PPB_AUDIO_INTERFACE));
  ppb.audio_config = reinterpret_cast<const PPB_AudioConfig*>(
      get_interface(PPB_AUDIO_CONFIG_INTERFACE));
  ppb.console = reinterpret_cast<const PPB_Console*>(
      get_interface(PPB_CONSOLE_INTERFACE));
  ppb.core = reinterpret_cast<const PPB_Core*>(
      get_interface(PPB_CORE_INTERFACE));
  ppb.crypto = reinterpret_cast<const PPB_Crypto_Dev*>(
      get_interface(PPB_CRYPTO_DEV_INTERFACE));
  ppb.file_io = reinterpret_cast<const PPB_FileIO*>(
      get_interface(PPB_FILEIO_INTERFACE));
  ppb.file_ref = reinterpret_cast<const PPB_FileRef*>(
      get_interface(PPB_FILEREF_INTERFACE));
  ppb.file_system = reinterpret_cast<const PPB_FileSystem*>(
      get_interface(PPB_FILESYSTEM_INTERFACE));
  ppb.graphics_2d = reinterpret_cast<const PPB_Graphics2D*>(
      get_interface(PPB_GRAPHICS_2D_INTERFACE));
  ppb.graphics_3d = reinterpret_cast<const PPB_Graphics3D*>(
      get_interface(PPB_GRAPHICS_3D_INTERFACE));
  ppb.image_data = reinterpret_cast<const PPB_ImageData*>(
      get_interface(PPB_IMAGEDATA_INTERFACE));
  ppb.input_event = reinterpret_cast<const PPB_InputEvent*>(
      get_interface(PPB_INPUT_EVENT_INTERFACE));
  ppb.instance = reinterpret_cast<const PPB_Instance*>(
      get_interface(PPB_INSTANCE_INTERFACE));
  ppb.keyboard_input_event = reinterpret_cast<const PPB_KeyboardInputEvent*>(
      get_interface(PPB_KEYBOARD_INPUT_EVENT_INTERFACE));
  ppb.opengl_es2 = reinterpret_cast<const PPB_OpenGLES2*>(
      get_interface(PPB_OPENGLES2_INTERFACE));
  ppb.message_loop = reinterpret_cast<const PPB_MessageLoop*>(
      get_interface(PPB_MESSAGELOOP_INTERFACE));
  ppb.mouse_input_event = reinterpret_cast<const PPB_MouseInputEvent*>(
      get_interface(PPB_MOUSE_INPUT_EVENT_INTERFACE));
  ppb.touch_input_event = reinterpret_cast<const PPB_TouchInputEvent*>(
      get_interface(PPB_TOUCH_INPUT_EVENT_INTERFACE));
  ppb.url_loader = reinterpret_cast<const PPB_URLLoader*>(
      get_interface(PPB_URLLOADER_INTERFACE));
  ppb.var = reinterpret_cast<const PPB_Var*>(
      get_interface(PPB_VAR_INTERFACE));
  ppb.view = reinterpret_cast<const PPB_View*>(
      get_interface(PPB_VIEW_INTERFACE));
  return true;
}

}
