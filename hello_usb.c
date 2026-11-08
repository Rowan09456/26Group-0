/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

/*
int main() {
    stdio_init_all();
     if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    int i = 0;
    while (true) {
        printf("Hello, world [%d]!\n", i++);
        sleep_ms(1000);
         cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN,!cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN) );
    }
}
    */
