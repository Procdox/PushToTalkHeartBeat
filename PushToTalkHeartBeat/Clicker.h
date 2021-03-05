#pragma once

#include "common.h"

class Clicker {
  PIMPL

public:
  Clicker();
  ~Clicker();

  //false for PTT, true for voice activity
  void setMode(bool);

  void setActivate(int);
  void setMute(int);
  
  void setTime(int);
  void setDelay(int);
  void setSensitivity(int);

  void setEnable(bool);
};