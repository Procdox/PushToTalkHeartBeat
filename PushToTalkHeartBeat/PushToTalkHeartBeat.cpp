#include "PushToTalkHeartBeat.h"

#include <QSettings>
#include <QDebug>

template<typename t>
void load_input(t* input, QSettings& settings, int default_value) {
  input->setValue(settings.value(input->objectName(),default_value).toInt());
}

template<typename t>
void save_input(t* input, QSettings& settings) {
  settings.setValue(input->objectName(), (int)input->value());
}

PushToTalkHeartBeat::~PushToTalkHeartBeat() {
  QSettings settings;
  settings.beginGroup("heartbeat");
  ui.keys_activate->save(settings);
  ui.keys_mute->save(settings);

  save_input(ui.time, settings);
  save_input(ui.delay, settings);
  save_input(ui.sensitivity, settings);

  settings.setValue("mode", ui.tabs->currentIndex());
}

PushToTalkHeartBeat::PushToTalkHeartBeat(QWidget *parent)
    : QMainWindow(parent)
{
  ui.setupUi(this);
  setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

  bool res = true;
  res &= (bool)connect(ui.keys_activate, &NativeKeySequence::nativeReady, this, &PushToTalkHeartBeat::handlePTTChanged);
  res &= (bool)connect(ui.keys_mute, &NativeKeySequence::nativeReady, this, &PushToTalkHeartBeat::handleMuteChanged);

  res &= (bool)connect(ui.time, SIGNAL(valueChanged(int)), this, SLOT(handleTimeChanged(int)));
  res &= (bool)connect(ui.delay, SIGNAL(valueChanged(int)), this, SLOT(handleDelayChanged(int)));
  res &= (bool)connect(ui.sensitivity, SIGNAL(valueChanged(int)), this, SLOT(handleSensitivityChanged(int)));

  res &= (bool)connect(ui.enabled, SIGNAL(toggled(bool)), this, SLOT(handleEnabledChanged(bool)));

  Q_ASSERT(res);

  QSettings settings;
  settings.beginGroup("heartbeat");
  ui.keys_activate->load(settings,0x05,"");
  ui.keys_mute->load(settings,0x25,"Left");

  load_input(ui.time,settings,28);
  load_input(ui.delay,settings,250);
  load_input(ui.sensitivity,settings,0);

  ui.tabs->setCurrentIndex(settings.value("mode", 0).toInt());
}

void PushToTalkHeartBeat::handlePTTChanged(qint32 ptt) {
  cl.setActivate(ptt);
}
void PushToTalkHeartBeat::handleMuteChanged(qint32 mute) {
  
}

void PushToTalkHeartBeat::handleTimeChanged(int time) {
  cl.setTime(time);
}
void PushToTalkHeartBeat::handleDelayChanged(int delay) {
  cl.setDelay(delay);
}
void PushToTalkHeartBeat::handleSensitivityChanged(int sensitivity) {

}

void PushToTalkHeartBeat::handleEnabledChanged(bool enabled) {
  cl.setEnable(enabled);
}