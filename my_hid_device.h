#ifndef MY_HID_DEVICE_H
#define MY_HID_DEVICE_H

#include "Keyboard.h"
#include "Mouse.h"

class MyKeyboard_: public Keyboard_ {
public:
  MyKeyboard_(): Keyboard_{}
  {}

  void begin(const uint8_t *layout = KeyboardLayout_en_US) override;
  void end() override;
  virtual void sendReport(KeyReport* keys) override;
  virtual void sendConsumerReport(uint16_t key) override;
  void sendSystemReport(uint16_t key);
  void tryWakeup();

private:
  bool checkSuspended();
  uint8_t _idSystem = 0;
  volatile bool _needWakeup = false;
};

extern MyKeyboard_ MyKeyboard;

class MyMouse_: public Mouse_ {
public:
  MyMouse_(): Mouse_{}
  {}

  void buttons(uint8_t b)
  {
    Mouse_::buttons(b);
  }
};

bool isUsbSuspended();

extern MyMouse_ MyMouse;
#endif
