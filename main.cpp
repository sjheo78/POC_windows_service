#include <stdio.h>
#include ".\InstallService.cpp"
#include ".\DeleteService.cpp"

int main() {
  LPCSTR serviceName = "A_test_service";

  InstallService(serviceName);
  DeleteService(serviceName);

  return 0;
}