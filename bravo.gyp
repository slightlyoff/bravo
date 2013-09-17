# Copyright (c) 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
  {
    'target_name': 'bravo_base',
    'type': 'static_library',
    'include_dirs': [
      '..',
    ],
    'dependencies': [
      '../ui/gl/gl.gyp:gl',
      '../base/base.gyp:base',
      '../v8/tools/gyp/v8.gyp:v8',
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
  },
  {
    'target_name': 'bravo_plugin',
    'type': 'none',
    'suppress_wildcard': 1,
    'include_dirs': [
      '..',
    ],
    'sources': [
      'plugin/exports.h',
      'plugin/exports.cc',
    ],
    'dependencies': [
      '../ppapi/ppapi.gyp:ppapi_c',
      'bravo_base',
    ],
    'defines': [
      'BRAVO_IMPLEMENTATION'
    ],
    'conditions': [
      ['os_posix==1 and OS!="mac"', {
        'cflags': ['-fvisibility=hidden'],
        'type': 'shared_library',
      }],
      ['OS=="win"', {
        'type': 'shared_library',
      }],
      ['OS=="mac"', {
        'type': 'loadable_module',
        'mac_bundle': 1,
        'product_name': 'ppGoogleBravoPluginChrome',
        'product_extension': 'plugin',
        'xcode_settings': {
          'OTHER_LDFLAGS': [
            # '-Wl,-exported_symbols_list <(DEPTH)/bravo/ppapi.def'
            # Not to strip important symbols by -Wl,-dead_strip.
            '-Wl,-exported_symbol,_PPP_GetInterface',
            '-Wl,-exported_symbol,_PPP_InitializeModule',
            '-Wl,-exported_symbol,_PPP_ShutdownModule'
          ],
        },
      }],
      ['OS=="mac" and mac_breakpad==1', {
        'variables': {
          # A real .dSYM is needed for dump_syms to operate on.
          'mac_real_dsym': 1,
        },
      }],
    ],
  },
  {
    'target_name': 'copy_bravo_plugin',
    'type': 'none',
    'dependencies': [
        'bravo_plugin',
    ],
    'conditions': [
        ['OS=="win"', {
            'copies': [{
                'destination': '<(PRODUCT_DIR)/plugins',
                'files': ['<(PRODUCT_DIR)/ppGoogleBravoPluginChrome.dll'],
            }],
        }],
        ['OS=="mac"', {
            'copies': [{
                'destination': '<(PRODUCT_DIR)/plugins/',
                'files': ['<(PRODUCT_DIR)/ppGoogleBravoPluginChrome.plugin/'],
            }],
        }],
        ['os_posix == 1 and OS != "mac"', {
            'copies': [{
                'destination': '<(PRODUCT_DIR)/plugins',
                'files': ['<(PRODUCT_DIR)/ppGoogleBravoPluginChrome.so'],
            }],
        }],
    ],
  }],
}
