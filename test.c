#include "stdio.h"
#include "stdint.h"

#include <windows.h>

int main() {
  MessageBoxA(NULL, "Win32 is working...", "Testing", MB_OK);
  return 0;
}
