#include "yinyang.h"
#include <string.h>
#include <strings.h>

extern char **environ;

#ifdef YY_HAS_GTK
extern struct env env_gtk;
#endif

#ifdef YY_HAS_KDE
extern struct env env_kde;
#endif

struct env *get_env(const char *name) {
#if defined(YY_HAS_KDE) && defined(YY_HAS_GTK)
  if (name == NULL) {
    bool is_kde = false;
    for (int i = 0; environ[i]; ++i) {
      if (!strncmp(environ[i], "KDE_SESSION_UID=", sizeof("KDE_SESSION_UID=")-1)
       || !strncmp(environ[i], "XDG_CURRENT_DESKTOP=KDE", sizeof("XDG_CURRENT_DESKTOP=KDE")-1)) {
        is_kde = true;
        break;
      }
    }
    if (is_kde) {
      return &env_kde;
    } else {
      return &env_gtk;
    }
  }

  if (!strcasecmp(name, "kde") || !strcasecmp(name, "plasma")) {
    return &env_kde;
  }
  if (!strcasecmp(name, "gtk") || !strcasecmp(name, "gnome")) {
    return &env_gtk;
  }
#elif defined(YY_HAS_KDE)
  if (name == NULL || !strcasecmp(name, "kde") || !strcasecmp(name, "plasma")) {
    return &env_kde;
  }
#elif defined(YY_HAS_GTK)
  if (name == NULL || !strcasecmp(name, "gtk") || !strcasecmp(name, "gnome")) {
    return &env_gtk;
  }
#endif
  return NULL;
}
