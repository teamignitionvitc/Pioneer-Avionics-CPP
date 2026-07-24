# Pioneer Avionics — Hardware & Wiring Reference

**MCU:** Raspberry Pi Pico (RP2040)

This reference consolidates the bus/pin assignments that are currently
scattered across `SensorManager`'s constructor and `InitAll()`. Verify
against the actual board wiring before flight — this reflects what the
firmware assumes, not an independently verified schematic.

## Bus assignments

| Peripheral | Bus | Notes |
|---|---|---|
| BMP280 (barometer/altitude) | I2C1 | Shares bus with magnetometer and IMU |
| QMC5883L (magnetometer) | I2C1 | Shares bus with BMP280 and IMU |
| MPU9250 (IMU: accel + gyro) | I2C1 | Shares bus with BMP280 and magnetometer |
| L89HA (GNSS/GPS) | UART0 | |
| SD card | SPI1 | Shares bus with NRF24 |
| NRF24 (radio telemetry) | SPI1 | Shares bus with SD card |

## GPIO pin map

| Function | GPIO | Notes |
|---|---|---|
| Status LED | 25 | Onboard LED; blinks during boot/POST sequence |
| Pyro channel 1 (drogue) | 20 | Initialized LOW (safe) at boot; driven HIGH on `ActivatePyroChannel(1)` |
| Pyro channel 2 (main) | 21 | Initialized LOW (safe) at boot; driven HIGH on `ActivatePyroChannel(2)` |
| Buzzer | 6 | PWM tone output, used for POST feedback and recovery beacon |

## Power-on safety sequencing

At boot, `SensorManager::InitAll()` explicitly drives both pyro GPIO pins
LOW **before** any other initialization occurs, so a mid-boot glitch cannot
accidentally fire a charge. Only `ActivatePyroChannel()`, called exclusively
from the APOGEE, DESCENT, or FAILSAFE state handlers, ever drives them HIGH.

## Boot LED sequence (for field debugging)

`main.cpp` performs a visible LED sequence before handing off to the Ark
kernel — useful for confirming the board powered on and reached firmware
entry even without a serial connection attached:

1. LED ON for ~5 s (power stabilization wait)
2. LED OFF
3. LED ON for ~100 ms
4. LED OFF
5. Hands off to `Ark::Init()` / `Ark::Start()`

## Telemetry / logging outputs

- **Serial (USB + UART):** enabled via `pico_enable_stdio_usb` /
  `pico_enable_stdio_uart` in `CMakeLists.txt`; view with `screen`,
  `minicom`, or an editor's serial monitor.
- **Radio (NRF24):** transmits the same packet string built by
  `SensorManager::BuildPacket()`.
- **SD card:** logs to `flight.csv` with header row
  `TS,ALT,TMP,HDG,AX,AY,AZ,GX,GY,GZ,MX,MY,MZ`, written only while
  `IsHealthy()` is true.
