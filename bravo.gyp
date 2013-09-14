# Copyright (c) 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [{
    'target_name': 'bravo_plugin',
    'type': 'static_library',
    # 'type': 'shared_library',
    'include_dirs': [
      '..',
    ],
    'dependencies': [
      '../ui/gl/gl.gyp:gl',
      '../base/base.gyp:base',
      '../v8/tools/gyp/v8.gyp:v8',
    ],
    'defines': [
      'BRAVO_IMPLEMENTATION'
    ],
    'sources': [
      'bindings/macros.h',
      'bindings/util.h',
      'bindings/util.cc',
      'bindings/v8_completion_callback.cc',
      'bindings/v8_completion_callback.h',
      'bindings/v8_graphics_3d.cc',
      'bindings/v8_graphics_3d.h',
      'bindings/v8_message_loop.cc',
      'bindings/v8_message_loop.h',
      'bindings/v8_opengl_es2.cc',
      'bindings/v8_opengl_es2.h',
      'bindings/v8_ppb.cc',
      'bindings/v8_ppb.h',
      'bindings/v8_rect.h',
      'bindings/v8_touch_point.h',
      'bindings/v8_url_loader.cc',
      'bindings/v8_url_loader.h',
      'bindings/v8_url_response_info.cc',
      'bindings/v8_url_response_info.h',
      'bindings/v8_url_request_info.cc',
      'bindings/v8_url_request_info.h',
      'bindings/v8_view.cc',
      'bindings/v8_view.h',
      'bravo_export.h',
      'plugin/instance.cc',
      'plugin/instance.h',
      'plugin/module.cc',
      'plugin/module.h',
      'plugin/ppb.cc',
      'plugin/ppb.h',
    ],
  }],
}
