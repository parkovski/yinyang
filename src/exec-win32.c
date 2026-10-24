#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>

int yy_exec(char *const *argv) {
  size_t argsize = 0;
  bool *hasspace;
  int i, j, k;
  for (i = 0; argv[i]; ++i) {
  }
  hasspace = malloc(sizeof(bool) * i);
  for (i = 0; argv[i]; ++i) {
    hasspace[i] = false;
    for (j = 0; argv[i][j]; ++j) {
      ++argsize;
      if (argv[i][j] == ' ' || argv[i][j] == '\t') {
        hasspace[i] = true;
      }
    }
    if (hasspace[i]) argsize += 2;
    ++argsize;
  }
  char *args = malloc(argsize);

  k = 0;
  for (i = 0; argv[i]; ++i) {
    if (hasspace[i] && argv[i][0] != '\"') {
      args[k++] = '"';
    }
    j = 0;
    for (; argv[i][j]; ++j, ++k) {
      args[k] = argv[i][j];
    }
    if (hasspace[i] && argv[i][0] != '\"') {
      args[k++] = '"';
    }
    args[k++] = ' ';
  }
  args[k - 1] = '\0';
  free(hasspace);

  STARTUPINFO si;
  memset(&si, 0, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
  si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  PROCESS_INFORMATION pi;
  if (CreateProcessA(
      NULL, args, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL,
      &si, &pi) == 0) {
    pi.dwProcessId = 0;
  }
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
  free(args);
  return pi.dwProcessId;
}
