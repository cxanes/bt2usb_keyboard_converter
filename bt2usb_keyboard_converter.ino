// SPDX-FileCopyrightText: 2025 John Park for Adafruit Industries
//
// SPDX-License-Identifier: MIT
//
// made with assistance from Claude Sonnet 4
//
// Unified Bluetooth HID Bridge - BT Classic & BLE
// put keyboard in pairing mode, then press reset or BOOT button
// slow blinks mean scanning
// fast blinks mean keyboard detected, press a keyboard key to connect

// === CONFIGURATION ===
#define DEBUG_MODE false   // Set to false to reduce serial output
#define ENABLE_SERIAL false  // Set to false to avoid possible usb/com port conflict
#define ENABLE_KEY_REMAP 1

#include <BluetoothHIDMaster.h>
#include <LittleFS.h>
#include "my_hid_device.h"
#if ENABLE_KEY_REMAP
#include "host.h"
#include "timer.h"
#include "action.h"
#endif

enum class ScanMode {
  BT,
  BLE,
  BOTH
};

// Connection state management
enum class ConnectionState {
    INIT,
    CONNECTING,
    CONNECTED
};

const ScanMode SCAN_MODE = ScanMode::BLE;

static ConnectionState g_state = ConnectionState::INIT;
static BluetoothHIDMaster g_btHID;
static HIDKeyStream g_keyStream;
static bool g_isBLE = false;

// Timing management
unsigned long g_stateStartTime = 0;
const unsigned long CLASSIC_SCAN_TIMEOUT = 10000;
const unsigned long BLE_SCAN_TIMEOUT = 15000;
const unsigned long CONNECTION_CHECK_TIMEOUT = 5000;

class LEDBlinker {
public:
  LEDBlinker() = default;
  ~LEDBlinker() = default;

  void SetLED(bool on)
  {
    digitalWrite(LED_BUILTIN, on ? HIGH : LOW);
    m_led = on;
  }

  void SetInterval(uint32_t interval)
  {
    m_interval = interval;
  }

  void Blink()
  {
    if (m_interval == 0) {
      return;
    }

    unsigned long currentTime = millis();
    if (currentTime >= m_timer) {
      SetLED(!m_led);

      m_timer = currentTime + m_interval;
    }
  }

private:
  bool m_led = false;
  uint32_t m_interval = 0;
  unsigned long m_timer = 0;
};

static const char *macToString(const uint8_t *addr, uint8_t addrType)
{
  static char mac[32];
  sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x,%d", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addrType ? 1 : 0);
  return mac;
}

static inline bool isValidAddress(uint8_t (&address)[6])
{
  uint8_t value = 0;
  for (int i = 0; i < 6; i++) {
    value |= address[i];
  }

  return (value != 0);
}

static LEDBlinker g_ledBlinker{};

class PairingManager {
  struct PairingInfo {
    char hdr[16];     // The header marker so we know this is our data
    uint8_t addr[6];  // MAC of the peer we last connected to
    uint8_t addrType; // Type of address (normal or randomized) of the last peer
  };

public:
  PairingManager()
  {}

  ~PairingManager()
  {}

  void init()
  {
    PairingInfo data = {};
    File f = LittleFS.open(PAIRING_INFO_PATH, "r");
    if (!f) {
      Serial.printf("%p not found\n", PAIRING_INFO_PATH);
      return;
    }

    int size = f.read((uint8_t *)&data, sizeof(PairingInfo));
    if (size != sizeof(PairingInfo)) {
      Serial.printf("%p read failed\n", PAIRING_INFO_PATH);
      return;
    }

    f.close();

    if (memcmp(data.hdr, DATAHEADER, sizeof(data.hdr))) {
      // Not our data, just clear things
      Serial.printf("No previous paired device found in %s\n", PAIRING_INFO_PATH);
      bzero(m_lastAddr, 6);
      m_lastAddrType = 0;
    } else {
      // Our baby, copy it to local storage
      Serial.printf("Previoudly paired device found: %s\n", macToString(data.addr, data.addrType));
      memcpy(m_lastAddr, data.addr, sizeof(m_lastAddr));
      m_lastAddrType = data.addrType;
    }
  }

  void clear()
  {
    bzero(m_lastAddr, sizeof(m_lastAddr));
  }

  bool connect(BluetoothHIDMaster &hid, bool ble)
  {
    bool connected = false;
    int type = 0;
    if (isValidAddress(m_lastAddr)) {
      type = 0;
      Serial.printf("Connect paired device directly: %s\n", macToString(m_lastAddr, m_lastAddrType));
      if (ble) {
        connected = g_btHID.connectBLE(m_lastAddr, m_lastAddrType, ledBlink);
      } else {
        connected = g_btHID.connect(m_lastAddr);
      }
    } else {
      type = 1;
      Serial.println("Waiting for device...");
      if (ble) {
        connected = g_btHID.connectBLE(ledBlink);
      } else {
        connected = g_btHID.connectAny();
      }

      memcpy(m_lastAddr, g_btHID.lastConnectedAddress(), sizeof(m_lastAddr));
      m_lastAddrType = g_btHID.lastConnectedAddressType();
      if (isValidAddress(m_lastAddr)) {
        savePairingInfo();
      }
    }

    Serial.printf("%s connection initiated (%d, %d)\n", ble ? "BLE" : "BT", type, (int)connected);
    return connected;
  }

private:
  void savePairingInfo()
  {
    PairingInfo data;
    memcpy(data.hdr, DATAHEADER, sizeof(data.hdr));
    memcpy(data.addr, m_lastAddr, sizeof(data.addr));
    data.addrType = m_lastAddrType;

    File f = LittleFS.open(PAIRING_INFO_PATH, "w");
    if (!f) {
      Serial.printf("%p not created\n", PAIRING_INFO_PATH);
      return;
    }

    int size = f.write((uint8_t *)&data, sizeof(PairingInfo));
    if (size != sizeof(PairingInfo)) {
      Serial.printf("%p write failed\n", PAIRING_INFO_PATH);
      return;
    }

    f.close();
    Serial.printf("Wrote paired device to %s: %s\n", PAIRING_INFO_PATH,
      macToString(m_lastAddr, m_lastAddrType));
  }

  uint8_t m_lastAddr[6] = {};
  uint8_t m_lastAddrType = 0;

  // From libraries/BluetoothHIDMaster/examples/KeyboardPianoBLE/KeyboardPianoBLE.ino
  static constexpr const char *DATAHEADER = "Last BLE Address"; // exactly 16 chars + \0
  static constexpr const char *PAIRING_INFO_PATH = "/pairing_info.bin";

  static void ledBlink(void *data)
  {
    g_ledBlinker.Blink();
  }
};

#if ENABLE_KEY_REMAP
static host_driver_t g_hostDriver = {
  NULL,
  [](report_keyboard_t *report) {
    Keyboard.sendReport((KeyReport*)report);
  },
  [](report_mouse_t *report) {
    Mouse.buttons(report->buttons);
    Mouse.move(report->x, report->y, report->v);
  },
  [](uint16_t key) {
    Keyboard.sendSystemReport(key);
  },
  [](uint16_t key) {
    Keyboard.sendConsumerReport(key);
  }
};

/* KEY CODE to Matrix
 *
 * HID keycode(1 byte):
 * Higher 5 bits indicates ROW and lower 3 bits COL.
 *
 *  7 6 5 4 3 2 1 0
 * +---------------+
 * |  ROW  |  COL  |
 * +---------------+
 *
 * Matrix space(16 * 16):
 *   r\c0123456789ABCDEF
 *   0 +----------------+
 *   : |                |
 *   : |                |
 *  16 +----------------+
 *
 * From: tmk_keyboard/converter/usb_usb/usb_usb.cpp
 */
#define ROW_MASK 0xF0
#define COL_MASK 0x0F
#define CODE(row, col)  (((row) << 4) | (col))
#define ROW(code)       ((uint8_t)(((code) & ROW_MASK) >> 4))
#define COL(code)       ((uint8_t)((code) & COL_MASK))
class KeyHandler {
public:
  KeyHandler() = default;
  ~KeyHandler() = default;

  void init()
  {
    host_set_driver(&g_hostDriver);
  }

  void handle(int key, bool state)
  {
    if (key >= 256) return;

    keyevent_t e = (keyevent_t){
      .key = (keypos_t){.col = COL(key), .row = ROW(key) },
      .pressed = state,
      .time = (uint16_t)(timer_read() | 1) /* time should not be 0 */
    };

    action_exec(e);
  }

  void clear()
  {}
};
#else
static uint8_t hidToUsbKey(uint8_t hidKey)
{
  switch (hidKey) {
    // Letters
    case 0x04: return 'a';    case 0x05: return 'b';    case 0x06: return 'c';    case 0x07: return 'd';
    case 0x08: return 'e';    case 0x09: return 'f';    case 0x0A: return 'g';    case 0x0B: return 'h';
    case 0x0C: return 'i';    case 0x0D: return 'j';    case 0x0E: return 'k';    case 0x0F: return 'l';
    case 0x10: return 'm';    case 0x11: return 'n';    case 0x12: return 'o';    case 0x13: return 'p';
    case 0x14: return 'q';    case 0x15: return 'r';    case 0x16: return 's';    case 0x17: return 't';
    case 0x18: return 'u';    case 0x19: return 'v';    case 0x1A: return 'w';    case 0x1B: return 'x';
    case 0x1C: return 'y';    case 0x1D: return 'z';

    // Numbers
    case 0x1E: return '1';    case 0x1F: return '2';    case 0x20: return '3';    case 0x21: return '4';
    case 0x22: return '5';    case 0x23: return '6';    case 0x24: return '7';    case 0x25: return '8';
    case 0x26: return '9';    case 0x27: return '0';

    // Special keys
    case 0x28: return KEY_RETURN;        case 0x29: return KEY_ESC;
    case 0x2A: return KEY_BACKSPACE;     case 0x2B: return KEY_TAB;
    case 0x2C: return ' ';               case 0x39: return KEY_CAPS_LOCK;

    // Symbols
    case 0x2D: return '-';    case 0x2E: return '=';    case 0x2F: return '[';    case 0x30: return ']';
    case 0x31: return '\\';   case 0x33: return ';';    case 0x34: return '\'';   case 0x35: return '`';
    case 0x36: return ',';    case 0x37: return '.';    case 0x38: return '/';

    // Function keys
    case 0x3A: return KEY_F1;     case 0x3B: return KEY_F2;     case 0x3C: return KEY_F3;
    case 0x3D: return KEY_F4;     case 0x3E: return KEY_F5;     case 0x3F: return KEY_F6;
    case 0x40: return KEY_F7;     case 0x41: return KEY_F8;     case 0x42: return KEY_F9;
    case 0x43: return KEY_F10;    case 0x44: return KEY_F11;    case 0x45: return KEY_F12;

    // Arrow keys
    case 0x4F: return KEY_RIGHT_ARROW;   case 0x50: return KEY_LEFT_ARROW;
    case 0x51: return KEY_DOWN_ARROW;    case 0x52: return KEY_UP_ARROW;

    // Navigation
    case 0x49: return KEY_INSERT;        case 0x4A: return KEY_HOME;
    case 0x4B: return KEY_PAGE_UP;       case 0x4C: return KEY_DELETE;
    case 0x4D: return KEY_END;           case 0x4E: return KEY_PAGE_DOWN;

    // Modifiers
    case 0xE0: return KEY_LEFT_CTRL;     case 0xE1: return KEY_LEFT_SHIFT;
    case 0xE2: return KEY_LEFT_ALT;      case 0xE3: return KEY_LEFT_GUI;
    case 0xE4: return KEY_RIGHT_CTRL;    case 0xE5: return KEY_RIGHT_SHIFT;
    case 0xE6: return KEY_RIGHT_ALT;     case 0xE7: return KEY_RIGHT_GUI;

    default: return 0;
  }
}

class KeyHandler {
public:
  KeyHandler() = default;
  ~KeyHandler() = default;

  void init()
  {}

  void handle(int key, bool state)
  {
    if (key >= 256) return;

    bool isModifier = (key >= 0xE0 && key <= 0xE7);

    if (isModifier) {
      uint8_t usbKey = hidToUsbKey(key);
      if (state) Keyboard.press(usbKey);
      else Keyboard.release(usbKey);
      return;
    }

    // Handle regular keys
    uint8_t usbKey = hidToUsbKey(key);
    if (usbKey != 0) {
      if (state && !keyPressed[key]) {
        keyPressed[key] = true;
        Keyboard.press(usbKey);
      } else if (!state && keyPressed[key]) {
        keyPressed[key] = false;
        Keyboard.release(usbKey);
      }
    }
  }

  void clear()
  {
    bzero(keyPressed, sizeof(keyPressed));
  }

private:
  bool keyPressed[256] = {0}; // Track which keys are currently pressed
};
#endif

static PairingManager g_pairingManager{};
static KeyHandler g_keyHandler{};

static void setupHIDCallbacks() {
  // Mouse callbacks
  g_btHID.onMouseMove([](void *cbdata, int dx, int dy, int dw) {
    (void) cbdata;
    if (DEBUG_MODE) {
      Serial.printf("BLE Mouse: X:%d  Y:%d  Wheel:%d\n", dx, dy, dw);
    }
    Mouse.move(dx, dy);
    if (dw != 0) Mouse.move(0, 0, dw);
  });

  g_btHID.onMouseButton([](void *cbdata, int butt, bool down) {
    (void) cbdata;
    if (DEBUG_MODE) {
      Serial.printf("BLE Mouse: Button %d %s\n", butt, down ? "DOWN" : "UP");
    }
    if (down) {
      if (butt == 1) Mouse.press(MOUSE_LEFT);
      else if (butt == 2) Mouse.press(MOUSE_RIGHT);
      else if (butt == 3) Mouse.press(MOUSE_MIDDLE);
    } else {
      if (butt == 1) Mouse.release(MOUSE_LEFT);
      else if (butt == 2) Mouse.release(MOUSE_RIGHT);
      else if (butt == 3) Mouse.release(MOUSE_MIDDLE);
    }
  });

  // Keyboard callbacks
  g_btHID.onKeyDown([](void *cbdata, int key) {
    handleKey(key, true);
  }, (void *)true);

  g_btHID.onKeyUp([](void *cbdata, int key) {
    handleKey(key, false);
  }, (void *)false);
}

static void handleKey(int key, bool state) {
  if (DEBUG_MODE) {
    Serial.printf("Keyboard: %02x %s\n", key, state ? "DOWN" : "UP");
  }

  if (key >= 256) return; // Bounds check

  // Forward the key to USB
  g_keyHandler.handle(key, state);
}

static const char *scanModeToString(ScanMode scanMode)
{
  switch (scanMode) {
    case ScanMode::BT:
      return "BT";
    case ScanMode::BLE:
      return "BLE";
    case ScanMode::BOTH:
    default:
      return "BOTH";
  }
}

static void tryConnect()
{
  g_state = ConnectionState::CONNECTING;
  bool connected = g_pairingManager.connect(g_btHID, g_isBLE);
  if (connected) {
    g_state = ConnectionState::CONNECTED;
    g_ledBlinker.SetInterval(0);
    g_ledBlinker.SetLED(true);
  } else {
    g_ledBlinker.Blink();
  }
}

void setup() {
  if (ENABLE_SERIAL || DEBUG_MODE) {
    Serial.begin(115200);
  } else {
    TinyHID.removeSerial();
  }
  delay(3000);

  Serial.println("=== UNIFIED BLUETOOTH HID BRIDGE ===");
  Serial.printf("Scan mode: %s\n", scanModeToString(SCAN_MODE));
  Serial.println("Put your Bluetooth device in pairing mode now");

  // Initialize LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("LED initialized");

  // Initialize USB HID
  Keyboard.begin();
  Mouse.begin();
  Serial.println("USB HID initialized");

  // Start with LED blinking to show we're alive
  g_ledBlinker.SetLED(true);

  LittleFS.begin();
  g_pairingManager.init();

  g_keyHandler.init();

  Serial.println("Initializing BT/BLE stack...");

  // Setup the HID key to ASCII conversion
  g_keyStream.begin();
  Serial.println("KeyStream initialized");

  // Setup BLE callbacks
  setupHIDCallbacks();
  Serial.println("BT/BLE callbacks configured");

  // Start BLE HID master
  g_isBLE = (SCAN_MODE == ScanMode::BT) ? false : true;
  g_btHID.begin(g_isBLE);
  Serial.println("BT/BLE HID master started");

  g_ledBlinker.SetInterval(250);
  g_stateStartTime = millis();
  tryConnect();
}

void loop() {
  unsigned long currentTime = millis();

  // Handle state timeouts
  handleStateTimeouts(currentTime);

  // Handle LED patterns
  handleLEDPatterns(currentTime);

  // Handle BOOTSEL button
  handleBootselButton();

  delay(10);
}

void handleStateTimeouts(unsigned long currentTime) {
  switch (g_state) {
    case ConnectionState::CONNECTING: {
      if (BOOTSEL) {
        return;
      }

      const unsigned long timeout = g_isBLE ? BLE_SCAN_TIMEOUT : CLASSIC_SCAN_TIMEOUT;
      if (currentTime - g_stateStartTime > timeout) {
        if (SCAN_MODE == ScanMode::BOTH) {
          g_isBLE = !g_isBLE;
          g_btHID.end();
          g_btHID.begin(g_isBLE);
        }

        tryConnect();
      }
      break;
    }
    case ConnectionState::CONNECTED: {
      static unsigned long g_checkConnectionTime = 0;
      if (currentTime - g_checkConnectionTime > CONNECTION_CHECK_TIMEOUT) {
        g_checkConnectionTime = currentTime;
        if (!g_btHID.connected()) {
          tryConnect();
        }
      }
      break;
    }
  }
}

void handleLEDPatterns(unsigned long currentTime) {
  static bool prevIsSuspended = false;
  bool currentIsSuspended = TinyHID.suspended();
  if (prevIsSuspended != currentIsSuspended) {
    g_ledBlinker.SetLED(!currentIsSuspended);
    prevIsSuspended = currentIsSuspended;
    return;
  } else if (currentIsSuspended) {
    g_ledBlinker.SetLED(false);
    return;
  }
}

void handleBootselButton() {
  if (!BOOTSEL) {
    return;
  }

  while (BOOTSEL) delay(1);

  Serial.println("\nBOOTSEL pressed - restarting scan sequence");

  g_pairingManager.clear();

  // Clean up current connections
  if (g_state == ConnectionState::CONNECTED) {
    g_btHID.disconnect();
    g_btHID.clearPairing();
    g_state = ConnectionState::INIT;
  }

  // Reset all state
  Keyboard.releaseAll();
  Mouse.release(MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE);

  g_keyHandler.clear();

  tryConnect();
}
