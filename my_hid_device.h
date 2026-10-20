// TinyUSB_KeyboardMouse.h
// Adafruit TinyUSB implementation, API-compatible with arduino-pico's Keyboard / Mouse libraries
//
// USB interface layout (chosen automatically from CFG_TUD_HID):
//   CFG_TUD_HID == 2 (Adafruit TinyUSB default on RP2040):
//     Interface A: Boot Keyboard (SubClass=1, Protocol=1, no Report ID)
//     Interface B: Mouse (Report ID 1) + Consumer (Report ID 2) + System Control (Report ID 3), non-boot
//   CFG_TUD_HID >= 3 (edit tusb_config_rp2040.h in the Adafruit TinyUSB library):
//     Interface A: Boot Keyboard
//     Interface B: Boot Mouse (no Report ID)
//     Interface C: Consumer (Report ID 1) + System Control (Report ID 2)
//   Boot interfaces cannot use Report IDs, so consumer/system cannot share the keyboard interface.
//
// Calling any report-sending function while the device is suspended automatically
// issues a remote wakeup; that report itself is dropped (send it again after resume).
//
// Requirements: Tools -> USB Stack -> "Adafruit TinyUSB", Adafruit TinyUSB Library 3.x
// Keyboard layout is fixed to en_US.
// Written by Claude

#pragma once
#include <Arduino.h>

// Uncomment to remove the default CDC Serial interface and make the device HID-only.
// If compilation fails because clearConfiguration is not found, update the Adafruit TinyUSB library.
// NOTE: Remove CDC for remote wakeup support
#define TINYHID_NO_CDC

// ---- Key constants (same as the Arduino Keyboard library) ----
#define KEY_LEFT_CTRL     0x80
#define KEY_LEFT_SHIFT    0x81
#define KEY_LEFT_ALT      0x82
#define KEY_LEFT_GUI      0x83
#define KEY_RIGHT_CTRL    0x84
#define KEY_RIGHT_SHIFT   0x85
#define KEY_RIGHT_ALT     0x86
#define KEY_RIGHT_GUI     0x87

#define KEY_RETURN        0xB0
#define KEY_ESC           0xB1
#define KEY_BACKSPACE     0xB2
#define KEY_TAB           0xB3
#define KEY_CAPS_LOCK     0xC1
#define KEY_F1            0xC2
#define KEY_F2            0xC3
#define KEY_F3            0xC4
#define KEY_F4            0xC5
#define KEY_F5            0xC6
#define KEY_F6            0xC7
#define KEY_F7            0xC8
#define KEY_F8            0xC9
#define KEY_F9            0xCA
#define KEY_F10           0xCB
#define KEY_F11           0xCC
#define KEY_F12           0xCD
#define KEY_PRINT_SCREEN  0xCE
#define KEY_SCROLL_LOCK   0xCF
#define KEY_PAUSE         0xD0
#define KEY_INSERT        0xD1
#define KEY_HOME          0xD2
#define KEY_PAGE_UP       0xD3
#define KEY_DELETE        0xD4
#define KEY_END           0xD5
#define KEY_PAGE_DOWN     0xD6
#define KEY_RIGHT_ARROW   0xD7
#define KEY_LEFT_ARROW    0xD8
#define KEY_DOWN_ARROW    0xD9
#define KEY_UP_ARROW      0xDA
#define KEY_NUM_LOCK      0xDB
#define KEY_MENU          0xED
#define KEY_F13           0xF0
#define KEY_F14           0xF1
#define KEY_F15           0xF2
#define KEY_F16           0xF3
#define KEY_F17           0xF4
#define KEY_F18           0xF5
#define KEY_F19           0xF6
#define KEY_F20           0xF7
#define KEY_F21           0xF8
#define KEY_F22           0xF9
#define KEY_F23           0xFA
#define KEY_F24           0xFB

// ---- Consumer usages (HID Usage Page 0x0C; any usage may be passed; 0 = release) ----
#define CONSUMER_PLAY_PAUSE    0x00CD
#define CONSUMER_SCAN_NEXT     0x00B5
#define CONSUMER_SCAN_PREV     0x00B6
#define CONSUMER_STOP          0x00B7
#define CONSUMER_MUTE          0x00E2
#define CONSUMER_VOLUME_UP     0x00E9
#define CONSUMER_VOLUME_DOWN   0x00EA
#define CONSUMER_BRIGHTNESS_UP   0x006F
#define CONSUMER_BRIGHTNESS_DOWN 0x0070

// ---- System Control usages (HID Usage Page 0x01; 0x01-0xB7 may be passed; 0 = release) ----
#define SYSTEM_POWER_DOWN      0x0081
#define SYSTEM_SLEEP           0x0082
#define SYSTEM_WAKE_UP         0x0083

// ---- Mouse buttons ----
#define MOUSE_LEFT    0x01
#define MOUSE_RIGHT   0x02
#define MOUSE_MIDDLE  0x04
#define MOUSE_ALL     (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)

// Standard 8-byte keyboard report (same as the Arduino Keyboard library)
typedef struct {
  uint8_t modifiers;   // bit0 LCtrl, bit1 LShift, bit2 LAlt, bit3 LGui, bit4-7 right-hand modifiers
  uint8_t reserved;
  uint8_t keys[6];     // HID usages (not ASCII)
} KeyReport;

// Shared USB device control (suspend state and remote wakeup)
class TinyHIDDevice {
public:
  void begin();            // Called automatically by Keyboard.begin()/Mouse.begin()
  bool suspended();        // True if the device is in USB suspend
  bool remoteWakeup();     // Issue remote wakeup; returns false if the host has not enabled it
  bool ready();            // Enumerated and not suspended
};

class TinyKeyboard : public Print {
public:
  void begin(const uint8_t *layout = nullptr);  // layout kept for compatibility; en_US is always used
  void end();
  size_t press(uint8_t k);
  size_t release(uint8_t k);
  void releaseAll();
  size_t write(uint8_t k) override;
  size_t write(const uint8_t *buffer, size_t size) override;
  using Print::write;

  // Send raw reports
  void sendReport(KeyReport *keys);       // Keyboard report; fill keys[] with HID usages
  void sendConsumerReport(uint16_t key);  // Consumer usage; send 0 to release
  void sendSystemReport(uint16_t key);    // System Control usage; send 0 to release

private:
  KeyReport _keyReport = {0, 0, {0}};
};

class TinyMouse {
public:
  void begin();
  void end();
  void click(uint8_t b = MOUSE_LEFT);
  void move(signed char x, signed char y, signed char wheel = 0);
  void press(uint8_t b = MOUSE_LEFT);
  void release(uint8_t b = MOUSE_LEFT);
  bool isPressed(uint8_t b = MOUSE_LEFT);

  // Raw access
  void buttons(uint8_t b);     // Set the button state directly (MOUSE_LEFT | ...); does not send a report
  uint8_t buttons();           // Get the current button state
  bool sendReport(signed char x = 0, signed char y = 0, signed char wheel = 0);  // Send current buttons + movement

private:
  uint8_t _buttons = 0;
};

extern TinyHIDDevice TinyHID;
extern TinyKeyboard Keyboard;
extern TinyMouse Mouse;
