# Pioneer Avionics — Flight State Machine Reference

The flight logic lives in `StatesManager::onUserLogic()`
(`user/usercode/usercode.cpp`), called once per 100ms tick from
`Loops::Run()`. The state machine has 9 active states (two additional
states, `CRUISING` and `DEPLOYMENT`, are declared for a future two-phase
refinement but are not currently wired into the loop's dispatch switch).

## State reference

| State | Entry condition | Exit condition | Action taken in this state |
|---|---|---|---|
| **BOOT** | Power-on | Altitude reading stable (±0.2 m) for 20 consecutive ticks (2 s) | Locks in `ground_altitude` as the zero reference |
| **IDLE** | From BOOT | `IsArmCommanded()` returns true | Applies slow drift correction to `ground_altitude` while stationary (accel magnitude between 0.9–1.1 g) |
| **ARMED** | From IDLE | Sustained upward accel (`accel_z > LAUNCH_ACCEL_G`) + 2 m altitude gain, debounced for 5 ticks (500 ms) | Waits for confirmed liftoff |
| **LAUNCH** | From ARMED | Altitude confirmed increasing vs. last reading | Transitional — confirms real ascent, not a spike |
| **ASCENT** | From LAUNCH | Altitude drops `APOGEE_DROP_METERS` below the tracked peak (`max_altitude`) | Continuously updates `max_altitude` |
| **APOGEE** | From ASCENT | Immediate (single tick) | Fires pyro channel 1 (drogue) exactly once, then moves to DESCENT |
| **DESCENT** | From APOGEE | Altitude stable (< `LAND_ALT_DELTA_METERS`) for 50 ticks (5 s) | Fires pyro channel 2 (main) once altitude falls below `MAIN_DEPLOY_ALT_METERS` and above 20 m AGL |
| **LANDED** | From DESCENT | Terminal | Switches buzzer to continuous recovery beacon |
| **FAILSAFE** | From any state, on critical fault or watchdog timeout | Terminal | Immediately fires **both** pyro channels |

## Why apogee is detected this way

Apogee detection uses **peak-tracking** (fire once altitude drops a fixed
amount below the highest altitude seen so far) rather than a velocity
zero-crossing or a pure acceleration threshold. This is a deliberate choice
— it's more robust to sensor noise near the top of the flight, where
vertical velocity is naturally near zero for an extended period and easy to
misread.

## Why deployment is two-stage

Pyro channel 1 (drogue) fires at apogee to slow the rocket without inducing
excessive drift. Pyro channel 2 (main) only fires once the rocket has
descended into a defined altitude window (`MAIN_DEPLOY_ALT_METERS` to 20 m
AGL) — this is standard dual-deploy practice: a small drogue keeps descent
controlled at altitude where wind carries the rocket far, and a larger main
chute opens close to the ground to minimize horizontal drift distance.

## Known gaps (as of this snapshot)

- `LAUNCH_ACCEL_G`, `APOGEE_DROP_METERS`, and `MAIN_DEPLOY_ALT_METERS` are
  all placeholder values (`0.0f`) — the detection *algorithms* are
  implemented, but the thresholds have not yet been tuned against real
  motor/airframe data (see `simulations/` for the modeled flight profile
  these should eventually be derived from).
- `IsArmCommanded()` in `SensorManager` currently always returns `true` —
  there is no physical arming switch or ground-command input wired in yet.
- `CRUISING` and `DEPLOYMENT` states exist in the `FlightState` enum and
  have handler stubs, but are not reachable from the current dispatch
  switch in `Loops::Run()`.

## Failsafe trigger conditions

FAILSAFE can be entered from any state when either:
1. `sensors.IsHealthy()` returns false (a required subsystem — BMP280,
   magnetometer, IMU, or SD card — failed its POST), or
2. The watchdog (`Watchdogs::IsCheckFailed()`) detects the 100ms state-logic
   tick has not run within its configured timeout.

Once in FAILSAFE, both pyro channels are fired immediately and
unconditionally, on the principle that an unrecovered rocket is a worse
outcome than an early or unnecessary deployment.
