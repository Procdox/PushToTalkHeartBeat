#include "clicker.h"

#include <thread>
#include <mutex>

#include <QDebug>

#include <chrono>

#include <Windows.h>
#include "winuser.h"

namespace {
  constexpr INPUT keyboard_up() {
    INPUT result = {0};
    result.type = INPUT_KEYBOARD;
    result.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    return result;
  }
  constexpr INPUT keyboard_down() {
    INPUT result = {0};
    result.type = INPUT_KEYBOARD;
    result.ki.dwFlags = KEYEVENTF_SCANCODE;
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

struct Clicker_State {
  bool running = false;
  bool mode = false;

  int activate = VK_LEFT;
  int mute = VK_LEFT;

  int delay = 100;
  double max_time = 28;
  int sensitivity = 0;

  bool isActive() const {
    //Voice
    if( mode ) {  
      return false; 
    }

    // PTT
    return (GetKeyState(activate) & 0x80) != 0;
  }
  void get_keys(INPUT& up, INPUT& down) const {
    if( mute < 0x07 ) {
      up = mouse_up();
      down = mouse_down();
      if( mute == VK_RBUTTON ) {
        up.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        down.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
      }
      else if( mute == VK_MBUTTON ) {
        up.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
        down.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
      }
      else if( mute != VK_LBUTTON ){
        int x = (mute == VK_XBUTTON1) ? XBUTTON1: XBUTTON2;
        up.mi.dwFlags = MOUSEEVENTF_XUP;
        down.mi.dwFlags = MOUSEEVENTF_XDOWN;
        up.mi.mouseData = x;
        down.mi.mouseData = x;
      }
    }
    else {
      up = keyboard_up();
      down = keyboard_down();
      up.ki.wScan = MapVirtualKey( mute, MAPVK_VK_TO_VSC);
      down.ki.wScan = MapVirtualKey( mute, MAPVK_VK_TO_VSC);
    }
  }
};



class Clicker::Data {
  typedef std::chrono::high_resolution_clock clock;

  std::thread monitor;

  std::mutex lock;

  bool update = true;
  bool alive = true;

  Clicker_State internal;
  
  void run() {
    INPUT up = {0};
    INPUT down = {0};

    Clicker_State current;

    auto last = clock::now();

    bool pressed = false;

    while( true ) {
      {
        const std::lock_guard<std::mutex> guard(lock);
        if(update) {
          if(!alive)
            break;
          update = false;
          current = internal;
          current.get_keys(up,down);
        }
      }
      if( current.running ) {
        if( !current.isActive() )
          pressed = false;
        else {
          if( !pressed ) {
            last = clock::now();
            pressed = true;
          }
          else if( std::chrono::duration<double>(clock::now() - last).count() > current.max_time ) {
            SendInput(1, &down, sizeof(INPUT));
            Sleep(current.delay);
            if( current.isActive() ) //did we stop holding the button during the pause
              SendInput(1, &up, sizeof(INPUT));

            last = clock::now();
          }
        }
  
      }

      Sleep(50);
    }
  }
  

public:
  Data() {
    monitor = std::move(std::thread(&Data::run,this));
  }
  ~Data() {
    {
      const std::lock_guard<std::mutex> guard(lock);
      alive = false;
      update = true;
    }
    monitor.join(); 
  }

  void setMode(bool m) {
    const std::lock_guard<std::mutex> guard(lock);
    internal.mode = m;
    update = true;
  }

  void setActivate(int key) {
    const std::lock_guard<std::mutex> guard(lock);
    internal.activate = key;
    update = true;
  }
  void setMute(int key) {
    const std::lock_guard<std::mutex> guard(lock);
    internal.mute = key;
    update = true;
  }

  void setTime(double t) {
    const std::lock_guard<std::mutex> guard(lock);
    internal.max_time = t;
    update = true;
  }
  void setDelay(double t) {
    const std::lock_guard<std::mutex> guard(lock);
    internal.delay = t;
    update = true;
  }
  void setSensitivity(double t) {
    const std::lock_guard<std::mutex> guard(lock);
    internal.sensitivity = t;
    update = true;
  }

  void setEnabled(bool enabled) {
    const std::lock_guard<std::mutex> guard(lock);
    internal.running = enabled;
    update = true;
  }
};



Clicker::Clicker() : data( std::make_unique<Data>() ){}
Clicker::~Clicker() {}

void Clicker::setMode(bool mode) {              d().setMode(mode); }
void Clicker::setActivate(int ptt) {            d().setActivate(ptt); }
void Clicker::setMute(int mute) {               d().setMute(mute); }
void Clicker::setTime(int time) {               d().setTime(time); }
void Clicker::setDelay(int delay) {             d().setDelay(delay); }
void Clicker::setSensitivity(int sensitivity) { d().setSensitivity(sensitivity); }
void Clicker::setEnable(bool enabled) {         d().setEnabled(enabled); }