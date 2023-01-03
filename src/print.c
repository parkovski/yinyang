#include "yinyang.h"
#include <stdio.h>

void print_theme_name(const char *name, unsigned flags, struct options *opts) {
  if ((flags & ThemeFlagInitialValue) && !(opts->print & PrintFlagNow)) {
    return;
  }

  if (opts->argc) {
    fputs("TODO: ", stdout);
    for (int i = 0; i < opts->argc; ++i) {
      fputs(opts->argv[i], stdout);
      fputc(' ', stdout);
    }
    fputc('\n', stdout);
  }

  switch (opts->print & (PrintFlagAppTheme | PrintFlagSystemTheme)) {
  case PrintFlagAppTheme:
    if (flags & ThemeFlagAppDark) {
      if (opts->print & PrintFlagStandardNames) {
        puts("dark");
      } else {
        puts(name);
      }
    } else if (flags & ThemeFlagAppLight) {
      if (opts->print & PrintFlagStandardNames) {
        puts("light");
      } else {
        puts(name);
      }
#ifndef NDEBUG
    } else {
      fprintf(stderr, "App theme flag not present.\n");
#endif
    }
    break;

  case PrintFlagSystemTheme:
    if (flags & ThemeFlagSystemDark) {
      if (opts->print & PrintFlagStandardNames) {
        puts("dark");
      } else {
        puts(name);
      }
    } else if (flags & ThemeFlagSystemLight) {
      if (opts->print & PrintFlagStandardNames) {
        puts("light");
      } else {
        puts(name);
      }
#ifndef NDEBUG
    } else {
      fprintf(stderr, "System theme flag not present.\n");
#endif
    }
    break;

  default: // Both flags
    if (flags & ThemeFlagSystemDark) {
      if (opts->print & PrintFlagStandardNames) {
        puts("sys:dark");
      } else {
        printf("sys:%s\n", name);
      }
    } else if (flags & ThemeFlagSystemLight) {
      if (opts->print & PrintFlagStandardNames) {
        puts("sys:light");
      } else {
        printf("sys:%s\n", name);
      }
#ifndef NDEBUG
    } else {
      fprintf(stderr, "App theme flag not present.\n");
#endif
    }

    if (flags & ThemeFlagAppDark) {
      if (opts->print & PrintFlagStandardNames) {
        puts("app:dark");
      } else {
        printf("app:%s\n", name);
      }
    } else if (flags & ThemeFlagAppLight) {
      if (opts->print & PrintFlagStandardNames) {
        puts("app:light");
      } else {
        printf("app:%s\n", name);
      }
#ifndef NDEBUG
    } else {
      fprintf(stderr, "App theme flag not present.\n");
#endif
    }
    break;
  }
}
