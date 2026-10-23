// TinyUSB_KeyboardMouse.cpp
// Written by Claude
#include "my_hid_device.h"
#include <Adafruit_TinyUSB.h>

// ---------------- HID interfaces ----------------

// Adafruit TinyUSB for RP2040 allows only CFG_TUD_HID HID interfaces (2 by default).
// - CFG_TUD_HID >= 3: Keyboard, Mouse and Consumer/System are three separate interfaces,
//   and both keyboard and mouse are boot devices.
// - CFG_TUD_HID == 2 (default): Consumer/System share the mouse interface using Report IDs,
//   so the mouse is no longer a boot device (the keyboard stays a boot keyboard).
#if CFG_TUD_HID >= 3
  #define TINYHID_SEPARATE_MEDIA 1
#else
  #define TINYHID_SEPARATE_MEDIA 0
#endif

#if TINYHID_SEPARATE_MEDIA
enum {
  REPORT_ID_MOUSE    = 0,   // Boot mouse: no Report ID
  REPORT_ID_CONSUMER = 1,
  REPORT_ID_SYSTEM   = 2,
};
#else
enum {
  REPORT_ID_MOUSE    = 1,
  REPORT_ID_CONSUMER = 2,
  REPORT_ID_SYSTEM   = 3,
};
#endif

static uint8_t const desc_kbd_report[] = { TUD_HID_REPORT_DESC_KEYBOARD() };

// Consumer: 16-bit usage array (0-0x3FF)
// System Control: 16-bit usage array (0-0xB7), so raw usages like 0x81/0x82/0x83 can be sent directly
#define TINYHID_DESC_MEDIA() \
  TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(REPORT_ID_CONSUMER)), \
  HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP), \
  HID_USAGE(HID_USAGE_DESKTOP_SYSTEM_CONTROL), \
  HID_COLLECTION(HID_COLLECTION_APPLICATION), \
    HID_REPORT_ID(REPORT_ID_SYSTEM) \
    HID_LOGICAL_MIN(0x00), \
    HID_LOGICAL_MAX_N(0x00B7, 2), \
    HID_USAGE_MIN(0x00), \
    HID_USAGE_MAX_N(0x00B7, 2), \
    HID_REPORT_COUNT(1), \
    HID_REPORT_SIZE(16), \
    HID_INPUT(HID_DATA | HID_ARRAY | HID_ABSOLUTE), \
  HID_COLLECTION_END

#if TINYHID_SEPARATE_MEDIA
static uint8_t const desc_mouse_report[] = { TUD_HID_REPORT_DESC_MOUSE() };
static uint8_t const desc_media_report[] = { TINYHID_DESC_MEDIA() };
#else
static uint8_t const desc_mouse_report[] = {
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(REPORT_ID_MOUSE)),
  TINYHID_DESC_MEDIA()
};
#endif

static Adafruit_USBD_HID kbdHid;
static Adafruit_USBD_HID mouseHid;
#if TINYHID_SEPARATE_MEDIA
static Adafruit_USBD_HID mediaHid;
#else
#define mediaHid mouseHid   // Consumer/System share the mouse interface
#endif
static bool hidStarted = false;

TinyHIDDevice TinyHID;
TinyKeyboard Keyboard;
TinyMouse Mouse;

void TinyHIDDevice::removeSerial() {
  _removeSerial = true;
}

void TinyHIDDevice::begin() {
  if (hidStarted) return;
  hidStarted = true;

  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  if (_removeSerial) {
    // If compilation fails because clearConfiguration is not found, update the Adafruit TinyUSB library.
    TinyUSBDevice.clearConfiguration();
  }

  // Interface A: Boot Keyboard
  kbdHid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  kbdHid.setPollInterval(2);
  kbdHid.setReportDescriptor(desc_kbd_report, sizeof(desc_kbd_report));
  kbdHid.setStringDescriptor("Pico Keyboard");
  kbdHid.begin();

#if TINYHID_SEPARATE_MEDIA
  // Interface B: Boot Mouse
  mouseHid.setBootProtocol(HID_ITF_PROTOCOL_MOUSE);
  mouseHid.setPollInterval(2);
  mouseHid.setReportDescriptor(desc_mouse_report, sizeof(desc_mouse_report));
  mouseHid.setStringDescriptor("Pico Mouse");
  mouseHid.begin();

  // Interface C: Consumer + System Control (non-boot, uses Report IDs)
  mediaHid.setBootProtocol(HID_ITF_PROTOCOL_NONE);
  mediaHid.setPollInterval(10);
  mediaHid.setReportDescriptor(desc_media_report, sizeof(desc_media_report));
  mediaHid.setStringDescriptor("Pico Media");
  mediaHid.begin();
#else
  // Interface B: Mouse + Consumer + System Control (non-boot, uses Report IDs)
  mouseHid.setBootProtocol(HID_ITF_PROTOCOL_NONE);
  mouseHid.setPollInterval(2);
  mouseHid.setReportDescriptor(desc_mouse_report, sizeof(desc_mouse_report));
  mouseHid.setStringDescriptor("Pico Mouse");
  mouseHid.begin();
#endif

  // If already enumerated, reconnect so the host reads the new descriptors
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }
}

bool TinyHIDDevice::suspended() {
  return TinyUSBDevice.suspended();
}

bool TinyHIDDevice::remoteWakeup() {
  return TinyUSBDevice.remoteWakeup();
}

bool TinyHIDDevice::ready() {
  return TinyUSBDevice.mounted() && !TinyUSBDevice.suspended();
}

// Wait until the HID endpoint can send; issue remote wakeup instead when suspended
static bool prepareSend(Adafruit_USBD_HID &hid) {
  if (TinyUSBDevice.suspended()) {
    TinyUSBDevice.remoteWakeup();
    return false;
  }
  if (!TinyUSBDevice.mounted()) return false;

  uint32_t start = millis();
  while (!hid.ready()) {
#ifdef TINYUSB_NEED_POLLING_TASK
    TinyUSBDevice.task();
#endif
    if (millis() - start > 50) return false;
    delay(1);
  }
  return true;
}

// ---------------- Keyboard ----------------

#define SHIFT 0x80

// en_US: ASCII -> HID usage (SHIFT bit means Shift must be held)
static const uint8_t asciiMap[128] = {
  0, 0, 0, 0, 0, 0, 0, 0,                                         // 0x00
  0x2A, 0x2B, 0x28, 0, 0, 0, 0, 0,                                // 0x08 BS TAB LF ... CR (ignored)
  0, 0, 0, 0, 0, 0, 0, 0,                                         // 0x10
  0, 0, 0, 0x29, 0, 0, 0, 0,                                      // 0x18 ESC
  0x2C, 0x1E|SHIFT, 0x34|SHIFT, 0x20|SHIFT,                       // ' ' ! " #
  0x21|SHIFT, 0x22|SHIFT, 0x24|SHIFT, 0x34,                       // $ % & '
  0x26|SHIFT, 0x27|SHIFT, 0x25|SHIFT, 0x2E|SHIFT,                 // ( ) * +
  0x36, 0x2D, 0x37, 0x38,                                         // , - . /
  0x27, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24,                 // 0-7
  0x25, 0x26, 0x33|SHIFT, 0x33, 0x36|SHIFT, 0x2E, 0x37|SHIFT, 0x38|SHIFT, // 8 9 : ; < = > ?
  0x1F|SHIFT, 0x04|SHIFT, 0x05|SHIFT, 0x06|SHIFT,                 // @ A B C
  0x07|SHIFT, 0x08|SHIFT, 0x09|SHIFT, 0x0A|SHIFT,                 // D E F G
  0x0B|SHIFT, 0x0C|SHIFT, 0x0D|SHIFT, 0x0E|SHIFT,                 // H I J K
  0x0F|SHIFT, 0x10|SHIFT, 0x11|SHIFT, 0x12|SHIFT,                 // L M N O
  0x13|SHIFT, 0x14|SHIFT, 0x15|SHIFT, 0x16|SHIFT,                 // P Q R S
  0x17|SHIFT, 0x18|SHIFT, 0x19|SHIFT, 0x1A|SHIFT,                 // T U V W
  0x1B|SHIFT, 0x1C|SHIFT, 0x1D|SHIFT, 0x2F,                       // X Y Z [
  0x31, 0x30, 0x23|SHIFT, 0x2D|SHIFT,                             // \ ] ^ _
  0x35, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,                 // ` a-g
  0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,                 // h-o
  0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A,                 // p-w
  0x1B, 0x1C, 0x1D, 0x2F|SHIFT, 0x31|SHIFT, 0x30|SHIFT, 0x35|SHIFT, 0 // x y z { | } ~ DEL
};

void TinyKeyboard::begin(const uint8_t *layout) {
  (void)layout;
  TinyHID.begin();
}

void TinyKeyboard::end() {
  releaseAll();
}

void TinyKeyboard::sendReport(KeyReport *keys) {
  if (!keys) return;
  if (!prepareSend(kbdHid)) return;
  kbdHid.keyboardReport(0, keys->modifiers, keys->keys);   // Boot format: report_id = 0
}

void TinyKeyboard::sendConsumerReport(uint16_t key) {
  if (!prepareSend(mediaHid)) return;
  mediaHid.sendReport(REPORT_ID_CONSUMER, &key, sizeof(key));   // RP2040 is little-endian
}

void TinyKeyboard::sendSystemReport(uint16_t key) {
  if (!prepareSend(mediaHid)) return;
  mediaHid.sendReport(REPORT_ID_SYSTEM, &key, sizeof(key));
}

// Convert an Arduino key value to a HID usage; modifiers update `modifiers` directly and return 0
// Returns -1 for unsupported characters
static int toUsage(uint8_t k, uint8_t &modifiers, bool pressing) {
  if (k >= 136) {
    return k - 136;                         // Non-printing keys (arrows, F1, etc.)
  }
  if (k >= 128) {
    uint8_t bit = 1 << (k - 128);           // Modifier keys
    if (pressing) modifiers |= bit; else modifiers &= ~bit;
    return 0;
  }
  uint8_t c = asciiMap[k];
  if (!c) return -1;
  if (c & SHIFT) {
    if (pressing) modifiers |= KEYBOARD_MODIFIER_LEFTSHIFT;
    else modifiers &= ~KEYBOARD_MODIFIER_LEFTSHIFT;
  }
  return c & 0x7F;
}

size_t TinyKeyboard::press(uint8_t k) {
  int usage = toUsage(k, _keyReport.modifiers, true);
  if (usage < 0) {
    setWriteError();
    return 0;
  }
  if (usage > 0) {
    bool present = false;
    for (int i = 0; i < 6; i++) {
      if (_keyReport.keys[i] == usage) present = true;
    }
    if (!present) {
      int i = 0;
      while (i < 6 && _keyReport.keys[i] != 0) i++;
      if (i == 6) {
        setWriteError();
        return 0;
      }
      _keyReport.keys[i] = usage;
    }
  }
  sendReport(&_keyReport);
  return 1;
}

size_t TinyKeyboard::release(uint8_t k) {
  int usage = toUsage(k, _keyReport.modifiers, false);
  if (usage < 0) return 0;
  if (usage > 0) {
    for (int i = 0; i < 6; i++) {
      if (_keyReport.keys[i] == usage) _keyReport.keys[i] = 0;
    }
  }
  sendReport(&_keyReport);
  return 1;
}

void TinyKeyboard::releaseAll() {
  memset(&_keyReport, 0, sizeof(_keyReport));
  sendReport(&_keyReport);
}

size_t TinyKeyboard::write(uint8_t k) {
  size_t p = press(k);
  release(k);
  return p;
}

size_t TinyKeyboard::write(const uint8_t *buffer, size_t size) {
  size_t n = 0;
  while (size--) {
    if (*buffer != '\r') {
      if (write(*buffer)) n++;
      else break;
    }
    buffer++;
  }
  return n;
}

// ---------------- Mouse ----------------

void TinyMouse::begin() {
  TinyHID.begin();
}

void TinyMouse::end() {
  _buttons = 0;
  sendReport(0, 0, 0);
}

bool TinyMouse::sendReport(signed char x, signed char y, signed char wheel) {
  if (!prepareSend(mouseHid)) return false;
  return mouseHid.mouseReport(REPORT_ID_MOUSE, _buttons, x, y, wheel, 0);
}

void TinyMouse::click(uint8_t b) {
  _buttons = b;
  sendReport(0, 0, 0);
  _buttons = 0;
  sendReport(0, 0, 0);
}

void TinyMouse::move(signed char x, signed char y, signed char wheel) {
  sendReport(x, y, wheel);
}

void TinyMouse::press(uint8_t b) {
  _buttons |= b;
  sendReport(0, 0, 0);
}

void TinyMouse::release(uint8_t b) {
  _buttons &= ~b;
  sendReport(0, 0, 0);
}

bool TinyMouse::isPressed(uint8_t b) {
  return (_buttons & b) != 0;
}

void TinyMouse::buttons(uint8_t b) {
  _buttons = b;
}

uint8_t TinyMouse::buttons() {
  return _buttons;
}
