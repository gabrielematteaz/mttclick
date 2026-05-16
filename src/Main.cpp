#include <Windows.h>
#include <array>
#include <system_error>
#include <cstdlib>
#include <exception>
#include <cstdio>
#include <print>

const int ID_EXIT = 1;
const int ID_START = 2;
const int ID_STOP = 3;
UINT_PTR timerID = 0;

void Click() {
  std::array < INPUT, 2 > inputs{ };

  inputs[0].type = INPUT_MOUSE;
  inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

  inputs[1].type = INPUT_MOUSE;
  inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

  SendInput(static_cast < UINT > (inputs.size()), inputs.data(), sizeof(INPUT));
}

int WINAPI wWinMain(_In_ HINSTANCE, _In_ HINSTANCE, _In_opt_ LPWSTR, _In_ int) {
  LPWSTR *arguments = NULL;

  try {
    auto commandLine = GetCommandLineW();
    int argumentCount;
    arguments = CommandLineToArgvW(commandLine, &argumentCount);
    UINT milliseconds = 50;

    if (argumentCount > 1) {
      milliseconds = wcstoul(arguments[1], NULL, 10);
    }

    if (arguments == NULL) {
      throw std::system_error(GetLastError(), std::system_category(), "CommandLineToArgvW");
    }

    if (!RegisterHotKey(NULL, ID_EXIT, MOD_CONTROL | MOD_NOREPEAT, VK_MULTIPLY)) {
      throw std::system_error(GetLastError(), std::system_category(), "RegisterHotKey");
    }
    
    if (!RegisterHotKey(NULL, ID_START, MOD_CONTROL | MOD_NOREPEAT, VK_SUBTRACT)) {
      throw std::system_error(GetLastError(), std::system_category(), "RegisterHotKey");
    }
    
    if (!RegisterHotKey(NULL, ID_STOP, MOD_CONTROL | MOD_NOREPEAT, VK_ADD)) {
      throw std::system_error(GetLastError(), std::system_category(), "RegisterHotKey");
    }

    MSG message;
    while (GetMessageW(&message, NULL, 0, 0)) {
      if (message.message == WM_HOTKEY) {
        int ID = (int)message.wParam;

        if (ID == ID_EXIT) {
          break;
        }

        if (ID == ID_START) {
          if (timerID == 0) {
            timerID = SetTimer(NULL, 0, milliseconds, NULL); 
          }
        }
        else if (ID == ID_STOP) {
          if (timerID != 0) {
            KillTimer(NULL, timerID);
            timerID = 0;
          }
        }
      }
      else if (message.message == WM_TIMER && message.wParam == timerID) {
        Click();
      }

      TranslateMessage(&message);
      DispatchMessageW(&message);
    }
  }
  catch (const std::exception& exception) {
    auto file = std::fopen("mttclick.log", "w");
    if (file != NULL) {
      std::println(file, "{}", exception.what());
      std::fclose(file);
    }
  }

  if (timerID != 0) {
    KillTimer(NULL, timerID);
  }

  UnregisterHotKey(NULL, ID_START);
  UnregisterHotKey(NULL, ID_STOP);
  UnregisterHotKey(NULL, ID_EXIT);
  LocalFree(arguments);

  return 0;
}