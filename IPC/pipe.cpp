#include <windows.h>
#include <conio.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>


#define BUFSIZE 4096

const LPTSTR gclpszExecutorPipe = const_cast<LPTSTR>("\\\\.\\pipe\\executor_pipe");
const LPTSTR gclpszBrokerPipe = const_cast<LPTSTR>("\\\\.\\pipe\\broker_pipe");

HANDLE ExecutorPipeServer() {
  HANDLE hPipe;
  BOOL fConnected = FALSE, fSuccess = FALSE;
  DWORD cbBytesRead = 0;

  hPipe = CreateNamedPipe(
    gclpszExecutorPipe,
    PIPE_ACCESS_DUPLEX,
    PIPE_TYPE_MESSAGE |
    PIPE_READMODE_MESSAGE |
    PIPE_WAIT,
    PIPE_UNLIMITED_INSTANCES,
    BUFSIZE,
    BUFSIZE,
    0,
    NULL
  );

  if(hPipe == INVALID_HANDLE_VALUE) {
    printf("CreateNamedPipe failed : %d\n", GetLastError());
    return INVALID_HANDLE_VALUE;
  }

  printf("[*] Waiting connection for client...\n");
  while(!(ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED)));

  TCHAR* pchRequest = (TCHAR*)malloc(BUFSIZE*sizeof(TCHAR));

  fSuccess = ReadFile(
    hPipe,
    pchRequest,
    BUFSIZE * sizeof(TCHAR),
    &cbBytesRead,
    NULL
  );

  if(!fSuccess || cbBytesRead == 0) {
    if(GetLastError() == ERROR_BROKEN_PIPE) {
      printf("[-] client disconnected\n");
      return INVALID_HANDLE_VALUE;
    }
    else {
      printf("[-] ReadFile failed : %d\n", GetLastError());
      return INVALID_HANDLE_VALUE;
    }
  }

  printf("ExecutorPipeServer succeed : %s\n", pchRequest);

  FlushFileBuffers(hPipe);
  DisconnectNamedPipe(hPipe);
  CloseHandle(hPipe);

  free(pchRequest);

  return hPipe;
}

HANDLE BrokerPipeServer() {
  HANDLE hPipe;
  LPTSTR lpvMessage = const_cast<LPTSTR>("Hello python!");
  TCHAR chBuf[BUFSIZE];
  BOOL fSuccess = FALSE;
  DWORD cbRead, cbToWrite, cbWritten, dwMode;

  hPipe = CreateFile(
    gclpszBrokerPipe,
    GENERIC_READ | GENERIC_WRITE,
    0,
    NULL,                           // TODO, security parms
    OPEN_EXISTING,
    0,
    NULL
  );

  if(GetLastError() == ERROR_PIPE_BUSY) {
    printf("Could not open pipe : %d\n", GetLastError());
    return INVALID_HANDLE_VALUE;
  }

  if(WaitNamedPipe(gclpszExecutorPipe, 20000)) {
    printf("Could not open pipe : 20 second wait timed out.\n");
    return INVALID_HANDLE_VALUE;
  }

  dwMode = PIPE_READMODE_MESSAGE;
  fSuccess = SetNamedPipeHandleState(
    hPipe,
    &dwMode,
    NULL,
    NULL
  );

  if(!fSuccess) {
    printf("SetNamedPipeHandleState failed : %d\n", GetLastError());
    return INVALID_HANDLE_VALUE;
  }

  cbToWrite = (lstrlen(lpvMessage)+1)*sizeof(TCHAR);
  printf("Sending %d byte message : \"%s\"\n", cbToWrite, lpvMessage);

  fSuccess = WriteFile(
    hPipe,
    lpvMessage,
    cbToWrite,
    &cbWritten,
    NULL
  );

  if(!fSuccess) {
    printf("WriteFile to pipe failed : %d\n", GetLastError());
    return INVALID_HANDLE_VALUE;
  }

  CloseHandle(hPipe);
  printf("BrokerPipeServer succeed\n");
  return hPipe;
}

/*
m_ps = new PipeSecurity();
                m_ps.AddAccessRule(
                    new PipeAccessRule(WindowsIdentity.GetCurrent().User, PipeAccessRights.FullControl, AccessControlType.Allow)
                );
                m_ps.AddAccessRule(
                    new PipeAccessRule(
                        new SecurityIdentifier(WellKnownSidType.AuthenticatedUserSid, null), PipeAccessRights.ReadWrite, AccessControlType.Allow
                    )
                );
*/

int main(int argc, char **argv) {
  printf("[*] Run main\n");

  if(argc < 1) {
    printf("Please send args option : broker | executor");
    return -1;
  }
  else if(strcmp(argv[1], "broker") == 0) {
    HANDLE hBrokerPipe = INVALID_HANDLE_VALUE;
    hBrokerPipe = BrokerPipeServer();
    if(hBrokerPipe == INVALID_HANDLE_VALUE)
      return -1;
  }
  else if(strcmp(argv[1], "executor") == 0) {
    HANDLE hExecutorPipe = INVALID_HANDLE_VALUE;
    hExecutorPipe = ExecutorPipeServer();
    if(hExecutorPipe == INVALID_HANDLE_VALUE)
      return -1;
  }
  else {
    printf("Unknowned args option...");
    printf("Please send args option : broker | executor");
  }

  return 0;
}