# gyp for executable

# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'target_defaults': {
    'include_dirs': [
      '.',
    ],
  },
  'includes': [
  ],
  'targets': [
    {
      'target_name': 'test',
      'type': 'executable',
      'toolsets': ['host', 'target'],
      'dependencies': [
        'base/base.gyp:base',
        'net/net.gyp:net',
      ],
      'sources': [
        'test/test_ip_endpoint.cc',
      ]
    },
  ],
}
