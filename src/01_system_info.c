#include "learnuv.h"

int main()
{
  log_info("01_system_info");
  int err;

  double uptime;
  err = uv_uptime(&uptime);
  CHECK(err, "uv_uptime");
  log_info("Uptime: %f", uptime);
  log_report("Uptime: %f", uptime);

  size_t resident_set_memory;
  err = uv_resident_set_memory(&resident_set_memory);
  CHECK(err, "uv_uptime");

  log_info("RSS: %ld", resident_set_memory);
  log_report("RSS: %ld", resident_set_memory);

  return 0;
}
