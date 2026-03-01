#include <stdint.h>
#include "stm32f4xx.h"
#include "../Drivers/stm32_i2c.h"
#include "../Drivers/imu.h"

/* -------------------------------------------------------
 * STM32 IMU Driver Demo
 * Target : STM32F411 (Blackpill)
 * Sensor : MPU-6050 (I2C)
 * No HAL, No CubeMX – bare-metal only
 * ------------------------------------------------------- */

/* Simple busy-wait delay (not accurate, just for demo) */
static void delay_ms(volatile uint32_t ms)
{
    while (ms--)
    {
        volatile uint32_t i = 16000; /* ~1ms @ 16MHz */
        while (i--);
    }
}

int main(void)
{
    /* 1. Initialize I2C1 peripheral */
    I2C_Init();

    /* 2. Initialize IMU (MPU-6050) */
    IMU_Init();

    IMU_Data_t data;

    while (1)
    {
        /* 3. Read accelerometer + gyroscope */
        IMU_ReadData(&data);

        /*
         * In a real system you would:
         *   - Send data over UART
         *   - Run a sensor fusion algorithm (e.g. Madgwick)
         *   - Feed into an RTOS task
         *
         * Here we just loop – set a breakpoint on delay_ms
         * and inspect `data` in the debugger.
         */

        delay_ms(100);
    }

    /* Should never reach here */
    return 0;
}
