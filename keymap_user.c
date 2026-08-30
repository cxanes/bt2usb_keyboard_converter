/*
 * Cxanes layout
 */
#include "config.h"
#include "keymap_common.h"

const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* 0: remapped Qwerty
     *         ,---------------. ,---------------. ,---------------.
     *         |F13|F14|F15|F16| |F17|F18|F19|F20| |F21|F22|F23|F24|
     * ,---.   |---------------| |---------------| |---------------| ,-----------. ,---------------. ,-------.
     * |Esc|   |F1 |F2 |F3 |F4 | |F5 |F6 |F7 |F8 | |F9 |F10|F11|F12| |PrS|ScL|Pau| |VDn|VUp|Mut|Pwr| | Help  |
     * `---'   `---------------' `---------------' `---------------' `-----------' `---------------' `-------'
     * ,-----------------------------------------------------------. ,-----------. ,---------------. ,-------.
     * |Esc|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|JPY|Bsp| |Ins|Hom|PgU| |NmL|  /|  *|  -| |Stp|Agn|
     * |-----------------------------------------------------------| |-----------| |---------------| |-------|
     * |Fn4  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|Fn5|  \  | |Del|End|PgD| |  7|  8|  9|  +| |Mnu|Und|
     * |-----------------------------------------------------------| `-----------' |---------------| |-------|
     * |Fn2   |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  :|  #|Retn|               |  4|  5|  6|KP,| |Sel|Cpy|
     * |-----------------------------------------------------------|     ,---.     |---------------| |-------|
     * |Fn0 |  <|  Z|  X|  C|  V|  B|  N|  M|  ,|  ,|  /| RO|Fn1   |     |Up |     |  1|  2|  3|KP=| |Exe|Pst|
     * |-----------------------------------------------------------| ,-----------. |---------------| |-------|
     * |Fn6|Ctl|Alt|MHEN|HNJ| Space  |H/E|HENK|KANA|Fn2|Ctl|Atl|Gui| |Lef|Dow|Rig| |  0    |  .|Ent| |Fnd|Cut|
     * `-----------------------------------------------------------' `-----------' `---------------' `-------'
     */
    KEYMAP_ALL(
              F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
    ESC,      F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12,           PSCR,SLCK,PAUS,    VOLD,VOLU,MUTE,PWR,     HELP,
    ESC, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS,EQL, JYEN,BSPC,     INS, HOME,PGUP,    NLCK,PSLS,PAST,PMNS,    STOP,AGIN,
    FN4, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,FN5,      BSLS,     DEL, END, PGDN,    P7,  P8,  P9,  PPLS,    MENU,UNDO,
    FN2, A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN,QUOT,     NUHS,ENT,                         P4,  P5,  P6,  PCMM,    SLCT,COPY,
    FN0, NUBS,Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,     RO,  FN1,           UP,           P1,  P2,  P3,  PEQL,    EXEC,PSTE,
    FN6, LCTL,LALT,MHEN,HANJ,     SPC,      HAEN,HENK,KANA,FN2, RCTL,RALT,RGUI,     LEFT,DOWN,RGHT,    P0,       PDOT,PENT,    FIND,CUT
    ),

    /* 1: plain Qwerty without layer switching
     *         ,---------------. ,---------------. ,---------------.
     *         |F13|F14|F15|F16| |F17|F18|F19|F20| |F21|F22|F23|F24|
     * ,---.   |---------------| |---------------| |---------------| ,-----------. ,---------------. ,-------.
     * |Esc|   |F1 |F2 |F3 |F4 | |F5 |F6 |F7 |F8 | |F9 |F10|F11|F12| |PrS|ScL|Pau| |VDn|VUp|Mut|Pwr| | Help  |
     * `---'   `---------------' `---------------' `---------------' `-----------' `---------------' `-------'
     * ,-----------------------------------------------------------. ,-----------. ,---------------. ,-------.
     * |  `|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|JPY|Bsp| |Ins|Hom|PgU| |NmL|  /|  *|  -| |Stp|Agn|
     * |-----------------------------------------------------------| |-----------| |---------------| |-------|
     * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|  \  | |Del|End|PgD| |  7|  8|  9|  +| |Mnu|Und|
     * |-----------------------------------------------------------| `-----------' |---------------| |-------|
     * |CapsL |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  :|  #|Retn|               |  4|  5|  6|KP,| |Sel|Cpy|
     * |-----------------------------------------------------------|     ,---.     |---------------| |-------|
     * |Shft|  <|  Z|  X|  C|  V|  B|  N|  M|  ,|  ,|  /| RO|Shift |     |Up |     |  1|  2|  3|KP=| |Exe|Pst|
     * |-----------------------------------------------------------| ,-----------. |---------------| |-------|
     * |Ctl|Gui|Alt|MHEN|HNJ| Space  |H/E|HENK|KANA|Alt|Gui|App|Ctl| |Lef|Dow|Rig| |  0    |  .|Ent| |Fnd|Cut|
     * `-----------------------------------------------------------' `-----------' `---------------' `-------'
     */
    KEYMAP_ALL(
              F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
    ESC,      F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12,           PSCR,SLCK,PAUS,    VOLD,VOLU,MUTE,PWR,     HELP,
    GRV, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS,EQL, JYEN,BSPC,     INS, HOME,PGUP,    NLCK,PSLS,PAST,PMNS,    STOP,AGIN,
    TAB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC,     BSLS,     DEL, END, PGDN,    P7,  P8,  P9,  PPLS,    MENU,UNDO,
    CAPS,A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN,QUOT,     NUHS,ENT,                         P4,  P5,  P6,  PCMM,    SLCT,COPY,
    LSFT,NUBS,Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,     RO,  RSFT,          UP,           P1,  P2,  P3,  PEQL,    EXEC,PSTE,
    LCTL,LGUI,LALT,MHEN,HANJ,     SPC,      HAEN,HENK,KANA,RALT,RGUI,APP, RCTL,     LEFT,DOWN,RGHT,    P0,       PDOT,PENT,    FIND,CUT
    ),

    /* 4: Fn'd
     * ,-----------------------------------------------------------.
     * |`  | F1| F2| F3| F4| F5| F6| F7| F8| F9|F10|F11|F12|Delete |
     * |-----------------------------------------------------------|
     * |Caps |PgU|Up |PgD|   |   |Cal|PgU|Hom|PgD|PrS|ScL|Pau|Ins  |
     * |-----------------------------------------------------------|
     * |      |Lef|Dow|Rig|Hom|Ins|Lef|Dow|Up |Rig|Bsp|Del|        |
     * |-----------------------------------------------------------|
     * |    |   |App|   |End|Fn3|   |End|Hom|PgU|PgD|End|   |PgU   |
     * |-----------------------------------------------------------|
     * |    |    |    |                        |    |Hom |PgD |End |
     * `-----------------------------------------------------------'
     */
    [4] = KEYMAP_ALL(
              TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
    TRNS,     TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,          TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,    TRNS,
    GRV, F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12, TRNS,DEL,      TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    CAPS,PGUP,UP,  PGDN,TRNS,TRNS,CALC,PGUP,HOME,PGDN,PSCR,SLCK,PAUS,     INS,      TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TRNS,LEFT,DOWN,RGHT,HOME,INS, LEFT,DOWN,UP,  RGHT,BSPC,DEL,      TRNS,TRNS,                        TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TRNS,TRNS,APP, TRNS,END, FN3, TRNS,END, TRNS,TRNS,TRNS,TRNS,     TRNS,PGUP,          PGUP,         TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,     TRNS,TRNS,TRNS,TRNS,HOME,PGDN,END,      HOME,PGDN,END,     TRNS,     TRNS,TRNS,    TRNS,TRNS
    ),

    /* 5: Default with Arrow
     * ,-----------------------------------------------------------.
     * |   |   |   |   |   |   |   |   |   |   |   |   |   |       |
     * |-----------------------------------------------------------|
     * |     |   |   |   |   |   |   |   |   |   |   |   |   |     |
     * |-----------------------------------------------------------|
     * |      |   |   |   |   |   |   |   |   |   |   |   |        |
     * |-----------------------------------------------------------|
     * |        |   |   |   |   |   |   |   |   |   |   |    |Up   |
     * |-----------------------------------------------------------|
     * |    |    |    |                        |    |Lef |Dow |Rig |
     * `-----------------------------------------------------------'
     */
    KEYMAP_ALL(
              TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
    TRNS,     TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,          TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,    TRNS,
    TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,     TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,TRNS,                        TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,UP,            TRNS,         TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,     TRNS,TRNS,TRNS,TRNS,LEFT,DOWN,RGHT,     TRNS,TRNS,TRNS,    TRNS,     TRNS,TRNS,    TRNS,TRNS

    ),

    /* 6: Shift + ESC = Tilde
     *    [ref] https://github.com/tmk/tmk_keyboard/wiki/FAQ-Keymap#esc-and--on-a-key
     * ,-----------------------------------------------------------.
     * |~  |   |   |   |   |   |   |   |   |   |   |   |   |       |
     * |-----------------------------------------------------------|
     * |Tab  |   |   |   |   |   |   |   |   |   |   |   |  ]|     |
     * |-----------------------------------------------------------|
     * |      |   |   |   |   |   |   |   |   |   |   |   |        |
     * |-----------------------------------------------------------|
     * |    |   |   |   |   |   |   |   |   |   |   |   |   |      |
     * |-----------------------------------------------------------|
     * |    |    |    |                        |    |    |    |    |
     * `-----------------------------------------------------------'
     */
    KEYMAP_ALL(
              TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
    TRNS,     TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,          TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,    TRNS,
    GRV, TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TAB, TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,RBRC,     TRNS,     TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,TRNS,                        TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,TRNS,          TRNS,         TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,     TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS,    TRNS,     TRNS,TRNS,    TRNS,TRNS
    ),

    /* 7: mouse layer
     * ,-----------------------------------------------------------.
     * |   |   |   |   |   |   |   |   |   |   |   |   |   |       |
     * |-----------------------------------------------------------|
     * |     |Mb1|McU|Mb2|   |   |   |Mb1|MwU|Mb2|   |   |   |     |
     * |-----------------------------------------------------------|
     * |      |McL|McD|McR|MwU|   |McL|McD|McU|McR|   |   |Fn7     |
     * |-----------------------------------------------------------|
     * |    |   |   |Mb3|MwD|   |   |MwD|Mb3|   |   |   |   |      |
     * |-----------------------------------------------------------|
     * |    |    |    |                        |    |    |    |    |
     * `-----------------------------------------------------------'
     * Mc: Mouse Cursor / Mb: Mouse Button / Mw: Mouse Wheel8
     */
    KEYMAP_ALL(
              TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
    TRNS,     TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,SLEP,WAKE,          TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,    TRNS,
    TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TRNS,BTN1,MS_U,BTN2,TRNS,NO,  NO,  BTN1,WH_U,BTN2,NO,  NO,  NO,       TRNS,     TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TRNS,MS_L,MS_D,MS_R,WH_U,NO,  MS_L,MS_D,MS_U,MS_R,TRNS,TRNS,     TRNS,FN7,                         TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TRNS,NO,  NO,  BTN3,WH_D,NO,  NO,  WH_D,BTN3,TRNS,TRNS,TRNS,     TRNS,TRNS,          TRNS,         TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,
    TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,     TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS,    TRNS,     TRNS,TRNS,    TRNS,TRNS
    ),
};

const action_t PROGMEM fn_actions[] = {
    [0] = ACTION_LAYER_MODS(6, MOD_LSFT),
    [1] = ACTION_LAYER_MODS(6, MOD_RSFT),
    [2] = ACTION_LAYER_BIT_XOR(1, 0b0011, ON_BOTH),
    [3] = ACTION_MODS_KEY(MOD_LSFT, KC_INS),  // Shift + Insert
    [4] = ACTION_MODS_TAP_KEY(MOD_LCTL, KC_TAB),
    [5] = ACTION_MODS_TAP_KEY(MOD_RALT, KC_RBRC),
    [6] = ACTION_LAYER_MOMENTARY(7),
    [7] = ACTION_LAYER_TOGGLE(5),
};
