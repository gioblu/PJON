#if defined(ZEPHYR)

#define OUTPUT GPIO_OUTPUT

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <drivers/gpio.h>
#include <drivers/uart.h>
#include <sys/ring_buffer.h>
#include <logging/log.h>
#include <devicetree.h>

RING_BUF_DECLARE(ringbuffer_pjon, CONFIG_PJON_MTU);
static uint8_t tmp_buffer[40];
static int serial_get_char(struct device* dev)
{
    size_t  ret;
    uint8_t b;
    ret = ring_buf_get(&ringbuffer_pjon, &b, sizeof(b));
    return b;
}

static bool serial_char_available(struct device* dev)
{
    bool b = ring_buf_is_empty(&ringbuffer_pjon);
    return !b;
}

static void uart_irq_callback(struct device *dev)
{
	uart_irq_update(dev);

	if (uart_irq_rx_ready(dev)) {
        uint8_t tmp;
		while (1) {
			if (uart_fifo_read(dev, &tmp, 1) == 0) {
				break;
			}
            ring_buf_put(&ringbuffer_pjon, &tmp, sizeof(tmp));
		}
    }
}

static struct device* pin_dev = NULL;

static int serial_put_char(struct device* dev, uint8_t byte)
{
    uart_poll_out(dev,byte);
    return 1;
}

static void digitalWrite(int pin, bool state)
{
    gpio_pin_set(pin_dev, pin, state);

}
static void pinMode(int pin, int mode)
{
    pin_dev = device_get_binding(DT_GPIO_LABEL(DT_ALIAS(pjon_txe_pin), gpios));

    gpio_pin_configure(pin_dev, pin, mode);

}

static void serial_flush(struct device* dev)
{
    k_usleep(10);
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
