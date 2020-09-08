#include "PJON_ZEPHYR_Interface.h"
#include <sys/ring_buffer.h>

#include <drivers/gpio.h>
#include <drivers/uart.h>
#include <logging/log.h>
#include <devicetree.h>

#include <stdint.h>
#include <map>
#include <string>

static struct device* pin_dev = NULL;;
#include "logging/log.h"
LOG_MODULE_REGISTER(PJON_SMP, LOG_LEVEL_INF);


static std::map<struct device*, struct ring_buf*> ringbuffers;


static struct ring_buf* _get_ring_buf(struct device* dev)
{
    auto search = ringbuffers.find(dev);
    struct ring_buf* tmp = nullptr;
    if ( search != ringbuffers.end() ) {
         tmp = search->second;
    }
    return tmp;
}

struct device* serial_open(const char* dt_label)
{
    struct device* uart = device_get_binding(dt_label); ///@todo multiple uarts for this layer --SA
    struct uart_config cfg;
    uart_config_get(uart,&cfg);
    cfg.data_bits = UART_CFG_DATA_BITS_8;
    cfg.stop_bits = UART_CFG_STOP_BITS_1;
    uart_configure(uart,&cfg);
    ///todo: how to do this in PJON interface
    uart_irq_callback_set(uart, uart_irq_callback);
    uart_irq_rx_enable(uart);

    uint8_t* buf = (uint8_t*) calloc(1,CONFIG_PJON_MTU);
    struct ring_buf* r = (struct ring_buf*) calloc(1, sizeof(struct ring_buf));
    r->size = CONFIG_PJON_MTU,
    r->buf.buf8 = buf;

    auto ret = ringbuffers.insert(std::make_pair(uart, r));

    LOG_INF("Size: %d", ringbuffers.size());

    if (ret.second != true) {
        uart = nullptr;
    }

    LOG_INF("UART: %p", uart);
    return uart;
}
void print_ringbuffer()
{
    LOG_INF("Ringbuffer: %p, size: %d",&ringbuffers, ringbuffers.size());
}

void serial_close(const char* dt_label)
{
    LOG_INF("close");
    // free((ringbuffers.at(dt_label))->buf.buf8);
}

int serial_get_char(struct device* dev)
{
    size_t  ret;
    uint8_t b;
    struct ring_buf* r = _get_ring_buf(dev);
    if(r) {
        ret = ring_buf_get(r, &b, sizeof(b));
    }
    return b;
}

bool serial_char_available(struct device* dev)
{
    struct ring_buf* r = _get_ring_buf(dev);
    bool b = false;
    if (r) {
         b = !ring_buf_is_empty(r);
    }
    return b;
}

///todo: implement this for different uarts --SA
void uart_irq_callback(struct device *dev)
{
	uart_irq_update(dev);

	if (uart_irq_rx_ready(dev)) {
        uint8_t tmp;
		while (1) {
			if (uart_fifo_read(dev, &tmp, 1) == 0) {
				break;
			}
			struct ring_buf* r = _get_ring_buf(dev);

			if (r) {
                ring_buf_put(r, &tmp, sizeof(tmp));
    		}
		}
    }
}

int serial_put_char(struct device* dev, uint8_t b)
{
    uart_poll_out(dev,b);
    return 1;
}

void digitalWrite(int pin, bool state)
{
    gpio_pin_set(pin_dev, pin, state);

}
void pinMode(int pin, int mode)
{
    pin_dev = device_get_binding(DT_GPIO_LABEL(DT_ALIAS(pjon_txe_pin), gpios));

    gpio_pin_configure(pin_dev, pin, mode);

}

void serial_flush(struct device* dev)
{
    k_usleep(1000);
}
