# Pioneer Avionics — Failure Modes & Safety Behavior

This document describes how the firmware responds when a subsystem fails,
so the behavior can be reasoned about (and questioned) without reading the
full source.

## 1. Startup failure (POST)

`SensorManager::InitAll()` runs a Power-On Self-Test on every subsystem and
prints a pass/fail line for each. Overall system health is gated on four
subsystems:

| Subsystem | Required for `IsHealthy()`? |
|---|---|
| BMP280 (altitude) | Yes |
| QMC5883L (magnetometer) | Yes |
| MPU9250 (IMU) | Yes |
| SD card | Yes |
| GPS (L89HA) | No |
| NRF24 (radio) | No |

If any required subsystem fails POST, `_systemOk` is set false and the
system prints "SYSTEM DEGRADED" instead of "SYSTEM READY" — but the firmware
does **not** halt. `StatesManager::onUserLogic()` checks `IsHealthy()` on
every tick and forces `FAILSAFE` immediately if it's false, so a
degraded-at-boot system fails safe rather than attempting flight logic with
missing sensors.

GPS and radio are excluded from the health gate deliberately: losing a GPS
fix or radio link should not prevent the rocket from flying and recovering
correctly, since neither is used in the deployment decision logic.

## 2. In-flight sensor failure

There is currently no continuous, per-tick health re-check of individual
sensors during flight — `IsHealthy()` reflects the POST result, not a live
status. A sensor that passes POST but fails mid-flight (e.g., an I2C bus
glitch) is not automatically detected as such; the state machine would
continue operating on whatever last value was read. This is a known gap,
worth flagging explicitly if asked about it.

## 3. Watchdog timeout

The state-logic tick pets the watchdog once per 100ms, but only while the
current state is **not** FAILSAFE. If the tick stalls (e.g., a blocking call
in a sensor driver) beyond the configured timeout, `Watchdogs::IsCheckFailed()`
returns true and the state machine is forced to FAILSAFE on the next
successful tick.

## 4. FAILSAFE behavior

FAILSAFE is the terminal safety state. On entry, both pyro channels are
fired immediately and unconditionally — this is a deliberate dead-man's
-switch design: if the system can no longer be trusted to know its own
flight phase, the safer outcome is to deploy full recovery rather than risk
an unrecovered rocket under power or at high velocity.

There is no automatic recovery out of FAILSAFE; it is a terminal state for
the remainder of the flight.

## 5. Pyro channel safety

Both pyro GPIO pins (20 and 21) are explicitly driven LOW during the very
first steps of `InitAll()`, before any other subsystem is touched, so that
a partial or interrupted boot sequence cannot leave a pin floating into a
firing state. The only code paths that drive them HIGH are the APOGEE,
DESCENT, and FAILSAFE state handlers via `ActivatePyroChannel()`.

## 6. Known limitations to disclose if asked

- No live/continuous sensor health monitoring during flight (POST-only).
- `IsArmCommanded()` is currently hardcoded `true` — no physical arm switch
  or ground-command interlock is implemented yet.
- No redundant/secondary apogee or landing detection path — a single
  algorithm drives each transition, with no independent cross-check.
- Flight-critical thresholds (launch accel, apogee drop, main deploy
  altitude) are unset placeholders as of this snapshot; see
  `STATE_MACHINE.md`.
