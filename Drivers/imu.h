#ifndef IMU_H
#define IMU_H

#include <stdint.h>

/* -------------------------------------------------------
 * IMU driver for MPU-6050
 * Interface : I2C (uses stm32_i2c driver)
 * ------------------------------------------------------- */

/* I2C address (AD0 pin = GND → 0x68) */
#define MPU6050_ADDR        0x68U

/* MPU-6050 register map (commonly used) */
#define MPU6050_REG_SMPLRT_DIV   0x19U
#define MPU6050_REG_CONFIG       0x1AU
#define MPU6050_REG_GYRO_CONFIG  0x1BU
#define MPU6050_REG_ACCEL_CONFIG 0x1CU
#define MPU6050_REG_ACCEL_XOUT_H 0x3BU
#define MPU6050_REG_TEMP_OUT_H   0x41U
#define MPU6050_REG_GYRO_XOUT_H  0x43U
#define MPU6050_REG_PWR_MGMT_1   0x6BU
#define MPU6050_REG_WHO_AM_I     0x75U

/* Gyro full-scale range */
typedef enum {
    GYRO_FS_250  = 0x00,
    GYRO_FS_500  = 0x08,
    GYRO_FS_1000 = 0x10,
    GYRO_FS_2000 = 0x18
} Gyro_FS_t;

/* Accel full-scale range */
typedef enum {
    ACCEL_FS_2G  = 0x00,
    ACCEL_FS_4G  = 0x08,
    ACCEL_FS_8G  = 0x10,
    ACCEL_FS_16G = 0x18
} Accel_FS_t;

/* Sensor data structure (raw 16-bit values) */
typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t temp_raw;   /* Temperature raw (use formula to convert) */
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} IMU_Data_t;

/* ---- Public API ---- */

/**
 * @brief  Initialise MPU-6050.
 *         Wakes the device, sets ±2g accel, ±250°/s gyro.
 * @retval 0 on success, non-zero on I2C error or wrong WHO_AM_I.
 */
uint8_t IMU_Init(void);

/**
 * @brief  Read all accelerometer and gyroscope raw values.
 * @param  data  Pointer to IMU_Data_t structure to fill.
 * @retval 0 on success, non-zero on I2C error.
 */
uint8_t IMU_ReadData(IMU_Data_t *data);

/**
 * @brief  Convert raw temperature to degrees Celsius.
 *         Formula: Temp_degC = raw / 340.0 + 36.53
 */
float   IMU_GetTempCelsius(int16_t temp_raw);

#endif /* IMU_H */
