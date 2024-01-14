#include <Windows.h>
#include <stdio.h>

int yy_exec(char *const *argv) {
  size_t argsize = 0;
  for (int i = 1; argv[i]; ++i) {
    argsize += strlen(argv[i]) + 1;
  }
  char *args = malloc(argsize);
  size_t offset = 0;
  for (int i = 1; argv[i]; ++i) {
    size_t length = strlen(argv[i]);
    memcpy(args + offset, argv[i], length);
    offset += length;
    args[offset] = ' ';
    ++offset;
  }
  args[offset - 1] = 0;

  printf("args: %s \"%s\"\n", argv[0], args);

  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  if (CreateProcessA(
      argv[0], args, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL,
      &si, &pi) == 0) {
    pi.dwProcessId = 0;
  }
  free(args);
  return pi.dwProcessId;
}
