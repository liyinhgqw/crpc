#include "net/base/ip_endpoint.h"
#include "net/base/net_util.h"

int main() {
  net::IPAddressNumber ipaddr;
  net::ParseIPLiteralToNumber("127.0.0.1", &ipaddr);
  net::IPEndPoint ep(ipaddr, 9090);
  printf("%s\n", ep.ToString().c_str());
}