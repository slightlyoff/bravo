// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BRAVO_BRAVO_EXPORT_H_
#define BRAVO_BRAVO_EXPORT_H_

#if defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(BRAVO_IMPLEMENTATION)
#define BRAVO_EXPORT __declspec(dllexport)
#else
#define BRAVO_EXPORT __declspec(dllimport)
#endif  // defined(BRAVO_IMPLEMENTATION)

#else // defined(WIN32)
#if defined(BRAVO_IMPLEMENTATION)
#define BRAVO_EXPORT __attribute__((visibility("default")))
#else
#define BRAVO_EXPORT
#endif
#endif

#else // defined(COMPONENT_BUILD)
#define BRAVO_EXPORT
#endif

#endif  // BRAVO_BRAVO_EXPORT_H_
