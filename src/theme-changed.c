#include "yinyang.h"
#include <stdio.h>
#include <string.h>

void theme_changed(unsigned flags, struct options *opts) {
  if ((flags & ThemeFlagInitialValue) && !(opts->print & PrintFlagNow)) {
    return;
  }

  const char *sysname, *appname;
  if (opts->print & PrintFlagSystemNames) {
    sysname = get_system_theme_name(flags & ThemeFlagSystemDark);
    appname = get_system_theme_name(flags & ThemeFlagAppDark);
  } else {
    sysname = flags & ThemeFlagSystemDark ? "dark" : "light";
    appname = flags & ThemeFlagAppDark ? "dark" : "light";
  }

  if (opts->argc) {
    fputs("TODO: ", stdout);
    for (int i = 0; i < opts->argc; ++i) {
      if (!strcmp(opts->argv[i], "$sys")) {
        fputs(sysname, stdout);
      } else if (!strcmp(opts->argv[i], "$app")) {
        fputs(appname, stdout);
      } else {
        fputs(opts->argv[i], stdout);
      }
      fputc(' ', stdout);
    }
    fputc('\n', stdout);
  }

  switch (opts->print & (PrintFlagAppTheme | PrintFlagSystemTheme)) {
  case PrintFlagAppTheme:
    puts(appname);
    break;

  case PrintFlagSystemTheme:
    puts(sysname);
    break;

  case PrintFlagAppTheme | PrintFlagSystemTheme:
    printf("sys:%s\napp:%s\n", sysname, appname);
    break;

  default:
    break;
  }
}
