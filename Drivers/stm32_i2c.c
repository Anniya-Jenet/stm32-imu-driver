#include "stm32_i2c.h"
#include "stm32f4xx.h"

/* -------------------------------------------------------
 * Bare-metal I2C1 driver – STM32F4xx
 *
 * Pin mapping (AF4):
 *   PB6 → I2C1_SCL
 *   PB7 → I2C1_SDA
 *
 * Clock: APB1 = 16 MHz (HSI), SCL = 100 kHz
 * ------------------------------------------------------- */

#define I2C_TIMEOUT_VAL  10000U

/* ---- private helpers ---- */

static uint8_t wait_flag_set(uint32_t flag, uint32_t timeout)
{
    while (!(I2C1->SR1 & flag))
    {
        if (--timeout == 0) return I2C_TIMEOUT;
    }
    return I2C_OK;
}

static uint8_t wait_flag_clear(uint32_t flag, uint32_t timeout)
{
    while (I2C1->SR1 & flag)
    {
        if (--timeout == 0) return I2C_TIMEOUT;
    }
    return I2C_OK;
}

/* ---- public API ---- */

void I2C_Init(void)
{
    /* 1. Enable clocks: GPIOB and I2C1 */
    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR  |= RCC_APB1ENR_I2C1EN;

    /* 2. Configure PB6, PB7 as AF4 (I2C1), open-drain, pull-up */
    /* Mode: Alternate function (10) */
    GPIOB->MODER  &= ~((3U << (6*2)) | (3U << (7*2)));
    GPIOB->MODER  |=  ((2U << (6*2)) | (2U << (7*2)));
    /* Output type: open-drain */
    GPIOB->OTYPER |=  (1U << 6) | (1U << 7);
    /* Speed: high */
    GPIOB->OSPEEDR|=  ((3U << (6*2)) | (3U << (7*2)));
    /* Pull-up */
    GPIOB->PUPDR  &= ~((3U << (6*2)) | (3U << (7*2)));
    GPIOB->PUPDR  |=  ((1U << (6*2)) | (1U << (7*2)));
    /* AF4 for PB6 and PB7 */
    GPIOB->AFR[0] &= ~((0xFU << (6*4)) | (0xFU << (7*4)));
    GPIOB->AFR[0] |=  ((4U  << (6*4)) | (4U  << (7*4)));

    /* 3. Reset I2C1 */
    I2C1->CR1 |=  I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    /* 4. Configure I2C1 for 100 kHz, APB1 = 16 MHz */
    I2C1->CR2   = 16U;          /* FREQ = 16 MHz              */
    I2C1->CCR   = 80U;          /* CCR  = 16MHz/(2*100kHz)    */
    I2C1->TRISE = 17U;          /* TRISE= (1000ns/62.5ns) + 1 */

    /* 5. Enable I2C1 */
    I2C1->CR1 |= I2C_CR1_PE;
}

uint8_t I2C_Write(uint8_t dev_addr, uint8_t reg,
                  const uint8_t *buf, uint8_t len)
{
    uint8_t ret;

    /* Generate START */
    I2C1->CR1 |= I2C_CR1_START;
    ret = wait_flag_set(I2C_SR1_SB, I2C_TIMEOUT_VAL);
    if (ret) return ret;

    /* Send device address (write) */
    I2C1->DR = (dev_addr << 1) & 0xFEU;
    ret = wait_flag_set(I2C_SR1_ADDR, I2C_TIMEOUT_VAL);
    if (ret) return ret;
    (void)I2C1->SR2; /* clear ADDR by reading SR2 */

    /* Send register address */
    I2C1->DR = reg;
    ret = wait_flag_set(I2C_SR1_TXE, I2C_TIMEOUT_VAL);
    if (ret) return ret;

    /* Send data bytes */
    for (uint8_t i = 0; i < len; i++)
    {
        I2C1->DR = buf[i];
        ret = wait_flag_set(I2C_SR1_TXE, I2C_TIMEOUT_VAL);
        if (ret) return ret;
    }

    /* Wait for BTF then STOP */
    ret = wait_flag_set(I2C_SR1_BTF, I2C_TIMEOUT_VAL);
    if (ret) return ret;
    I2C1->CR1 |= I2C_CR1_STOP;

    return I2C_OK;
}

uint8_t I2C_Read(uint8_t dev_addr, uint8_t reg,
                 uint8_t *buf, uint8_t len)
{
    uint8_t ret;

    /* --- Write phase: send register address --- */
    I2C1->CR1 |= I2C_CR1_START;
    ret = wait_flag_set(I2C_SR1_SB, I2C_TIMEOUT_VAL);
    if (ret) return ret;

    I2C1->DR = (dev_addr << 1) & 0xFEU;
    ret = wait_flag_set(I2C_SR1_ADDR, I2C_TIMEOUT_VAL);
    if (ret) return ret;
    (void)I2C1->SR2;

    I2C1->DR = reg;
    ret = wait_flag_set(I2C_SR1_BTF, I2C_TIMEOUT_VAL);
    if (ret) return ret;

    /* --- Read phase: repeated START --- */
    I2C1->CR1 |= I2C_CR1_START;
    ret = wait_flag_set(I2C_SR1_SB, I2C_TIMEOUT_VAL);
    if (ret) return ret;

    I2C1->DR = (dev_addr << 1) | 0x01U; /* read */
    ret = wait_flag_set(I2C_SR1_ADDR, I2C_TIMEOUT_VAL);
    if (ret) return ret;

    if (len == 1)
    {
        /* Disable ACK before clearing ADDR */
        I2C1->CR1 &= ~I2C_CR1_ACK;
        (void)I2C1->SR2;
        I2C1->CR1 |= I2C_CR1_STOP;
        ret = wait_flag_set(I2C_SR1_RXNE, I2C_TIMEOUT_VAL);
        if (ret) return ret;
        buf[0] = (uint8_t)I2C1->DR;
    }
    else
    {
        I2C1->CR1 |= I2C_CR1_ACK;
        (void)I2C1->SR2;

        for (uint8_t i = 0; i < len; i++)
        {
            if (i == len - 1)
            {
                I2C1->CR1 &= ~I2C_CR1_ACK;
                I2C1->CR1 |=  I2C_CR1_STOP;
            }
            ret = wait_flag_set(I2C_SR1_RXNE, I2C_TIMEOUT_VAL);
            if (ret) return ret;
            buf[i] = (uint8_t)I2C1->DR;
        }
    }

    return I2C_OK;
}
