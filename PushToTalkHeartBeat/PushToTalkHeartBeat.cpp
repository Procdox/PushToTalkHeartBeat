#include "PushToTalkHeartBeat.h"

#include <QSettings>
#include <QDebug>

PushToTalkHeartBeat::~PushToTalkHeartBeat() {
  qDebug() << "Saving...";
  qDebug() << ui.keys_activate->text() << ui.keys_output->text() << last_activate << last_output << ui.time->value() << ui.delay->value();
  QSettings settings;
  settings.beginGroup("heartbeat");
  settings.setValue("last_activate", (int)last_activate);
  settings.setValue("last_output", (int)last_output);
  settings.setValue("title_activate", ui.keys_activate->text());
  settings.setValue("title_output", ui.keys_output->text());
  settings.setValue("time", (int)ui.time->value());
  settings.setValue("delay", (int)ui.delay->value());
  settings.endGroup();
}

PushToTalkHeartBeat::PushToTalkHeartBeat(QWidget *parent)
    : QMainWindow(parent)
{
  ui.setupUi(this);
  setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

  bool res = true;
  res &= (bool)connect(ui.keys_activate, &NativeKeySequence::nativeReady, this, &PushToTalkHeartBeat::handleActivateChanged);
  res &= (bool)connect(ui.keys_output, &NativeKeySequence::nativeReady, this, &PushToTalkHeartBeat::handleOutputChanged);
  res &= (bool)connect(ui.time, SIGNAL(valueChanged(int)), this, SLOT(handleTimeChanged(int)));
  res &= (bool)connect(ui.delay, SIGNAL(valueChanged(int)), this, SLOT(handleDelayChanged(int)));
  res &= (bool)connect(ui.enabled, &QPushButton::toggled, this, &PushToTalkHeartBeat::handleEnabled);

  Q_ASSERT(res);

  QSettings settings;
  settings.beginGroup("heartbeat");
  const auto last_activate = settings.value("last_activate",0x05).toInt();
  const auto last_output = settings.value("last_output",0x06).toInt();
  const auto title_activate = settings.value("title_activate","").toString();
  const auto title_output = settings.value("title_output","").toString();
  const auto time = settings.value("time",28).toInt();
  const auto delay = settings.value("delay",200).toInt();
  settings.endGroup();

  qDebug() << "Loading...";
  qDebug() << title_activate << title_output << last_activate << last_output << time << delay;

  ui.keys_activate->set(last_activate, title_activate);
  ui.keys_output->set(last_output, title_output);
  ui.time->setValue(time);
  ui.delay->setValue(delay);
  cl.setTime(time);
  cl.setDelay(delay);
}

void PushToTalkHeartBeat::handleActivateChanged(qint32 key) {
  if(key == last_output) {
    ui.enabled->setChecked(false);
    cl.setEnable(false); 
  }
  cl.setActivate(key);
  last_activate = key;
}
void PushToTalkHeartBeat::handleOutputChanged(qint32 key) {
  if(key == last_activate) {
    ui.enabled->setChecked(false);
    cl.setEnable(false); 
  }
  cl.setOutput(key);
  last_output = key;
}
void PushToTalkHeartBeat::handleTimeChanged(int t) {
  cl.setTime(t);
}
void PushToTalkHeartBeat::handleDelayChanged(int t) {
  cl.setDelay(t);
}
void PushToTalkHeartBeat::handleEnabled(bool enabled) {
  cl.setEnable(enabled);
}
