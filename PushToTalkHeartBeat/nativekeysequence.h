#pragma once

#include <QLineEdit>
#include <QSettings>

class NativeKeySequence : public QLineEdit {
  Q_OBJECT

  qint32 last = 0x00;

public:
  ~NativeKeySequence();
  NativeKeySequence(QWidget*);

  void set(qint32, const QString& s = {});
  void load(QSettings& settings, qint32 default_value, QString default_text);
  void save(QSettings& settings);

private:
  bool event(QEvent*);

signals:
  void nativeReady(qint32);
};