/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Johan Kanflo (github.com/kanflo)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
/**
 * I2C driver for ESP8266 written for use with esp-open-rtos
 * Based on https://en.wikipedia.org/wiki/I²C#Example_of_bit-banging_the_I.C2.B2C_Master_protocol
 */

#ifndef __ADV_I2C_H__
#define __ADV_I2C_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef ESP_PLATFORM

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#else

#include <errno.h>
#include <FreeRTOS.h>
#include <task.h>

#endif

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef ADV_I2C_SEMAPHORE_TIMEOUT_MS
#define ADV_I2C_SEMAPHORE_TIMEOUT_MS                (2000)
#endif

#define ADV_I2C_SEMAPHORE_TIMEOUT                   (ADV_I2C_SEMAPHORE_TIMEOUT_MS / portTICK_PERIOD_MS)

/**
 * Define i2c bus max number
 */
#ifndef I2C_MAX_BUS
    #define I2C_MAX_BUS 2
#endif

/**
 * Init bitbanging I2C driver on given pins
 * @param bus Bus i2c selection
 * @param scl_pin SCL pin for I2C
 * @param sda_pin SDA pin for I2C
 * @param freq frequency of bus in hertz
 * @param scl_pin_pullup Internal pull-up resistor for SCL pin
 * @param sda_pin_pullup Internal pull-up resistor for SDA pin
 * @return Non-zero if error occured
 */
int adv_i2c_init_hz(uint8_t bus, uint8_t scl_pin, uint8_t sda_pin, uint32_t freq, bool scl_pin_pullup, bool sda_pin_pullup);

/**
 * Issue a send operation of 'data' register adress, followed by reading 'len' bytes
 * from slave into 'buf'.
 * @param bus Bus i2c selection
 * @param slave_addr slave device address
 * @param data Pointer to register address to send if non-null
 * @param data_len Number of byte of data to send
 * @param buf Pointer to data buffer
 * @param len Number of byte of buf to read
 * @return Non-Zero if error occured
 */
int adv_i2c_slave_read(uint8_t bus, uint8_t slave_addr, const uint8_t *data, const size_t data_len, uint8_t *buf, size_t len);
int adv_i2c_slave_read_no_wait(uint8_t bus, uint8_t slave_addr, const uint8_t *data, const size_t data_len, uint8_t *buf, size_t len);

/**
 * Write 'len' bytes from 'buf' to slave at 'data' register adress .
 * @param bus Bus i2c selection
 * @param slave_addr slave device address
 * @param data Pointer to register address to send if non-null
 * @param data_len Number of byte of data to send
 * @param buf Pointer to data buffer
 * @param len Number of byte of buf to send
 * @return Non-Zero if error occured
 */
int adv_i2c_slave_write(uint8_t bus, uint8_t slave_addr, const uint8_t *data, const size_t data_len, const uint8_t *buf, size_t len);
int adv_i2c_slave_write_no_wait(uint8_t bus, uint8_t slave_addr, const uint8_t *data, const size_t data_len, const uint8_t *buf, size_t len);

#ifndef ESP_PLATFORM

/* Set this to 1 if you intend to use GPIO 16 for I2C. It is not recommended
 * and will result in degradation of performance and timing accuracy.
 */
#ifndef I2C_USE_GPIO16
    #define I2C_USE_GPIO16 0
#endif

/* Default clock strech waiting time, 250 msec. */
#define I2C_DEFAULT_CLK_STRETCH (250 / portTICK_PERIOD_MS)

/* SCL speed settings. 160 MHz sysclk frequency will result in improved
 * timing accuracy. Greater bitrates will have poorer accuracy. 1000K is the
 * maximum SCL speed at 80 MHz sysclk.
 */
typedef enum
{
  I2C_FREQ_80K = 0,
  I2C_FREQ_100K,
  I2C_FREQ_400K,
  I2C_FREQ_500K,
  I2C_FREQ_600K,
  I2C_FREQ_800K,
  I2C_FREQ_1000K,
  I2C_FREQ_1300K
} i2c_freq_t;

/// Level 0 API

/**
 * Init bitbanging I2C driver on given pins
 * @param bus Bus i2c selection
 * @param scl_pin SCL pin for I2C
 * @param sda_pin SDA pin for I2C
 * @param freq frequency of bus (ex : I2C_FREQ_400K)
 * @return Non-zero if error occured
 */
int adv_i2c_init(uint8_t bus, uint8_t scl_pin, uint8_t sda_pin, i2c_freq_t freq, bool scl_pin_pullup, bool sda_pin_pullup);

/**
 * Change bus frequency
 * @param bus Bus i2c selection
 * @param freq frequency of bus (ex : I2C_FREQ_400K)
 */
int i2c_set_frequency(uint8_t bus, i2c_freq_t freq);

/**
 * Change bus frequency
 * @param bus Bus i2c selection
 * @param freq frequency of bus in hertz
 */
int i2c_set_frequency_hz(uint8_t bus, uint32_t freq);

/**
 * Change clock stretch
 * @param bus I2C bus
 * @param clk_stretch I2C clock stretch, in ticks. I2C_DEFAULT_CLK_STRETCH by default
 */
void i2c_set_clock_stretch(uint8_t bus, TickType_t clk_stretch);

/**
 * Write a byte to I2C bus.
 * @param bus Bus i2c selection
 * @param byte Pointer to device descriptor
 * @return true if slave acked
 */
bool i2c_write(uint8_t bus, uint8_t byte);

/**
 * Read a byte from I2C bus.
 * @param bus Bus i2c selection
 * @param ack Set Ack for slave (false: Ack // true: NoAck)
 * @return byte read from slave.
 */
uint8_t i2c_read(uint8_t bus, bool ack);

/**
 * Send start or restart condition
 * @param bus Bus i2c selection
 */
void i2c_start(uint8_t bus);

/**
 * Send stop condition
 * @param bus Bus i2c selection
 * @return false if link was broken
 */
bool i2c_stop(uint8_t bus);

/**
 * get status from I2C bus.
 * @param bus Bus i2c selection
 * @return true if busy.
 */
bool i2c_status(uint8_t bus);

/// Level 1 API (Don't need functions above)


#endif  // no ESP_PLATFORM

#ifdef	__cplusplus
}
#endif

#endif /* __ADV_I2C_H__ */
