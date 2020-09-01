#pragma once

#if defined(ZEPHYR)

#define OUTPUT GPIO_OUTPUT

#include <drivers/gpio.h>
#include <zephyr.h>
#include <stdint.h>
#include <string>

int serial_get_char(struct device* dev);
bool serial_char_available(struct device* dev);
void uart_irq_callback(struct device *dev);
int serial_put_char(struct device* dev, uint8_t byte);
struct device* serial_open(const char* dt_label);
void serial_close(const char* dt_label);
void digitalWrite(int pin, bool state);
void pinMode(int pin, int mode);
void serial_flush(struct device* dev);

// deal with randomness

#ifndef PJON_RANDOM
#define PJON_RANDOM(randMax) (int)((1.0 + randMax) * rand() / ( RAND_MAX + 1.0 ))
#endif

#ifndef PJON_RANDOM_SEED
#define PJON_RANDOM_SEED srand
#endif

#ifndef A0
#define A0 0
#endif

#ifndef PJON_ANALOG_READ
#define PJON_ANALOG_READ(P) 0
#endif

// delay and timing functions

#ifndef PJON_DELAY
#define PJON_DELAY(x) (k_sleep(K_MSEC(x)))
#endif

#ifndef PJON_DELAY_MICROSECONDS
#define PJON_DELAY_MICROSECONDS(x) (k_sleep(K_USEC(x)))
#endif

#ifndef PJON_MILLIS
#define PJON_MILLIS k_uptime_get
#endif

#ifndef PJON_MICROS
#define PJON_MICROS 1000*k_uptime_get
#endif

// serial port handling

#ifndef PJON_SERIAL_TYPE
#define PJON_SERIAL_TYPE struct device*
#endif

#ifndef PJON_SERIAL_AVAILABLE
#define PJON_SERIAL_AVAILABLE(S) serial_char_available(S)
#endif

#ifndef PJON_SERIAL_READ
#define PJON_SERIAL_READ(S) serial_get_char(S)
#endif

#ifndef PJON_SERIAL_WRITE
#define PJON_SERIAL_WRITE(S, C) serial_put_char(S, C)
#endif

#ifndef PJON_SERIAL_FLUSH
#define PJON_SERIAL_FLUSH(S) serial_flush(S)
#endif

// io pin handling (for setting the rs485 txe pin) is not needed since we use the auto-direction
// feature of the rs485 transceiver

#ifndef PJON_IO_WRITE
#define PJON_IO_WRITE digitalWrite
#endif

#ifndef PJON_IO_MODE
#define PJON_IO_MODE pinMode
#endif

#ifndef LOW
#define LOW 0
#endif

#ifndef HIGH
#define HIGH 1
#endif

#endif
