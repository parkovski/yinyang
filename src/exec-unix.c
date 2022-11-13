#include "yinyang.h"
#include <stdlib.h>
#include <spawn.h>

int yy_exec(char *const *argv) {
  int pid;
  posix_spawnp(&pid, argv[0], NULL, NULL, argv, NULL);
  return pid;
}
