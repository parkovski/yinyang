#include "yinyang.h"
#include <Windows.h>

static void is_dark_mode(BOOL *system, BOOL *apps) {
  static HANDLE uxtheme_dll = NULL;
  if (uxtheme_dll == NULL) {
    uxtheme_dll = LoadLibraryW(L"uxtheme.dll");
    if (!uxtheme_dll) {
      *system = *apps = FALSE;
      return;
    }
  }

  BOOL (WINAPI *pShouldSystemUseDarkMode)() =
    (BOOL (WINAPI *)())GetProcAddress(uxtheme_dll, MAKEINTRESOURCEA(138));
  // This API seems to always return false. I wish they'd document this stuff.
  // BOOL (WINAPI *pShouldAppsUseDarkMode)() =
  //   (BOOL (WINAPI *)())GetProcAddress(uxtheme_dll, MAKEINTRESOURCEA(132));

  if (!pShouldSystemUseDarkMode) {
    *system = FALSE;
  } else {
    *system = pShouldSystemUseDarkMode();
  }
  *apps = *system;

  // if (!pShouldAppsUseDarkMode) {
  //   *apps = FALSE;
  // } else {
  //   *apps = pShouldAppsUseDarkMode();
  // }
}

static unsigned get_theme_flags() {
  BOOL system, apps;
  is_dark_mode(&system, &apps);
  unsigned flags = 0;
  if (system) {
    flags |= ThemeFlagSystemDark;
  } else {
    flags |= ThemeFlagSystemLight;
  }
  if (apps) {
    flags |= ThemeFlagAppDark;
  } else {
    flags |= ThemeFlagAppLight;
  }
  return flags;
}

static const char *get_system_theme_name(bool isdark) {
  return isdark ? "Dark" : "Light";
}

static struct options *g_opts;

static LRESULT CALLBACK
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_SETTINGCHANGE:
    if (!strcmp((LPSTR)lParam, "ImmersiveColorSet")) {
      theme_changed(get_theme_flags(), g_opts);
    }
    break;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

static int listen_for_theme_change(struct options *opts) {
  g_opts = opts;

  WNDCLASSEX wcl;
  memset(&wcl, 0, sizeof(WNDCLASSEX));
  wcl.cbSize = sizeof(WNDCLASSEX);
  wcl.lpfnWndProc = WndProc;
  wcl.hInstance = GetModuleHandle(0);
  wcl.lpszClassName = "yinyang";

  RegisterClassEx(&wcl);

  HWND hWnd = CreateWindowEx(
    0, "yinyang", "YinYang", 0, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(0),
    NULL
  );

  ShowWindow(hWnd, SW_HIDE);
  UpdateWindow(hWnd);

  unsigned flags = get_theme_flags();
  theme_changed(flags | ThemeFlagInitialValue, opts);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    DispatchMessage(&msg);
  }

  return 0;
}

struct env env_win32 = {
  .get_theme_flags = get_theme_flags,
  .get_system_theme_name = get_system_theme_name,
  .listen_for_theme_change = listen_for_theme_change,
};

struct env *get_env(const char *name) {
  if (name == NULL || !strcmp(name, "win32") || !strcmp(name, "Win32") ||
    !strcmp(name, "windows") || !strcmp(name, "Windows")) {
    return &env_win32;
  }
  return NULL;
}
