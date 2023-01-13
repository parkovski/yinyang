#include "yinyang.h"
#include <string.h>
#import <AppKit/AppKit.h>

// Private API {{{

@interface YYAppearanceObserver : NSObject {
  bool seenInitial;
  struct options *opts;
}
- (instancetype)initWithOptions:(struct options*)options;
+ (instancetype)newWithOptions:(struct options*)options;
- (void)observeValueForKeyPath:(NSString*)keyPath
                      ofObject:(id)object
                        change:(nullable NSDictionary<NSString *,id> *)change
                       context:(nullable void *)context;
@end

static unsigned get_NSAppearance_flags(NSAppearance *appearance) {
  if ([[appearance name] containsString:@"Dark"]) {
    // TODO Does macOS differentiate between system and app colors?
    return ThemeFlagSystemDark | ThemeFlagAppDark;
  } else {
    return ThemeFlagSystemLight | ThemeFlagAppLight;
  }
}

@implementation YYAppearanceObserver
- (instancetype)initWithOptions:(struct options*)options {
  self->seenInitial = false;
  self->opts = options;
  return self;
}

+ (instancetype)newWithOptions:(struct options*)options {
  return [[YYAppearanceObserver alloc] initWithOptions:options];
}

- (void)observeValueForKeyPath:(NSString*)keyPath
                      ofObject:(id)object
                        change:(nullable NSDictionary<NSString *,id> *)change
                       context:(nullable void *)context {
  NSAppearance *appearance = [(NSApplication *)object effectiveAppearance];
  unsigned flags = get_NSAppearance_flags(appearance);

  if (!self->seenInitial) {
    self->seenInitial = true;
    flags |= ThemeFlagInitialValue;
  }

  theme_changed(flags, self->opts);
}
@end

// }}}

// Public API {{{

static unsigned get_theme_flags() {
  return get_NSAppearance_flags(
    [[NSApplication sharedApplication] effectiveAppearance]
  );
}

static const char *get_system_theme_name(bool isdark) {
  if (isdark) {
    return NSAppearanceNameDarkAqua.UTF8String;
  }
  return NSAppearanceNameAqua.UTF8String;
}

static int listen_for_theme_change(struct options *opts) {
  NSApplication *app = [NSApplication sharedApplication];
  YYAppearanceObserver *observer = [YYAppearanceObserver newWithOptions:opts];
  [app addObserver:observer
        forKeyPath:@"effectiveAppearance"
           options:NSKeyValueObservingOptionNew | NSKeyValueObservingOptionInitial
           context:nil];
  [app run];
  return 0;
}

struct env env_macos = {
  .get_theme_flags = get_theme_flags,
  .get_system_theme_name = get_system_theme_name,
  .listen_for_theme_change = listen_for_theme_change,
};

struct env *get_env(const char *name) {
  if (name == NULL || !strcmp(name, "macos") || !strcmp(name, "macOS")
   || !strcmp(name, "aqua") || !strcmp(name, "Aqua")) {
    return &env_macos;
  }
  return NULL;
}

// }}}
