#pragma once

#include <stdbool.h>

// For strndup
#ifdef __STDC_ALLOC_LIB__
# define __STDC_WANT_LIB_EXT2__ 1
#else
# define _POSIX_C_SOURCE 200809L
#endif

enum ThemeFlags {
  ThemeFlagInitialValue = 0x1,
  ThemeFlagSystemLight  = 0x2,
  ThemeFlagSystemDark   = 0x4,
  ThemeFlagSystemMask   = 0x6,
  ThemeFlagAppLight     = 0x8,
  ThemeFlagAppDark      = 0x10,
  ThemeFlagAppMask      = 0x18,
};

enum PrintFlags {
  PrintFlagSystemTheme = 0x1,
  PrintFlagAppTheme    = 0x2,
  PrintFlagNow         = 0x4,
  PrintFlagSystemNames = 0x8,
};

struct options {
  int print;
  int listen;
  int argcap;
  int argc;
  char **argv;
};

typedef void (*ThemeChangedCallback)(unsigned flags, struct options *opts);
void theme_changed(unsigned flags, struct options *opts);

unsigned get_theme_flags();
const char *get_system_theme_name(bool isdark);
int listen_for_theme_change(ThemeChangedCallback callback,
                            struct options *opts);

int yy_exec(char *const *argv);
