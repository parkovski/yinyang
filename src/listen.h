#pragma once

enum {
  ThemeFlagInitialValue = 1,
  ThemeFlagSystemLight  = 2,
  ThemeFlagSystemDark   = 4,
  ThemeFlagSystemMask   = 6,
  ThemeFlagAppLight     = 8,
  ThemeFlagAppDark      = 16,
  ThemeFlagAppMask      = 24,
};

typedef void (*ThemeChangedCallback)(const char *name, unsigned flags);

unsigned get_theme_flags();
int listen_for_theme_change(ThemeChangedCallback callback);
