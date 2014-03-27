// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_BASE_NET_UTIL_H_
#define NET_BASE_NET_UTIL_H_

#include "build/build_config.h"
#include "net/base/address_family.h"
#include "base/logging.h"

#if defined(OS_WIN)
#include <windows.h>
#include <ws2tcpip.h>
#elif defined(OS_POSIX)
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include <string>
#include <vector>

namespace net {

// IPAddressNumber is used to represent an IP address's numeric value as an
// array of bytes, from most significant to least significant. This is the
// network byte ordering.
//
// IPv4 addresses will have length 4, whereas IPv6 address will have length 16.
typedef std::vector<unsigned char> IPAddressNumber;
typedef std::vector<IPAddressNumber> IPAddressList;

static const size_t kIPv4AddressSize = 4;
static const size_t kIPv6AddressSize = 16;

// Returns AddressFamily of the address.
AddressFamily GetAddressFamily(
    const IPAddressNumber& address);

// Maps the given AddressFamily to either AF_INET, AF_INET6 or AF_UNSPEC.
int ConvertAddressFamily(AddressFamily address_family);

// Parses an IP address literal (either IPv4 or IPv6) to its numeric value.
// Returns true on success and fills |ip_number| with the numeric value.
bool ParseIPLiteralToNumber(const std::string& ip_literal,
                                               IPAddressNumber* ip_number);


// Extracts the IP address and port portions of a sockaddr. |port| is optional,
// and will not be filled in if NULL.
bool GetIPAddressFromSockAddr(const struct sockaddr* sock_addr,
                              socklen_t sock_addr_len,
                              const unsigned char** address,
                              size_t* address_len,
                              uint16* port);

// Returns the string representation of an IP address.
// For example: "192.168.0.1" or "::1".
std::string IPAddressToString(const uint8* address,
                                         size_t address_len);

// Returns the string representation of an IP address along with its port.
// For example: "192.168.0.1:99" or "[::1]:80".
std::string IPAddressToStringWithPort(const uint8* address,
                                                 size_t address_len,
                                                 uint16 port);

// Same as IPAddressToString() but for a sockaddr. This output will not include
// the IPv6 scope ID.
std::string NetAddressToString(const struct sockaddr* sa,
                                          socklen_t sock_addr_len);

// Same as IPAddressToStringWithPort() but for a sockaddr. This output will not
// include the IPv6 scope ID.
std::string NetAddressToStringWithPort(const struct sockaddr* sa,
                                                  socklen_t sock_addr_len);

// Same as IPAddressToString() but for an IPAddressNumber.
std::string IPAddressToString(const IPAddressNumber& addr);

// Same as IPAddressToStringWithPort() but for an IPAddressNumber.
std::string IPAddressToStringWithPort(
    const IPAddressNumber& addr, uint16 port);

// Returns the address as a sequence of bytes in network-byte-order.
std::string IPAddressToPackedString(const IPAddressNumber& addr);

// Returns the hostname of the current system. Returns empty string on failure.
std::string GetHostName();

// Parses an IP address literal (either IPv4 or IPv6) to its numeric value.
// Returns true on success and fills |ip_number| with the numeric value.
bool ParseIPLiteralToNumber(const std::string& ip_literal,
                                               IPAddressNumber* ip_number);


} // namespace net

#endif  // NET_BASE_NET_UTIL_H_