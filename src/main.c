#include "yinyang.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void show_help() {
  fprintf(stderr, "Usage: yinyang [options] [-x | --exec command...]\n\n"
          "Options:\n"
          "  -h | --help     Show this message.\n"
          "  -n | --now      Print the current theme name.\n"
          "  -l | --listen   Listen for system theme changes.\n"
          "  -s | --system   Print the system theme.\n"
          "  -a | --app      Print the app theme.\n"
          "  -S | --system-names  Use system-specific names instead of"
                                 " 'dark' / 'light'.\n"
#if defined(YY_HAS_GTK) && defined(YY_HAS_KDE)
          "  -e | --env      Specify the desktop environment (GNOME, KDE).\n"
#endif
          "  -x | --exec     Run a command when a theme change occurs."
                            " Separate arguments with ',': '-xfoo,bar'.\n"
          "                  Arguments $sys_theme and $app_theme are"
                            " substituted with the corresponding theme name.\n"
          "\n"
          "If no options are specified, the default behavior is as if the"
          " arguments '-n' and '-a' were passed.\n");
}

static void push_exec_arg(const char *arg, size_t len, struct options *opts) {
  if (opts->argc == opts->argcap) {
    if (opts->argcap < 8) {
      opts->argcap = 8;
    } else {
      opts->argcap *= 2;
    }
    opts->argv = realloc(opts->argv, opts->argcap * sizeof(char*));
  }
#if UNIX
  opts->argv[opts->argc++] = strndup(arg, len);
#else
  opts->argv[opts->argc] = malloc(len + 1);
  memcpy(opts->argv[opts->argc], arg, len);
  opts->argv[opts->argc][len] = 0;
  ++opts->argc;
#endif
}

static void set_exec(const char *cmd, struct options *opts) {
  const char *arg = cmd;
  size_t ofs = 0;
  while (1) {
    if (arg[ofs] == 0) {
      push_exec_arg(arg, ofs, opts);
      break;
    } else if (arg[ofs] == ',') {
      push_exec_arg(arg, ofs, opts);
      arg = arg + ofs + 1;
      ofs = 0;
    } else {
      ++ofs;
    }
  }
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
  } else if (!strcmp(arg, "system-names")) {
    opts->print |= PrintFlagSystemNames;
  } else if (!strcmp(arg, "app")) {
    opts->print |= PrintFlagAppTheme;
  } else if (!strcmp(arg, "now")) {
    opts->print |= PrintFlagNow;
  } else if (!strcmp(arg, "env")) {
    if (argc == *argn + 1) {
      fprintf(stderr, "expected argument after '--env'\n");
      return 1;
    }
    if ((opts->env = get_env(argv[++*argn])) == NULL) {
      fprintf(stderr, "unknown env '%s'\n", argv[*argn]);
      return 1;
    }
  } else if (!strncmp(arg, "env=", 4)) {
    if (arg[4] == 0) {
      fprintf(stderr, "expected argument after '--env'\n");
      return 1;
    }
    if ((opts->env = get_env(arg + 4)) == NULL) {
      fprintf(stderr, "unknown env '%s'\n", arg + 4);
      return 1;
    }
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
      opts->print |= PrintFlagSystemNames;
      break;

    case 'a':
      opts->print |= PrintFlagAppTheme;
      break;

    case 'n':
      opts->print |= PrintFlagNow;
      break;

    case 'e': {
      char *envname;
      if (argv[*argn][i+1]) {
        envname = &argv[*argn][i+1];
      } else if (*argn + 1 < argc) {
        envname = argv[++*argn];
      } else {
        fputs("Expected argument after -e", stderr);
        return 1;
      }
      if ((opts->env = get_env(envname)) == NULL) {
        fprintf(stderr, "Unknown env '%s'\n", envname);
        return 1;
      }
      return 0;
    }

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

  if (opts.env == NULL) {
    opts.env = get_env(NULL);
  }

  if (opts.listen) {
    return opts.env->listen_for_theme_change(&opts);
  } else {
    int flags = opts.env->get_theme_flags();
    if (!(opts.print & (PrintFlagSystemTheme | PrintFlagAppTheme))) {
      opts.print |= PrintFlagAppTheme;
    }
    theme_changed(flags, &opts);
  }

  for (int i = 0; i < opts.argc; ++i) {
    free(opts.argv[i]);
  }
  free(opts.argv);
  return 0;
}
