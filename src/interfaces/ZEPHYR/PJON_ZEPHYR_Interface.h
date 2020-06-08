#if defined(ZEPHYR)

#include <string.h>
#include <stdlib.h>

static int serial_get_char(struct tty_serial* tty)
{
    u8_t tmp;
    tty_read(tty, &tmp, 1);
    return tmp;
}

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
#define PJON_DELAY k_sleep
#endif

#ifndef PJON_DELAY_MICROSECONDS
#define PJON_DELAY_MICROSECONDS k_usleep
#endif

#ifndef PJON_MILLIS
#define PJON_MILLIS k_uptime_get
#endif

#ifndef PJON_MICROS
#define PJON_MICROS 1000*k_uptime_get
#endif

// serial port handling

#ifndef PJON_SERIAL_TYPE
#define PJON_SERIAL_TYPE struct tty_serial*
#endif

#ifndef PJON_SERIAL_AVAILABLE
#define PJON_SERIAL_AVAILABLE(S) S != NULL and S->rx_get != S->rx_put
#endif

#ifndef PJON_SERIAL_READ
#define PJON_SERIAL_READ(S) serial_get_char(S)
#endif

#ifndef PJON_SERIAL_WRITE
#define PJON_SERIAL_WRITE(S, C) tty_write(S, &C, 1)
#endif

#ifndef PJON_SERIAL_FLUSH
#define PJON_SERIAL_FLUSH(S)
#endif

// io pin handling (for setting the rs485 txe pin) is not needed since we use the auto-direction
// feature of the rs485 transceiver

#ifndef PJON_IO_WRITE
#define PJON_IO_WRITE(P, C)
#endif

#ifndef PJON_IO_MODE
#define PJON_IO_MODE(P, C)
#endif

#ifndef LOW
#define LOW 0
#endif

#ifndef HIGH
#define HIGH 1
#endif

#endif
