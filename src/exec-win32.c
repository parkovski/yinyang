#include <Windows.h>
#include <stdio.h>

int yy_exec(char *const *argv) {
  size_t argsize = 2;
  for (int i = 0; argv[i]; ++i) {
    argsize += strlen(argv[i]) + 1;
  }
  char *args = malloc(argsize);

  size_t offset = 1;
  args[0] = '"';
  size_t length = strlen(argv[0]);
  memcpy(args + 1, argv[0], length);
  offset += length;
  args[offset] = '"';
  ++offset;
  args[offset] = ' ';
  ++offset;

  for (int i = 1; argv[i]; ++i) {
    length = strlen(argv[i]);
    memcpy(args + offset, argv[i], length);
    offset += length;
    args[offset] = ' ';
    ++offset;
  }
  args[offset - 1] = 0;

  printf("args: %s\n", args);

  STARTUPINFO si;
  memset(&si, 0, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  PROCESS_INFORMATION pi;
  if (CreateProcessA(
      NULL, args, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL,
      &si, &pi) == 0) {
    pi.dwProcessId = 0;
  }
  free(args);
  return pi.dwProcessId;
}
