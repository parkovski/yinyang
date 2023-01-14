#include "listen-kde.h"

#include <QCoreApplication>
#include <KConfig>
#include <KConfigGroup>
#include <KConfigWatcher>
#include <memory>

// Private API {{{

static unsigned get_theme_flags();

ConfigChanged::ConfigChanged(struct options *opts)
  : _opts{opts}
{}

ConfigChanged::~ConfigChanged() {}

void ConfigChanged::configChanged(const KConfigGroup &group, const QByteArrayList &names) {
  if (group.name() == "General" && names.contains("ColorScheme")) {
    theme_changed(get_theme_flags(), _opts);
  }
}

// }}}

// Public API {{{

static int argc = 1;
static char *argv[] = { const_cast<char*>("yinyang") };
static QCoreApplication *app = nullptr;

static unsigned get_theme_flags() {
  std::unique_ptr<QCoreApplication> local_app = nullptr;
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
  QCoreApplication _app{argc, argv};
  app = &_app;

  if (opts->print & PrintFlagNow) {
    theme_changed(get_theme_flags() | ThemeFlagInitialValue, opts);
  }

  ConfigChanged conf{opts};
  auto watcher = KConfigWatcher::create(KSharedConfig::openConfig());
  bool b = QObject::connect(
    watcher.data(), SIGNAL(configChanged(const KConfigGroup&, const QByteArrayList&)),
    &conf, SLOT(configChanged(const KConfigGroup&, const QByteArrayList&))
  );
  return _app.exec();
}

extern "C" struct env env_kde;
struct env env_kde = {
  .get_theme_flags = get_theme_flags,
  .get_system_theme_name = get_system_theme_name,
  .listen_for_theme_change = listen_for_theme_change,
};

// }}}
