#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PushToTalkHeartBeat.h"

#include "clicker.h"

class PushToTalkHeartBeat : public QMainWindow
{
  Q_OBJECT

  Clicker cl;

public:
  ~PushToTalkHeartBeat();
  PushToTalkHeartBeat(QWidget *parent = Q_NULLPTR);

private:
  Ui::PushToTalkHeartBeatClass ui;

private slots:
  void handleModeChanged(int);
  void handlePTTChanged(qint32);
  void handleMuteChanged(qint32);
  void handleTimeChanged(int);
  void handleDelayChanged(int);
  void handleSensitivityChanged(int);
  void handleEnabledChanged(bool);
};
