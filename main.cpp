#include <stdio.h>
#include "InstallService.h"
#include "DeleteService.h"

int main() {
  LPCSTR serviceName = "A_test_service";

  InstallService(serviceName);
  DeleteService(serviceName);

  return 0;
}