#ifndef GLOBAL_LED_H
#define GLOBAL_LED_H

#include "driver/gpio.h"

class Led
{
    gpio_num_t ledPin;

public:
    Led(gpio_num_t ledPin)
        : ledPin(ledPin)
    {
        gpio_config_t io_conf;
        io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_OUTPUT;
        io_conf.pin_bit_mask = (1ULL<<ledPin);
        io_conf.pull_down_en = (gpio_pulldown_t)0;
        io_conf.pull_up_en = (gpio_pullup_t)0;
        gpio_config(&io_conf);
    }

    void set()
    {
        gpio_set_level(ledPin, 1);
    }

    void reset()
    {
        gpio_set_level(ledPin, 0);
    }

    void blink(int count = 5)
    {
        for(int i=0; i<count; i++)
        {
            set();
            vTaskDelay(500);
            reset();
            vTaskDelay(500);
        }
    }
};

#endif // GLOBAL_LED_H
