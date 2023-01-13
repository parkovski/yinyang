extern "C" {
#include "yinyang.h"
}

#include <QCoreApplication>
#include <KConfig>
#include <KConfigGroup>
#include <memory>

// Private API {{{



// }}}

// Public API {{{

static QCoreApplication *app = nullptr;

static unsigned get_theme_flags() {
  std::unique_ptr<QCoreApplication> local_app = nullptr;
  int argc = 1;
  char *argv[] = { const_cast<char*>("yinyang") };
  if (!app) {
    local_app = std::make_unique<QCoreApplication>(argc, argv);
  }
  KConfig config;
  KConfigGroup generalGroup(&config, "General");
  QString value = generalGroup.readEntry("ColorScheme", QString());
  if (value == "BreezeDark") {
    return ThemeFlagAppDark | ThemeFlagSystemDark;
  }
  return ThemeFlagAppLight | ThemeFlagSystemLight;
}

static const char *get_system_theme_name(bool isdark) {
  if (isdark) {
    return "BreezeDark";
  }
  return "BreezeLight";
}

static int listen_for_theme_change(struct options *opts) {
  return 0;
}

extern "C" struct env env_kde;
struct env env_kde = {
  .get_theme_flags = get_theme_flags,
  .get_system_theme_name = get_system_theme_name,
  .listen_for_theme_change = listen_for_theme_change,
};

// }}}
