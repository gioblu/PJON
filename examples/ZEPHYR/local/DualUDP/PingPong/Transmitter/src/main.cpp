#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <net/net_if.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#if DT_PHA_HAS_CELL(LED0_NODE, gpios, flags)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)
#endif
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0	""
#define PIN	0
#endif

#ifndef FLAGS
#define FLAGS	0
#endif

#include "PJONDualUDP.h"

PJONDualUDP bus(CONFIG_PJON_DEVICE_ID);

static int cnt = 0;
static struct net_mgmt_event_callback mgmt_cb;

static void pjon_receive_cb(uint8_t* payload, uint16_t length, const PJON_Packet_Info& packet_info)
{
    if (payload[0] == 'P') cnt++;
}


void pjon_thread(void)
{
    bus.set_receiver(pjon_receive_cb);

    bus.begin();
    printk("PJON Bus started!\n");

    bus.send_repeatedly(44, "P", 1, 20000);

    while (true) {
        bus.update();
        bus.receive();
        k_usleep(10);
    }
}

K_THREAD_DEFINE(router_thread, 1024, pjon_thread, NULL, NULL, NULL, 12, 0, 0);

/*
 * print a IP addresse assigned by DHCP
 */
static void handler(struct net_mgmt_event_callback* cb, u32_t mgmt_event, struct net_if* iface)
{
    printk("dhcp handler called!");

    if (mgmt_event != NET_EVENT_IPV4_ADDR_ADD) {
        return;
    }

    for (int i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
        char buf[NET_IPV4_ADDR_LEN];

        if (iface->config.ip.ipv4->unicast[i].addr_type != NET_ADDR_DHCP) {
            continue;
        }
        printk("Your address: %s",
            net_addr_ntop(AF_INET,
                &iface->config.ip.ipv4->unicast[i].address.in_addr, buf, sizeof(buf)));
    }
}

void main(void)
{
    printk("Hello World from Transmitter: %s!\n", CONFIG_BOARD);

	struct device *dev;
	bool led_is_on = true;
	int ret;

	dev = device_get_binding(LED0);
	if (dev == NULL) {
		return;
	}

	ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		return;
	}

	// starting dhcpv4 on iface and add handler to print the IP
    struct net_if* iface;
    net_mgmt_init_event_callback(&mgmt_cb, handler, NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&mgmt_cb);
    iface = net_if_get_default();
    net_dhcpv4_start(iface);

	while (1) {
		gpio_pin_set(dev, PIN, (int)led_is_on);
		led_is_on = !led_is_on;
		k_msleep(SLEEP_TIME_MS);
		printk("PONG/s: %d\n", cnt);
		cnt = 0;
	}
}
