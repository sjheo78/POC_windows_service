#ifndef __UninstallService__
#define __UninstallService__

#include <windows.h>
#include <winsvc.h>
#include <stdio.h>


bool UninstallService(LPCSTR lpServiceName) {
  SC_HANDLE schManager;
  SC_HANDLE schService;

  schManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

  if (NULL == schManager)
  {
    printf("OpenSCManager failed (%d)\n", GetLastError());
    CloseServiceHandle(schManager);
    return false;
  }

  schService = OpenService(schManager, lpServiceName, DELETE);

  if (schService == NULL) {
    printf("OpenService failed (%d)\n", GetLastError());
    CloseServiceHandle(schManager);
    return false;
  }

  if(!DeleteService(schService)) {
    printf("DeleteService failed (%d)\n", GetLastError());
    CloseServiceHandle(schManager);
    CloseServiceHandle(schService);
    return false;
  }

  printf("Service deleted successfully\n");
  CloseServiceHandle(schManager);
  CloseServiceHandle(schService);

  return true;
}

#endif