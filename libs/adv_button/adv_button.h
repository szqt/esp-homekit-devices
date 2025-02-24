/*
 * Advanced Button Manager
 *
 * Copyright 2019-2022 José Antonio Jiménez Campos (@RavenSystem)
 *
 */

/*
 * Based on Button library by Maxim Kulkin (@MaximKulkin), licensed under the MIT License.
 * https://github.com/maximkulkin/esp-homekit-demo/blob/master/examples/button/button.c
 */

#ifndef __ADVANCED_BUTTON__
#define __ADVANCED_BUTTON__

#define INVSINGLEPRESS_TYPE             (0)
#define SINGLEPRESS_TYPE                (1)
#define DOUBLEPRESS_TYPE                (2)
#define LONGPRESS_TYPE                  (3)
#define VERYLONGPRESS_TYPE              (4)
#define HOLDPRESS_TYPE                  (5)

#define ADV_BUTTON_NORMAL_MODE          (0)
#define ADV_BUTTON_PULSE_MODE           (1)

#define ADV_BUTTON_CONTINUOS_MODE_OFF   (0)
#define ADV_BUTTON_CONTINUOS_MODE_ON    (1)


typedef void (*button_callback_fn)(uint16_t gpio, void *args, uint8_t param);

void adv_button_init(const uint16_t new_delay_ms, const bool continuos_mode);
int adv_button_create(const uint16_t gpio, const bool inverted, const uint8_t mode, const uint8_t mcp_bus, const uint8_t max_eval);
//int adv_button_destroy(const int gpio);
void adv_button_set_disable_time();
int adv_button_read_by_gpio(const uint16_t gpio);

/*
 * Button callback types:
 * 0 Single press (inverted to 1)
 * 1 Single press
 * 2 Double press
 * 3 Long press
 * 4 Very long press
 * 5 Hold press
 */
int adv_button_register_callback_fn(const uint16_t gpio, const button_callback_fn callback, const uint8_t button_callback_type, void *args, const uint8_t param);

#endif // __ADVANCED_BUTTON__
