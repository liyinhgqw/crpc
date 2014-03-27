// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/base/net_util.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <errno.h>

#include "build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#include <iphlpapi.h>
#include <winsock2.h>
#pragma comment(lib, "iphlpapi.lib")
#elif defined(OS_POSIX)
#include <fcntl.h>
#if !defined(OS_ANDROID)
#include <ifaddrs.h>
#endif
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#endif

#include "base/basictypes.h"
#include "base/sys_byteorder.h"
#include "base/strings/stringprintf.h"

namespace net {

namespace {

#ifndef WIN32

int _itoa_s(int value, char* buffer, size_t size_in_chars, int radix) {
  const char* format_str;
  if (radix == 10)
    format_str = "%d";
  else if (radix == 16)
    format_str = "%x";
  else
    return EINVAL;

  int written = snprintf(buffer, size_in_chars, format_str, value);
  if (static_cast<size_t>(written) >= size_in_chars) {
    // Output was truncated, or written was negative.
    return EINVAL;
  }
  return 0;
}

// Secure template overloads for these functions
template<size_t N>
inline int _itoa_s(int value, char (&buffer)[N], int radix) {
  return _itoa_s(value, buffer, N, radix);
}

#endif  // !WIN32

void AppendIPv4Address(const unsigned char address[4], std::string& output) {
  for (int i = 0; i < 4; i++) {
    char str[16];
    _itoa_s(address[i], str, 10);

    for (int ch = 0; str[ch] != 0; ch++)
      output += str[ch];

    if (i != 3)
      output += ".";
  }
}

void AppendIPv6Address(const unsigned char address[4], std::string& output) {
  // Not implemented yet
}
} //  namespace

AddressFamily GetAddressFamily(const IPAddressNumber& address) {
  switch (address.size()) {
    case kIPv4AddressSize:
      return ADDRESS_FAMILY_IPV4;
    case kIPv6AddressSize:
      return ADDRESS_FAMILY_IPV6;
    default:
      return ADDRESS_FAMILY_UNSPECIFIED;
  }
}

int ConvertAddressFamily(AddressFamily address_family) {
  switch (address_family) {
    case ADDRESS_FAMILY_UNSPECIFIED:
      return AF_UNSPEC;
    case ADDRESS_FAMILY_IPV4:
      return AF_INET;
    case ADDRESS_FAMILY_IPV6:
      return AF_INET6;
  }
  NOTREACHED();
  return AF_UNSPEC;
}

// Extracts the address and port portions of a sockaddr.
bool GetIPAddressFromSockAddr(const struct sockaddr* sock_addr,
                              socklen_t sock_addr_len,
                              const uint8** address,
                              size_t* address_len,
                              uint16* port) {
  if (sock_addr->sa_family == AF_INET) {
    if (sock_addr_len < static_cast<socklen_t>(sizeof(struct sockaddr_in)))
      return false;
    const struct sockaddr_in* addr =
        reinterpret_cast<const struct sockaddr_in*>(sock_addr);
    *address = reinterpret_cast<const uint8*>(&addr->sin_addr);
    *address_len = kIPv4AddressSize;
    if (port)
      *port = base::NetToHost16(addr->sin_port);
    return true;
  }

  if (sock_addr->sa_family == AF_INET6) {
    if (sock_addr_len < static_cast<socklen_t>(sizeof(struct sockaddr_in6)))
      return false;
    const struct sockaddr_in6* addr =
        reinterpret_cast<const struct sockaddr_in6*>(sock_addr);
    *address = reinterpret_cast<const unsigned char*>(&addr->sin6_addr);
    *address_len = kIPv6AddressSize;
    if (port)
      *port = base::NetToHost16(addr->sin6_port);
    return true;
  }

  return false;  // Unrecognized |sa_family|.
}

std::string IPAddressToString(const uint8* address,
                              size_t address_len) {
  std::string str;

  if (address_len == kIPv4AddressSize) {
    AppendIPv4Address(address, str);
  } else if (address_len == kIPv6AddressSize) {
    AppendIPv6Address(address, str);
  } else {
    CHECK(false) << "Invalid IP address with length: " << address_len;
  }

  return str;
}

std::string IPAddressToStringWithPort(const uint8* address,
                                      size_t address_len,
                                      uint16 port) {
  std::string address_str = IPAddressToString(address, address_len);

  if (address_len == kIPv6AddressSize) {
    // Need to bracket IPv6 addresses since they contain colons.
    return base::StringPrintf("[%s]:%d", address_str.c_str(), port);
  }
  return base::StringPrintf("%s:%d", address_str.c_str(), port);
}

std::string NetAddressToString(const struct sockaddr* sa,
                               socklen_t sock_addr_len) {
  const uint8* address;
  size_t address_len;
  if (!GetIPAddressFromSockAddr(sa, sock_addr_len, &address,
                                &address_len, NULL)) {
    NOTREACHED();
    return std::string();
  }
  return IPAddressToString(address, address_len);
}

std::string NetAddressToStringWithPort(const struct sockaddr* sa,
                                       socklen_t sock_addr_len) {
  const uint8* address;
  size_t address_len;
  uint16 port;
  if (!GetIPAddressFromSockAddr(sa, sock_addr_len, &address,
                                &address_len, &port)) {
    NOTREACHED();
    return std::string();
  }
  return IPAddressToStringWithPort(address, address_len, port);
}

std::string IPAddressToString(const IPAddressNumber& addr) {
  return IPAddressToString(&addr.front(), addr.size());
}

std::string IPAddressToStringWithPort(const IPAddressNumber& addr,
                                      uint16 port) {
  return IPAddressToStringWithPort(&addr.front(), addr.size(), port);
}

std::string IPAddressToPackedString(const IPAddressNumber& addr) {
  return std::string(reinterpret_cast<const char *>(&addr.front()),
                     addr.size());
}

std::string GetHostName() {
#if defined(OS_WIN)
  EnsureWinsockInit();
#endif

  // Host names are limited to 255 bytes.
  char buffer[256];
  int result = gethostname(buffer, sizeof(buffer));
  if (result != 0) {
    DLOG(ERROR) << "gethostname() failed with " << result;
    buffer[0] = '\0';
  }
  return std::string(buffer);
}

bool ParseIPLiteralToNumber(const std::string& ip_literal,
                            IPAddressNumber* ip_number) {
  // |ip_literal| could be either a IPv4 or an IPv6 literal. If it contains
  // a colon however, it must be an IPv6 address.
  if (ip_literal.find(':') != std::string::npos) {
    // GURL expects IPv6 hostnames to be surrounded with brackets.
    // Try parsing the hostname as an IPv6 literal.
    ip_number->resize(16);  // 128 bits.
    // Not supported yet
    return false;
  }

  // Otherwise the string is an IPv4 address.
  ip_number->resize(4);  // 32 bits.

  int dot_idx = 0;
  std::string ip(ip_literal);
  std::string component = "";
  uint32 sum = 0, component_num = 0;
  for (int i = 0; i < 4; i ++) {
    if (i < 3) {
      dot_idx = ip.find_first_of('.');
      component = ip.substr(0, dot_idx);
      ip = ip.substr(dot_idx + 1);
      component_num = atoi(component.c_str());
    } else {
      component_num = atoi(ip.c_str());
    }
    sum = (sum << 8) + component_num;
  }
  sum = base::HostToNet32(sum);
  char* ptr = (char*) &sum;
  for (int i = 0; i < 4; i++) {
    (*ip_number)[i] = ptr[i];
  }
  return true;
}

} // namespace net