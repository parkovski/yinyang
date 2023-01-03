#include "yinyang.h"
#include <stdio.h>

void print_theme_name(const char *name, unsigned flags, struct options *opts) {
  if ((flags & ThemeFlagInitialValue) && !(opts->print & PrintFlagNow)) {
    return;
  }

  if (name == NULL) {
    opts->print &= ~PrintFlagSystemNames;
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
      if (opts->print & PrintFlagSystemNames) {
        puts(name);
      } else {
        puts("dark");
      }
    } else if (flags & ThemeFlagAppLight) {
      if (opts->print & PrintFlagSystemNames) {
        puts(name);
      } else {
        puts("light");
      }
#ifndef NDEBUG
    } else {
      fprintf(stderr, "App theme flag not present.\n");
#endif
    }
    break;

  case PrintFlagSystemTheme:
    if (flags & ThemeFlagSystemDark) {
      if (opts->print & PrintFlagSystemNames) {
        puts(name);
      } else {
        puts("dark");
      }
    } else if (flags & ThemeFlagSystemLight) {
      if (opts->print & PrintFlagSystemNames) {
        puts(name);
      } else {
        puts("light");
      }
#ifndef NDEBUG
    } else {
      fprintf(stderr, "System theme flag not present.\n");
#endif
    }
    break;

  default: // Both flags
    if (flags & ThemeFlagSystemDark) {
      if (opts->print & PrintFlagSystemNames) {
        printf("sys:%s\n", name);
      } else {
        puts("sys:dark");
      }
    } else if (flags & ThemeFlagSystemLight) {
      if (opts->print & PrintFlagSystemNames) {
        printf("sys:%s\n", name);
      } else {
        puts("sys:light");
      }
#ifndef NDEBUG
    } else {
      fprintf(stderr, "System theme flag not present.\n");
#endif
    }

    if (flags & ThemeFlagAppDark) {
      if (opts->print & PrintFlagSystemNames) {
        printf("app:%s\n", name);
      } else {
        puts("app:dark");
      }
    } else if (flags & ThemeFlagAppLight) {
      if (opts->print & PrintFlagSystemNames) {
        printf("app:%s\n", name);
      } else {
        puts("app:light");
      }
#ifndef NDEBUG
    } else {
      fprintf(stderr, "App theme flag not present.\n");
#endif
    }
    break;
  }
}
