#pragma once

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
  PrintFlagSystemTheme   = 0x1,
  PrintFlagAppTheme      = 0x2,
  PrintFlagNow           = 0x4,
  PrintFlagStandardNames = 0x8,
};

struct options {
  int print;
  int listen;
  int argcap;
  int argc;
  char **argv;
};

typedef void (*ThemeChangedCallback)(const char *name, unsigned flags,
                                     struct options *opts);

unsigned get_theme_flags();
int listen_for_theme_change(ThemeChangedCallback callback,
                            struct options *opts);

void print_theme_name(const char *name, unsigned flags, struct options *opts);

int yy_exec(char *const *argv);
