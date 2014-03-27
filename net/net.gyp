# gyp for static_library

# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'target_defaults': {
    'include_dirs': [
      '..',
    ],
  },
  'includes': [
  ],
  'targets': [
    {
      'target_name': 'net',
      'type': 'static_library',
      'toolsets': ['host', 'target'],
      'dependencies': [
        '../base/base.gyp:base',
      ],
      'sources': [
        'base/address_family.h',
        'base/ip_endpoint.cc',
        'base/ip_endpoint.h',
        'base/net_util.h',
        'base/net_util.cc',
        'base/sys_byteorder.h',
        'base/tcp_socket_libevent.h',
      ],
    },
  ],
}
