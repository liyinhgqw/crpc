# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'target_defaults': {
    'variables': {
      'base_target': 0,
    },
    'target_conditions': [
      # This part is shared between the targets defined below.
      ['base_target==1', {
        'sources': [
          'atomicops.h',
          'atomicops_internals_atomicword_compat.h',
          'atomicops_internals_mac.h',
          'basictypes.h',
          'build/build_config.h',
          'callback.h',
          'callback.cc',
          'compiler_specific.h',
          'logging.h',
          'logging.cc',
          'macros.h',
          'mutex.h',
          'mutex.cc',
          'mutexlock.h',
          'once.h',
          'once.cc',
          'port.h',
          'shutdown.h',
        ],
        'defines': [
          'BASE_IMPLEMENTATION',
        ],
        'include_dirs': [
          '..',
        ],
        'msvs_disabled_warnings': [
          4018,
        ],
      }],
    ],
  },
}
