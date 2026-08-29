#ifndef WAIT_H
#define WAIT_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

#   define wait_ms(ms)  delay(ms)
#   define wait_us(us)  delayMicroseconds(us)

#ifdef __cplusplus
}
#endif

#endif
