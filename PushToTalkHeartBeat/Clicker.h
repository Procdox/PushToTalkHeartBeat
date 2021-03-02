#pragma once

#include "common.h"

class Clicker {
  PIMPL

public:
  Clicker();
  ~Clicker();

  void setActivate(int);
  void setOutput(int);
  void setTime(int);
  void setDelay(int);
  void setEnable(bool);
};