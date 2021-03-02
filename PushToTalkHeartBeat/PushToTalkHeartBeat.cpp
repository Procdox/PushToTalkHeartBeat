#include "PushToTalkHeartBeat.h"

#include <QSettings>
#include <QDebug>

PushToTalkHeartBeat::~PushToTalkHeartBeat() {
  qDebug() << "Saving...";
  qDebug() << ui.keys_activate->text() << last_activate << ui.time->value() << ui.delay->value();
  QSettings settings;
  settings.beginGroup("heartbeat");
  settings.setValue("last_activate", (int)last_activate);
  settings.setValue("title_activate", ui.keys_activate->text());
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
  res &= (bool)connect(ui.time, SIGNAL(valueChanged(int)), this, SLOT(handleTimeChanged(int)));
  res &= (bool)connect(ui.delay, SIGNAL(valueChanged(int)), this, SLOT(handleDelayChanged(int)));

  Q_ASSERT(res);

  QSettings settings;
  settings.beginGroup("heartbeat");
  const auto last_activate = settings.value("last_activate",0x05).toInt();
  const auto title_activate = settings.value("title_activate","").toString();
  const auto time = settings.value("time",28).toInt();
  const auto delay = settings.value("delay",250).toInt();
  settings.endGroup();

  qDebug() << "Loading...";
  qDebug() << title_activate << last_activate << time << delay;

  ui.keys_activate->set(last_activate, title_activate);
  ui.time->setValue(time);
  ui.delay->setValue(delay);
  cl.setTime(time);
  cl.setDelay(delay);
}

bool PushToTalkHeartBeat::event(QEvent *event){
  if( event->type()==QEvent::Enter ) {
    qDebug() << "entering";
    cl.setEnable(false);
  }
  else if( event->type()==QEvent::Leave ) {
    qDebug() << "leaving";
    if( ui.enabled->isChecked() ) {
      cl.setEnable(true);
    }
  }

  return QMainWindow::event(event);
}

void PushToTalkHeartBeat::handleActivateChanged(qint32 key) {
  cl.setActivate(key);
  last_activate = key;
}
void PushToTalkHeartBeat::handleTimeChanged(int t) {
  cl.setTime(t);
}
void PushToTalkHeartBeat::handleDelayChanged(int t) {
  cl.setDelay(t);
}
