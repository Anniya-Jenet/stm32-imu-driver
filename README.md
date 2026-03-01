# STM32 IMU Driver

Bare-metal I2C driver for the **MPU-6050** IMU on **STM32F411xE** (Blackpill).  
ONly using C and registers, NO HAL or CubeEX

---

## Project Structure

```
stm32-imu-driver/
├── Core/
│   └── main.c              ← Application entry point
├── Drivers/
│   ├── stm32_i2c.c/h       ← Bare-metal I2C1 driver (PB6/PB7)
│   ├── imu.c/h             ← MPU-6050 driver
├── RTOS/                   ← (optional) FreeRTOS files go here
├── startup_stm32.s         ← Vector table + Reset_Handler
├── linker.ld               ← Memory layout for STM32F411xE
├── Makefile
└── README.md
```

---

## Prerequisites

| Tool | Purpose |
|------|---------|
| `arm-none-eabi-gcc` | Cross-compiler |
| CMSIS device headers | `stm32f4xx.h` and friends |
| `make` | Build system |
| OpenOCD *(optional)* | Flashing to real hardware |

### Download CMSIS Headers

```bash
git clone https://github.com/STMicroelectronics/cmsis-device-f4
```

Place the cloned folder in the project root so the Makefile can find it:

```
stm32-imu-driver/
└── cmsis-device-f4/
    └── Include/
        └── stm32f4xx.h   ✅
```

---

## Build

```bash
make
```

Output files appear in `build/`:
- `stm32-imu-driver.elf` — debuggable ELF
- `stm32-imu-driver.bin` — raw binary for flashing
- `stm32-imu-driver.hex` — Intel HEX format
- `stm32-imu-driver.map` — linker map

### Clean

```bash
make clean
```

---

## Hardware 

| STM32F411 Pin | MPU-6050 Pin |
|--------------|-------------|
| PB6 (I2C1_SCL) | SCL |
| PB7 (I2C1_SDA) | SDA |
| 3.3V | VCC |
| GND | GND, AD0 |

AD0 tied to GND → I2C address = **0x68**

### Flash

Uncomment the `flash` target in the Makefile, then:

```bash
make flash
```

---

## How It Works

1. **`I2C_Init()`** — Configures PB6/PB7 as AF4, sets I2C1 to 100 kHz
2. **`IMU_Init()`** — Wakes MPU-6050, sets ±2g / ±250°/s ranges
3. **`IMU_ReadData()`** — Burst-reads 14 bytes (accel + temp + gyro)
4. `main()` polls every 100 ms in a loop

---

## Adding FreeRTOS (optional- to be added in the final copy)

1. Download [FreeRTOS](https://www.freertos.org/a00104.html)
2. Copy `FreeRTOS/Source/` into the `RTOS/` folder
3. Add a `FreeRTOSConfig.h` tuned for STM32F411
4. Add source files to the `Makefile`
5. Wrap `IMU_ReadData()` in an RTOS task

---

## License

MIT — free to use, modify, and distribute.
