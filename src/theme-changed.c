#include "yinyang.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *const var_sys_theme = "$sys_theme";
static const char *const var_app_theme = "$app_theme";
static const size_t var_len = 10;

static void do_exec(int argc, char *const *argv, const char *sysname,
                    const char *appname) {
  char **new_argv = malloc(sizeof(char*) * (argc + 1));
  for (int i = 0; i < argc; ++i) {
    const char *replace = sysname;
    char *var = strstr(argv[i], var_sys_theme);
    if (var == NULL) {
      var = strstr(argv[i], var_app_theme);
      replace = appname;
    }

    if (var != NULL) {
      size_t left_len = (size_t)(var - argv[i]);
      size_t replace_len = strlen(replace);
      size_t right_len = strlen(var + var_len);
      new_argv[i] = malloc(left_len + replace_len + right_len + 1);
      memcpy(new_argv[i], argv[i], left_len);
      memcpy(new_argv[i] + left_len, replace, replace_len);
      memcpy(new_argv[i] + left_len + replace_len, var + var_len, right_len);
      new_argv[i][left_len + replace_len + right_len] = 0;
    } else {
      new_argv[i] = strdup(argv[i]);
    }
  }
  new_argv[argc] = 0;

  yy_exec(new_argv);

  for (int i = 0; i < argc; ++i) {
    free(new_argv[i]);
  }
  free(new_argv);
}

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
    do_exec(opts->argc, opts->argv, sysname, appname);
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
