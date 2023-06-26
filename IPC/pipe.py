import sys
import win32file
import win32pipe


EXECUTOR_PIPE_NAME = "executor_pipe"
BROKER_PIPE_NAME = "broker_pipe"
MESSAGE = b"Hello cpp!"

def broker_pipe_server():
  pipe = win32pipe.CreateNamedPipe(
    r"\\.\pipe\{broker_pipe_name}".format(
      broker_pipe_name=BROKER_PIPE_NAME
    ),
    win32pipe.PIPE_ACCESS_DUPLEX,
    win32pipe.PIPE_TYPE_MESSAGE
    | win32pipe.PIPE_READMODE_MESSAGE
    | win32pipe.PIPE_WAIT,
    1,
    65536,
    65536,
    0,
    None,
  )
  try:
    win32pipe.ConnectNamedPipe(pipe, None)
    resp = win32file.ReadFile(pipe, 64 * 1024)[1]
    win32file.FlushFileBuffers(pipe)
    win32pipe.DisconnectNamedPipe(pipe)
    win32file.CloseHandle(pipe)
  except Exception as e:
    win32pipe.DisconnectNamedPipe(pipe)
    print(f"exec CloseHandle")
    win32file.CloseHandle(pipe)
  print(resp)

def executor_pipe_client():
  try:
    handle = win32file.CreateFile(
      r"\\.\pipe\{executor_pipe_name}".format(
        executor_pipe_name=EXECUTOR_PIPE_NAME
      ),
      win32file.GENERIC_READ | win32file.GENERIC_WRITE,
      0,
      None,
      win32file.OPEN_EXISTING,
      0,
      None,
    )
    res = win32pipe.SetNamedPipeHandleState(
      handle, win32pipe.PIPE_READMODE_MESSAGE, None, None
    )
    if res == 0:
      print(f"SetNamedPipeHandleState return code: {res}")
    win32file.WriteFile(handle, MESSAGE)
    win32file.CloseHandle(handle)
  except Exception as e:
    print(e)

if __name__ == "__main__":
  if len(sys.argv) < 2:
    print(f"Please send args option : broker | executor")
    sys.exit(-1)
  elif sys.argv[1] == "broker":
    broker_pipe_server()
  elif sys.argv[1] == "executor":
    executor_pipe_client()
  else:
    print(f"Unknowned args option...")
    print(f"Please send args option : broker | executor")
    sys.exit(-1)
