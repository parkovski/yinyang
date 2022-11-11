#include "listen.h"
#include <string.h>
#import <AppKit/AppKit.h>

// Private API {{{

static unsigned get_ns_appearance_flags(NSAppearance *appearance) {
  if ([[appearance name] containsString:@"Dark"]) {
    // TODO Does macOS differentiate between system and app colors?
    return ThemeFlagSystemDark | ThemeFlagAppDark;
  } else {
    return ThemeFlagSystemLight | ThemeFlagAppLight;
  }
}

@interface YYAppearanceObserver : NSObject {
  bool seenInitial;
}
- (instancetype)init;
- (void)observeValueForKeyPath:(NSString*)keyPath
                      ofObject:(id)object
                        change:(nullable NSDictionary<NSString *,id> *)change
                       context:(nullable void *)context;
@end

@implementation YYAppearanceObserver
- (instancetype)init {
  self->seenInitial = false;
  return self;
}

- (void)observeValueForKeyPath:(NSString*)keyPath
                      ofObject:(id)object
                        change:(nullable NSDictionary<NSString *,id> *)change
                       context:(nullable void *)context {
  NSAppearance *appearance = [(NSApplication *)object effectiveAppearance];
  const char *name = [appearance name].UTF8String;
  unsigned flags = get_ns_appearance_flags(appearance);

  if (!self->seenInitial) {
    self->seenInitial = true;
    flags |= ThemeFlagInitialValue;
  }

  ((ThemeChangedCallback)context)(name, flags);
}
@end

// }}}

// Public API {{{

unsigned get_theme_flags() {
  return get_ns_appearance_flags(
    [[NSApplication sharedApplication] effectiveAppearance]
  );
}

int listen_for_theme_change(ThemeChangedCallback callback) {
  NSApplication *app = [NSApplication sharedApplication];
  YYAppearanceObserver *observer = [YYAppearanceObserver new];
  [app addObserver:observer
        forKeyPath:@"effectiveAppearance"
           options:NSKeyValueObservingOptionNew | NSKeyValueObservingOptionInitial
           context:callback];
  [app run];
  return 0;
}

// }}}
