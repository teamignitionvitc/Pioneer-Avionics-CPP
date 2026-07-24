# Pioneer Avionics — Architecture Overview

**Target platform:** Raspberry Pi Pico (RP2040)
**Maintainer:** Team Ignition — Avionics Software Department

## 1. Purpose

Pioneer is the onboard flight computer firmware for Team Ignition's sounding
rocket. Its job is to read flight sensors in real time, determine what phase
of flight the rocket is in, and trigger recovery (parachute) deployment at
the correct moments — while degrading safely if any subsystem fails.

## 2. Layered design

The system is organized as four layers, each with a single responsibility:

```
main.cpp
   │
   ▼
Ark (ark.h / ark.cpp)
   │   platform lifecycle: Init() → Start() → Stop()
   ▼
Loops (kernel/loops.h / loops.cpp)
   │   the scheduler: runs sensors, telemetry, and state logic
   │   at three independent frequencies
   ▼
StatesManager (kernel/statesmanager.h/.cpp + user/usercode.cpp)
   │   the flight-phase state machine and its decision logic
   ▼
SensorManager (system/sensors.h/.cpp)
       aggregates all hardware drivers (ARK/modules/*) and exposes
       a single clean interface (GetAltitude(), GetAccelZ(), etc.)
```

Each layer only talks to the layer directly below it. `StatesManager` never
touches hardware registers directly — it asks `SensorManager` for readings
and asks it to fire pyro channels. This separation is what makes it possible
to write host-side unit tests for flight logic without real hardware
(see `tests/UNIT_TEST_TEMPLATE.md`).

## 3. The scheduler (`Loops::Run()`)

The main loop is a single `while(true)` containing three independently
timed blocks, each gated by its own "time since last run" check:

| Block | Frequency | Period | Responsibility |
|---|---|---|---|
| Sensor update | 100 Hz | 10 ms | Poll BMP280, QMC5883L, MPU9250 |
| Telemetry stream | 10 Hz | 100 ms | Print/transmit the current packet |
| State logic | 10 Hz | 100 ms | Watchdog check + run the active state handler |

Sensors are polled ten times more often than decisions are made, so the
state logic always sees fresh data without spending CPU time on a full 100Hz
control loop it doesn't need.

## 4. Watchdog integration

A software watchdog (`system/watchdogs.h/.cpp`) is petted (`Reset()`) once
per 100ms state-logic tick — but **only when the current state is not
FAILSAFE**. If a timeout is ever detected, the state machine is forced into
FAILSAFE immediately. Petting is deliberately withheld once in FAILSAFE so a
later freeze cannot silently escape the failsafe condition.

## 5. Sensor abstraction

`SensorManager` owns one instance of every driver:

- `BMP280` — barometric altitude + temperature (I2C1)
- `QMC5883L` — magnetometer / heading (I2C1)
- `MPU9250` — accelerometer + gyroscope (I2C1)
- `L89HA` — GNSS/GPS (UART0)
- `SDCard` — CSV data logger (SPI1)
- `NRF24` — radio telemetry (SPI1)
- `Buzzer` — audible POST/recovery feedback (GPIO)

It runs a Power-On Self-Test (`InitAll()`) at boot, and overall system
health (`IsHealthy()`) is gated on BMP280 + magnetometer + IMU + SD card
passing — GPS and radio are treated as non-critical, since the rocket must
still be able to sense altitude and log data even if it never gets a GPS
fix or loses the radio link.

## 6. Design intent vs. current state

The architecture (layering, scheduler, watchdog-driven failsafe, sensor
abstraction) is complete and sound. Some pieces are still placeholders as of
this snapshot — most notably the flight-detection thresholds in
`user/usercode.cpp` (see `STATE_MACHINE.md`), which are defined but not yet
tuned to real motor/airframe data.
