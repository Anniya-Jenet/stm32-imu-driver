#include "imu.h"
#include "stm32_i2c.h"

/* -------------------------------------------------------
 * MPU-6050 IMU driver implementation
 * ------------------------------------------------------- */

uint8_t IMU_Init(void)
{
    uint8_t who_am_i = 0;
    uint8_t data;
    uint8_t ret;

    /* 1. Verify device identity */
    ret = I2C_Read(MPU6050_ADDR, MPU6050_REG_WHO_AM_I, &who_am_i, 1);
    if (ret != 0) return ret;
    if (who_am_i != 0x68U) return 0xFF; /* Wrong device */

    /* 2. Wake device (clear SLEEP bit in PWR_MGMT_1) */
    data = 0x00U;
    ret = I2C_Write(MPU6050_ADDR, MPU6050_REG_PWR_MGMT_1, &data, 1);
    if (ret != 0) return ret;

    /* 3. Sample rate divider = 7 → 1 kHz / (1+7) = 125 Hz */
    data = 0x07U;
    ret = I2C_Write(MPU6050_ADDR, MPU6050_REG_SMPLRT_DIV, &data, 1);
    if (ret != 0) return ret;

    /* 4. DLPF config = 1 (184 Hz bandwidth) */
    data = 0x01U;
    ret = I2C_Write(MPU6050_ADDR, MPU6050_REG_CONFIG, &data, 1);
    if (ret != 0) return ret;

    /* 5. Gyroscope full-scale: ±250 °/s */
    data = GYRO_FS_250;
    ret = I2C_Write(MPU6050_ADDR, MPU6050_REG_GYRO_CONFIG, &data, 1);
    if (ret != 0) return ret;

    /* 6. Accelerometer full-scale: ±2 g */
    data = ACCEL_FS_2G;
    ret = I2C_Write(MPU6050_ADDR, MPU6050_REG_ACCEL_CONFIG, &data, 1);
    if (ret != 0) return ret;

    return 0; /* success */
}

uint8_t IMU_ReadData(IMU_Data_t *data)
{
    /*
     * Burst-read 14 bytes starting at ACCEL_XOUT_H:
     *   [0-1]  Accel X
     *   [2-3]  Accel Y
     *   [4-5]  Accel Z
     *   [6-7]  Temperature
     *   [8-9]  Gyro X
     *   [10-11] Gyro Y
     *   [12-13] Gyro Z
     */
    uint8_t raw[14];
    uint8_t ret;

    ret = I2C_Read(MPU6050_ADDR, MPU6050_REG_ACCEL_XOUT_H, raw, 14);
    if (ret != 0) return ret;

    data->accel_x  = (int16_t)((raw[0]  << 8) | raw[1]);
    data->accel_y  = (int16_t)((raw[2]  << 8) | raw[3]);
    data->accel_z  = (int16_t)((raw[4]  << 8) | raw[5]);
    data->temp_raw = (int16_t)((raw[6]  << 8) | raw[7]);
    data->gyro_x   = (int16_t)((raw[8]  << 8) | raw[9]);
    data->gyro_y   = (int16_t)((raw[10] << 8) | raw[11]);
    data->gyro_z   = (int16_t)((raw[12] << 8) | raw[13]);

    return 0;
}

float IMU_GetTempCelsius(int16_t temp_raw)
{
    return ((float)temp_raw / 340.0f) + 36.53f;
}
