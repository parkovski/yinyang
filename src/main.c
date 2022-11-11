#include "listen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void print_theme_name(const char *name, unsigned flags) {
  printf("%s\n", name);
}

static void show_help() {
  fprintf(stderr, "help!\n");
  fprintf(stderr, "-h | --help\n-l | --listen\n-s | --system\n"
                  "-a | --app\n-n | --now\n-x | --exec\n");
}

enum print_flags {
  print_sys = 1,
  print_app = 2,
  print_now = 4,
};

struct options {
  int print;
  int listen;
  int argcap;
  int argc;
  char **argv;
};

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
    opts->print |= print_sys;
  } else if (!strcmp(arg, "app")) {
    opts->print |= print_app;
  } else if (!strcmp(arg, "now")) {
    opts->print |= print_now;
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
      opts->print |= print_sys;
      break;

    case 'a':
      opts->print |= print_app;
      break;

    case 'n':
      opts->print |= print_now;
      break;

    case 'x':
      if (argv[*argn][i+1]) {
        set_exec(&argv[*argn][i+1], opts);
      } else if (*argn + 1 < argc) {
        set_exec(argv[++*argn], opts);
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
    return listen_for_theme_change(print_theme_name);
  } else if (opts.print & print_now) {
    int flags = get_theme_flags() & ThemeFlagSystemMask;
    print_theme_name(flags == ThemeFlagSystemDark ? "dark" : "light", 0);
  }

  for (int i = 0; i < opts.argc; ++i) {
    free(opts.argv[i]);
  }
  free(opts.argv);
  return 0;
}
