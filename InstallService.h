#ifndef __InstallService__
#define __InstallService__

#include <windows.h>
#include <winsvc.h>
#include <stdio.h>


bool InstallService(LPCSTR lpServiceName) {
  SC_HANDLE schManager(nullptr);
  SC_HANDLE hService(nullptr);
  bool retValue(false);
  TCHAR szPath[MAX_PATH];

  if(!GetModuleFileName(NULL, szPath, MAX_PATH)) {
    printf("Cannot install service (%d)\n", GetLastError());
    return false;
  }

  schManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

  if(schManager == NULL) {
    printf("OpenSCManager failed (%d)\n", GetLastError());
    CloseServiceHandle(schManager);
    return false;
  }

  hService = CreateService(
    schManager,                // SCM database
    lpServiceName,             // name of service
    lpServiceName,             // service name to display
    SERVICE_ALL_ACCESS,        // desired access
    SERVICE_WIN32_OWN_PROCESS, // service type
    SERVICE_DEMAND_START,      // start type
    SERVICE_ERROR_NORMAL,      // error control type
    szPath,                    // path to service's binary
    NULL,                      // no load ordering group
    NULL,                      // no tag identifier
    NULL,                      // no dependencies
    NULL,                      // LocalSystem account
    NULL);                     // no password

  if(hService == NULL)
  {
    printf("CreateService failed (%d)\n", GetLastError());
    CloseServiceHandle(schManager);
    return false;
  }

  printf("Service installed successfully\n");
  CloseServiceHandle(schManager);
  CloseServiceHandle(hService);

  return true;
}

#endif