#ifndef STM32_I2C_H
#define STM32_I2C_H

#include <stdint.h>

/* -------------------------------------------------------
 * Bare-metal I2C driver for STM32F4xx
 * Uses I2C1: PB6 = SCL, PB7 = SDA
 * Standard mode: 100 kHz
 * ------------------------------------------------------- */

/* Return codes */
#define I2C_OK      0
#define I2C_ERROR   1
#define I2C_TIMEOUT 2

/* Initialise I2C1 peripheral and GPIO */
void    I2C_Init(void);

/* Write `len` bytes from `buf` to device at `dev_addr`, starting at `reg` */
uint8_t I2C_Write(uint8_t dev_addr, uint8_t reg, const uint8_t *buf, uint8_t len);

/* Read `len` bytes into `buf` from device at `dev_addr`, starting at `reg` */
uint8_t I2C_Read(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len);

#endif /* STM32_I2C_H */
