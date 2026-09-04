#include <Arduino.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <USB.h>
#include "tusb.h"
#include "my_hid_device.h"

static const uint8_t desc_hid_report_keyboard[] = { TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(1))};
static const uint8_t desc_hid_report_consumer[] = { TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(1))};
static const uint8_t desc_hid_report_system[] = { TUD_HID_REPORT_DESC_SYSTEM_CONTROL(HID_REPORT_ID(1)) };

void MyKeyboard_::begin(const uint8_t *layout)
{
  if (_running) {
    return;
  }
  USB.disconnect();
  _id = USB.registerHIDDevice(desc_hid_report_keyboard, sizeof(desc_hid_report_keyboard), 10, 0x0001);
  _idConsumer = USB.registerHIDDevice(desc_hid_report_consumer, sizeof(desc_hid_report_consumer), 11, 0x0000);
  _idSystem = USB.registerHIDDevice(desc_hid_report_system, sizeof(desc_hid_report_system), 12, 0x0000);
  USB.connect();
  HID_Keyboard::begin(layout);
}

void MyKeyboard_::end()
{
  USB.disconnect();
  USB.unregisterHIDDevice(_id);
  USB.unregisterHIDDevice(_idConsumer);
  USB.unregisterHIDDevice(_idSystem);
  USB.connect();
  HID_Keyboard::end();
}

bool isUsbSuspended()
{
  return tud_suspended();
}

void MyKeyboard_::sendReport(KeyReport* keys)
{
  if (!_running) {
    return;
  }

  /*
   * If the remote has gone to sleep (laptop), tickle it so they will get the message
   * Following code doesn't work:
   *
   *   if (tud_suspended()) tud_remote_wakeup();
   *
   * so we need other trick:
   */
  if (tud_suspended()) {
    Serial.println("Host is suspended, trying wakeup...");
    USB.disconnect();
    delay(100);
    USB.connect();
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
  }

  Keyboard_::sendReport(keys);
}

void MyKeyboard_::sendConsumerReport(uint16_t key)
{
    Keyboard_::sendConsumerReport(key);
}

void MyKeyboard_::sendSystemReport(uint16_t key)
{
  if (!_running) {
    return;
  }
  CoreMutex m(&USB.mutex);
  tud_task();
  if (USB.HIDReady()) {
    tud_hid_report(USB.findHIDReportID(_idSystem), &key, 1u);
  }
  tud_task();
}

MyKeyboard_ MyKeyboard;
MyMouse_ MyMouse;
