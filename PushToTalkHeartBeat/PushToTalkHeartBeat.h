#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PushToTalkHeartBeat.h"

#include "clicker.h"

class PushToTalkHeartBeat : public QMainWindow
{
  Q_OBJECT

  Clicker cl;
  qint32 last_activate = 0x00;
  qint32 last_output = 0x00;

public:
  PushToTalkHeartBeat(QWidget *parent = Q_NULLPTR);

private:
  Ui::PushToTalkHeartBeatClass ui;

private slots:
  void handleActivateChanged(qint32);
  void handleOutputChanged(qint32);
  void handleTimeChanged(int);
  void handleDelayChanged(int);
  void handleEnabled(bool);
};
