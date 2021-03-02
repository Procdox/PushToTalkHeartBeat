#include "clicker.h"

#include <thread>
#include <mutex>

#include <QDebug>

#include <chrono>

#include <Windows.h>
#include "winuser.h"

namespace {
  enum ClickerMode {
    dead = 0
  , paused
  , running
  };

  constexpr INPUT keyboard_up() {
    INPUT result = {0};
    result.type = INPUT_KEYBOARD;
    result.ki.dwFlags = KEYEVENTF_KEYUP;
    return result;
  }
  constexpr INPUT keyboard_down() {
    INPUT result = {0};
    result.type = INPUT_KEYBOARD;
    result.ki.dwFlags = 0;
    return result;
  }
  constexpr INPUT mouse_up() {
    INPUT result = {0};
    result.type = INPUT_MOUSE;
    result.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    return result;
  }
  constexpr INPUT mouse_down() {
    INPUT result = {0};
    result.type = INPUT_MOUSE;
    result.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    return result;
  }
};

class Clicker::Data {

  std::thread monitor;

  std::mutex lock;

  bool update = true;
  ClickerMode mode = ClickerMode::paused;
  int activate = VK_LEFT;
  int delay = 100;
  double max_time = 28;
  
  void run() {
    INPUT up = {0};
    INPUT down = {0};

    ClickerMode current_mode;
    int current_activate;
    int current_delay;
    double current_max_time;

    auto last = std::chrono::high_resolution_clock::now();

    bool pressed = false;

    while(true) {
      {
        const std::lock_guard<std::mutex> guard(lock);
        if(update) {
          if(mode == ClickerMode::dead)
            break;

          current_mode = mode;
          current_activate = activate;
          current_delay = delay;
          current_max_time = max_time;

          qDebug() << "Updated" << current_max_time << current_delay << hex << current_activate;

          if(current_activate < 0x07) {
            up = mouse_up();
            down = mouse_down();
            if(current_activate == VK_RBUTTON) {
              up.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
              down.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            }
            else if(current_activate == VK_MBUTTON) {
              up.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
              down.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
            }
            else if( current_activate != VK_LBUTTON ){
              int x = (current_activate == VK_XBUTTON1) ? XBUTTON1: XBUTTON2;
              up.mi.dwFlags = MOUSEEVENTF_XUP;
              down.mi.dwFlags = MOUSEEVENTF_XDOWN;
              up.mi.mouseData = x;
              down.mi.mouseData = x;
            }
          }
          else {
            up = keyboard_up();
            down = keyboard_down();
            up.ki.wVk = current_activate;
            down.ki.wVk = current_activate;
          }
          update = false;
        }
      }
      if(current_mode == ClickerMode::paused || (GetKeyState(current_activate) & 0x80) == 0) {
        last = std::chrono::high_resolution_clock::now();
        //SendInput(1,&up,sizeof(INPUT));

        if(pressed){
          qDebug() << "Up";
          pressed = false;
          Sleep(current_delay);
        }
      }
      else {
        if(!pressed){
          qDebug() << "Down";
          pressed = true;
        }
        else if( std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - last).count() > current_max_time ){
          last = std::chrono::high_resolution_clock::now();
          qDebug() << "Bounce";
          SendInput(1,&up,sizeof(INPUT));
          Sleep(current_delay);
          SendInput(1,&down,sizeof(INPUT));
        }
      }

      Sleep(50);
    }
    qDebug() << "Killed";
  }
  

public:
  Data() {
    monitor = std::move(std::thread(&Data::run,this));
  }
  ~Data() {
    setMode(ClickerMode::dead);
    monitor.join(); 
  }

  void setActivate(int key) {
    const std::lock_guard<std::mutex> guard(lock);
    activate = key;
    update = true;
  }
  void setTime(double t) {
    const std::lock_guard<std::mutex> guard(lock);
    max_time = t;
    update = true;
  }
  void setDelay(double t) {
    const std::lock_guard<std::mutex> guard(lock);
    delay = t;
    update = true;
  }
  void setMode(ClickerMode _mode) {
    const std::lock_guard<std::mutex> guard(lock);
    mode = _mode;
    update = true;
  }
};



Clicker::Clicker() 
: data( std::make_unique<Data>() ){
}


Clicker::~Clicker() {
}

void Clicker::setActivate(int key) {
  d().setActivate(key);
}
void Clicker::setTime(int t) {
  d().setTime(t);
}
void Clicker::setDelay(int t) {
  d().setDelay(t);
}
void Clicker::setEnable(bool enabled) {
  d().setMode(enabled ? ClickerMode::running : ClickerMode::paused);
}