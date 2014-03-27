#include "base/logging.h"


int main() {
  LOG(INFO) << "test level info";
  LOG(WARNING) << "test level warning";
  LOG(ERROR) << "test level error";
  // LOG(FATAL) << "test level fatal";

  CHECK(2 > 1);
  CHECK(2 < 1);
}