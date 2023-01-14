#pragma once

extern "C" {
#include "yinyang.h"
}

#include <QObject>
#include <KConfigGroup>

class ConfigChanged : public QObject {
  Q_OBJECT

public:
  ConfigChanged(struct options *opts);
  virtual ~ConfigChanged();

public slots:
  void configChanged(const KConfigGroup &group, const QByteArrayList &names);

private:
  struct options *_opts;
};
