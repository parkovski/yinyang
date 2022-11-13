#include "yinyang.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void print_theme_name(const char *name, unsigned flags,
                             struct options *opts) {
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
      fprintf(stderr, "App theme flag not present.");
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
      fprintf(stderr, "System theme flag not present.");
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
      fprintf(stderr, "App theme flag not present.");
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
      fprintf(stderr, "App theme flag not present.");
#endif
    }
    break;
  }
}

static void show_help() {
  fprintf(stderr, "Usage: yinyang [options] [-x | --exec command...]\n\n"
          "Options:\n"
          "  -h | --help     Show this message.\n"
          "  -n | --now      Print the current theme name.\n"
          "  -l | --listen   Listen for system theme changes.\n"
          "  -s | --system   Print the system theme.\n"
          "  -a | --app      Print the app theme.\n"
          "  -S | --standard-names   Use standard names 'dark'/'light'"
                                    " instead of system specific names.\n"
          "  -x | --exec     Run a command when a theme change occurs."
                            " Arguments $system and $app are substituted with"
                            " the theme name.\n\n"
          "If no options are specified, the default behavior is as if the"
          " arguments '-n', '-a', and '-S' were passed.\n");
}

static void set_exec(const char *cmd, struct options *opts) {
  if (opts->argc == opts->argcap) {
    if (opts->argcap < 8) {
      opts->argcap = 8;
    } else {
      opts->argcap *= 2;
    }
    opts->argv = realloc(opts->argv, opts->argcap * sizeof(char*));
  }
  opts->argv[opts->argc++] = strndup(cmd, 1023);
}

static int long_arg(int argc, char *argv[], int *argn, struct options *opts) {
  const char *arg = argv[*argn] + 2; // Skip the '--'
  if (*arg == 0) {
  } else if (!strcmp(arg, "help")) {
    show_help();
    return -1;
  } else if (!strcmp(arg, "listen")) {
    opts->listen = 1;
  } else if (!strcmp(arg, "system")) {
    opts->print |= PrintFlagSystemTheme;
  } else if (!strcmp(arg, "standard-names")) {
    opts->print |= PrintFlagStandardNames;
  } else if (!strcmp(arg, "app")) {
    opts->print |= PrintFlagAppTheme;
  } else if (!strcmp(arg, "now")) {
    opts->print |= PrintFlagNow;
  } else if (!strcmp(arg, "exec")) {
    if (argc == *argn + 1) {
      fprintf(stderr, "expected argument after '--exec'\n");
      return 1;
    }
    set_exec(argv[++*argn], opts);
  } else if (!strncmp(arg, "exec=", 5)) {
    if (arg[5] == 0) {
      fprintf(stderr, "expected argument after '--exec='\n");
      return 1;
    }
    set_exec(arg + 5, opts);
  } else {
    fprintf(stderr, "bad argument '--%s'\n", arg);
    return 1;
  }
  return 0;
}

static int short_arg(int argc, char *argv[], int *argn, struct options *opts) {
  for (int i = 1;; ++i) {
    switch (argv[*argn][i]) {
    default:
      fprintf(stderr, "bad argument '-%c'\n", argv[*argn][i]);
      return 1;

    case 0:
      if (i == 1) {
        fprintf(stderr, "bad argument '-'\n");
        return 1;
      }
      return 0;

    case 'h':
      show_help();
      return -1;

    case 'l':
      opts->listen = 1;
      break;

    case 's':
      opts->print |= PrintFlagSystemTheme;
      break;

    case 'S':
      opts->print |= PrintFlagStandardNames;
      break;

    case 'a':
      opts->print |= PrintFlagAppTheme;
      break;

    case 'n':
      opts->print |= PrintFlagNow;
      break;

    case 'x':
      if (argv[*argn][i+1]) {
        set_exec(&argv[*argn][i+1], opts);
        return 0;
      } else if (*argn + 1 < argc) {
        set_exec(argv[++*argn], opts);
        return 0;
      } else {
        fputs("Expected argument after -x", stderr);
        return 1;
      }
      break;
    }
  }
}

static int parse_args(int argc, char *argv[], struct options *opts) {
  int ret = 0;
  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == '-') {
        ret = long_arg(argc, argv, &i, opts);
      } else {
        ret = short_arg(argc, argv, &i, opts);
      }
      if (ret) {
        break;
      }
    } else {
      fprintf(stderr, "bad argument %s\n", argv[i]);
    }
  }
  return ret;
}

int main(int argc, char *argv[]) {
  struct options opts;
  memset(&opts, 0, sizeof(opts));
  int ret = parse_args(argc, argv, &opts);
  if (ret > 0) {
    return ret;
  } else if (ret < 0) {
    return 0;
  }

  if (opts.listen) {
    return listen_for_theme_change(print_theme_name, &opts);
  } else {
    int flags = get_theme_flags();
    if (!(opts.print & (PrintFlagSystemTheme | PrintFlagAppTheme))) {
      opts.print |= PrintFlagAppTheme;
    }
    opts.print |= PrintFlagStandardNames;
    print_theme_name(NULL, flags, &opts);
  }

  for (int i = 0; i < opts.argc; ++i) {
    free(opts.argv[i]);
  }
  free(opts.argv);
  return 0;
}
