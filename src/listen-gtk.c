#include "yinyang.h"
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <glib-unix.h>

// Private API {{{

typedef struct {
  struct options *opts;
  GSettings *settings;
} callback_data;

static unsigned get_color_scheme_flags(gchar *color_scheme) {
  if (color_scheme == NULL) {
    return 0;
  }
  if (!strcmp(color_scheme, "prefer-dark")) {
    return ThemeFlagAppDark;
  }
  // "prefer-light" or "default":
  return ThemeFlagAppLight;
}

static void theme_changed_callback(GSettings *settings, gchar *key,
                                   void *data) {
  callback_data *cbdata = data;
  char *color_scheme = g_settings_get_string(settings, "color-scheme");
  unsigned flags = get_color_scheme_flags(color_scheme);
  free(color_scheme);
  theme_changed(flags, cbdata->opts);
}

static gboolean handle_sigint(void *data) {
  g_application_release((GApplication*)data);
  return true;
}

static void on_activate(GtkApplication *app, void *data) {
  g_unix_signal_add(SIGINT, handle_sigint, G_APPLICATION(app));

  callback_data *cbdata = (callback_data*)data;
  cbdata->settings = g_settings_new("org.gnome.desktop.interface");

  // Check the property once so change events will be emitted.
  char *color_scheme = g_settings_get_string(cbdata->settings, "color-scheme");
  if (color_scheme == NULL) {
    g_object_unref(cbdata->settings);
    return;
  }
  unsigned flags = get_color_scheme_flags(color_scheme);
  free(color_scheme);
  theme_changed(flags | ThemeFlagInitialValue, cbdata->opts);

  g_signal_connect(cbdata->settings, "changed::color-scheme",
                   G_CALLBACK(theme_changed_callback), data);
}

// }}}

// Public API {{{

unsigned get_theme_flags() {
  GSettings *settings = g_settings_new("org.gnome.desktop.interface");
  gchar *color_scheme = g_settings_get_string(settings, "color-scheme");
  unsigned flags = get_color_scheme_flags(color_scheme);
  free(color_scheme);
  g_object_unref(settings);
  return flags;
}

const char *get_system_theme_name(bool isdark) {
  if (isdark) {
    return "prefer-dark";
  }
  return "default";
}

int listen_for_theme_change(struct options *opts) {
  GtkApplication *app
    = gtk_application_new("com.parkovski.yinyang", G_APPLICATION_NON_UNIQUE);
  callback_data data = (callback_data) {
    .opts = opts,
    .settings = NULL,
  };
  g_signal_connect(app, "activate", G_CALLBACK(on_activate), &data);
  char *argv[] = { "yinyang" };
  g_application_hold(G_APPLICATION(app));
  int status = g_application_run(G_APPLICATION(app), 1, argv);
  g_object_unref(app);
  if (data.settings) {
    g_object_unref(data.settings);
  }
  return status;
}

// }}}
