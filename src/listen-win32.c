#include "yinyang.h"
#include <Windows.h>
#include <stdio.h>

static BOOL is_dark_mode() {
  static HANDLE uxtheme_dll = NULL;
  if (uxtheme_dll == NULL) {
    uxtheme_dll = LoadLibraryW(L"uxtheme.dll");
    if (!uxtheme_dll) {
      return FALSE;
    }
  }

  BOOL (WINAPI *pShouldSystemUseDarkMode)() =
    (BOOL (WINAPI *)())GetProcAddress(uxtheme_dll, MAKEINTRESOURCEA(138));
  if (!pShouldSystemUseDarkMode) {
    return FALSE;
  }

  if (pShouldSystemUseDarkMode()) {
    return TRUE;
  }
  return FALSE;
}

static unsigned get_theme_flags() {
  if (is_dark_mode()) {
    return ThemeFlagSystemDark | ThemeFlagAppDark;
  } else {
    return ThemeFlagSystemLight | ThemeFlagAppLight;
  }
}

static const char *get_system_theme_name(bool isdark) {
  return is_dark_mode() ? "dark" : "light";
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
