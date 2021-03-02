#include "PushToTalkHeartBeat.h"

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

  ui.keys_activate->set(0x25,"Left");
  ui.keys_output->set(0x01);

  cl.setTime(ui.time->value());
  cl.setDelay(ui.delay->value());
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
