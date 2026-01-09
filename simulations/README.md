# Pioneer Flight Simulations

<div align="center">
  <img src="https://teamignition.space/logo/head-logo.png" alt="Team Ignition" width="120"/>
  <h3>Team Ignition VIT Chennai</h3>
  <p><strong>RocketPy Flight Dynamics Simulations for PIONEER</strong></p>
  
  [![Python](https://img.shields.io/badge/Python-3.8+-3776ab?logo=python&logoColor=white)](https://python.org)
  [![RocketPy](https://img.shields.io/badge/RocketPy-1.0+-ff6b35)](https://rocketpy.org)
  [![Jupyter](https://img.shields.io/badge/Jupyter-Notebook-f37626?logo=jupyter)](https://jupyter.org)
</div>

---

## 📋 Overview

This directory contains **6-DOF flight trajectory simulations** for the **PIONEER** sounding rocket using [RocketPy](https://rocketpy.org), an open-source rocket flight simulation library. These simulations provide independent verification alongside OpenRocket and RASAero II analyses.

### Key Capabilities

- Full 6-DOF flight dynamics with atmospheric modeling
- Custom motor characterization from static fire data
- Variable drag coefficients (power-on/off phases)
- Monte Carlo dispersion analysis
- KML export for trajectory visualization

### Simulated Performance Summary

| Parameter | Value | Notes |
|-----------|-------|-------|
| **Apogee** | ~1,025 m AGL | At nominal conditions |
| **Max Velocity** | ~185 m/s | Mach 0.54 at burnout |
| **Stability (Rail Exit)** | 1.95 cal | Target: ≥1.5 cal |
| **Motor** | Inferno K1120 | Custom BATES, 2.8 kN·s |
| **Burn Time** | 2.5 s | OpenMotor validated |
| **Drift Distance** | ~343 m | 5 m/s crosswind |

---

## 📁 Directory Structure

```
simulations/
├── README.md                       # This documentation
├── notebooks/
│   └── pioneer_simulation.ipynb    # Main simulation notebook (Jupyter)
├── data/
│   ├── motor/
│   │   └── inferno_k1120.eng       # Thrust curve (RASP format)
│   └── aerodynamics/
│       ├── power_on_drag.csv       # CD vs Mach (motor firing)
│       └── power_off_drag.csv      # CD vs Mach (coast phase)
└── output/                         # Generated outputs (git-ignored)
    ├── trajectory.kml              # Google Earth flight path
    ├── figures/                    # Exported plots
    └── reports/                    # Analysis summaries
```

---

## 🚀 Quick Start

### Prerequisites

| Requirement | Version | Purpose |
|-------------|---------|---------|
| Python | 3.8+ | Runtime |
| RocketPy | 1.0+ | Simulation engine |
| NumPy | 1.20+ | Numerical computing |
| Matplotlib | 3.4+ | Visualization |
| Jupyter | - | Notebook interface |

### Installation

```bash
# Create virtual environment (recommended)
python -m venv venv
source venv/bin/activate  # Linux/Mac
# or: venv\Scripts\activate  # Windows

# Install dependencies
pip install rocketpy numpy matplotlib jupyter

# Or install all at once
pip install rocketpy[all]
```

<details>
<summary><strong>Conda Installation</strong></summary>

```bash
conda create -n pioneer-sim python=3.10
conda activate pioneer-sim
conda install -c conda-forge rocketpy jupyterlab
```
</details>

### Running the Simulation

```bash
# Navigate to notebooks
cd simulations/notebooks

# Launch Jupyter
jupyter notebook pioneer_simulation.ipynb
# or: jupyter lab
```

**Run all cells sequentially** — the notebook is structured to build the simulation progressively.

---

## 📊 Simulation Components

### Environment Model
| Parameter | Value | Source |
|-----------|-------|--------|
| Launch Site | Sriharikota, India | SHAR |
| Coordinates | 13.47°N, 80.00°E | - |
| Elevation | 20.6 m ASL | Survey data |
| Weather | NOAA GFS Forecast | Real-time fetch |
| Rail Length | 5.0 m | Competition spec |

### Motor: Inferno K1120
| Specification | Value |
|---------------|-------|
| Classification | K1120 (2800 N·s) |
| Grain Type | BATES (4-grain) |
| Diameter | 76 mm |
| Length | 631 mm |
| Propellant Mass | 1.82 kg |
| Total Mass | 6.82 kg |
| Peak Thrust | ~1183 N |
| Avg Thrust | 1120 N |

> **Validation:** Thrust curve derived from OpenMotor and validated against Static Fire Test-3 data.

### Rocket Configuration
| Component | Specification |
|-----------|---------------|
| **Total Length** | ~2.1 m |
| **Body Diameter** | 102 mm (4") |
| **Nose Cone** | Ogive, 250 mm, fiberglass |
| **Fins** | 4× Trapezoidal, G10 fiberglass |
| **Dry Mass** | ~8.5 kg |
| **Recovery** | Drogue + Main (dual deploy) |

### Analysis Outputs
- 3D trajectory visualization
- Velocity/altitude profiles
- Static margin evolution
- Rail exit conditions check
- Dispersion ellipse (Monte Carlo)
- Frequency response plots

---

## 📈 Results & Visualization

### Trajectory Output
The simulation generates comprehensive flight data:

| Output | Description | Format |
|--------|-------------|--------|
| 3D Trajectory | Full flight path visualization | Interactive plot |
| KML Export | Google Earth compatible | `.kml` |
| Flight Phases | Powered → Coast → Drogue → Main | Timeline |
| Landing Zone | Predicted touchdown location | Coordinates |

### Key Plots Generated
1. **Altitude vs Time** — Flight profile with phase markers
2. **Velocity Components** — Vx, Vy, Vz decomposition  
3. **Static Margin** — Stability throughout flight
4. **Attitude** — Pitch, yaw, roll angles
5. **Aerodynamic Forces** — Drag, lift coefficients
6. **Energy** — Kinetic + potential energy budget

### Stability Verification
| Check | Criterion | Status |
|-------|-----------|--------|
| Rail Exit Stability | ≥ 1.5 cal | ✅ 1.95 cal |
| Rail Exit Velocity | ≥ 30 m/s | ✅ ~52 m/s |
| Max Static Margin | ≤ 6.0 cal | ✅ Nominal |
| Weathercock Stability | No divergence | ✅ Verified |

---

## 🔧 Data File Formats

### Motor File (RASP `.eng`)

Standard format compatible with OpenRocket, RASAero, and RockSim:

```
; Comment line (optional)
<Code> <Diameter_mm> <Length_mm> <Delays> <PropMass_kg> <TotalMass_kg> <Mfg>
<Time_s> <Thrust_N>
<Time_s> <Thrust_N>
...
```

**Example** (`inferno_k1120.eng`):
```
K1120 76.0 631.0 P 1.820874 6.820874 ignition
0 0.01
0.03 1091.95
0.06 1097.18
...
2.50 0.0
```

### Drag Coefficient File (`.csv`)

Two-column CSV: Mach number and drag coefficient (CD).

```csv
Mach Number,CD
0.01,0.46
0.10,0.47
0.30,0.45
0.50,0.46
...
```

> **Note:** Separate files for power-on (jet effects) and power-off (clean airframe) phases.

---

## 🤝 Contributing

### Adding New Simulations

1. Create notebook in `notebooks/` with naming: `<rocket>_<analysis>.ipynb`
2. Use `pathlib.Path` for cross-platform paths
3. Document assumptions in markdown cells
4. Export key results to `output/`

### Updating Motor Data

1. Generate thrust curve in OpenMotor (or from test data)
2. Export to RASP `.eng` format
3. Place in `data/motor/` with lowercase naming: `motor_name.eng`
4. Add validation notes in notebook

### Code Style

```python
# Use pathlib for paths
from pathlib import Path
DATA_DIR = Path(__file__).parent.parent / "data"

# Document parameters
motor = SolidMotor(
    thrust_source=DATA_DIR / "motor/inferno_k1120.eng",
    dry_mass=5.0,  # kg, measured post-burn
    ...
)
```

---

## 📚 References

| Resource | Link |
|----------|------|
| RocketPy Documentation | [docs.rocketpy.org](https://docs.rocketpy.org) |
| RocketPy Examples | [GitHub Examples](https://github.com/RocketPy-Team/RocketPy/tree/master/docs/examples) |
| RASP Motor Format | [thrustcurve.org](http://www.thrustcurve.org/raspformat.shtml) |
| OpenMotor | [github.com/reilleya/openMotor](https://github.com/reilleya/openMotor) |
| ThrustCurve Database | [thrustcurve.org](https://www.thrustcurve.org) |

---

## ⚠️ Important Notes

1. **Weather Data**: Simulations using real-time NOAA data require internet connection
2. **Coordinate System**: All coordinates use ENU (East-North-Up) convention
3. **Reproducibility**: Set random seed for Monte Carlo runs to ensure reproducibility
4. **Units**: RocketPy uses SI units (m, kg, s, N) throughout

---

## 👥 Authors

| Contributor | Role |
|-------------|------|
| **Anurag** | Simulation Development, Validation |
| **Pranjal** | Simulation Development, Motor Characterization, Static Fire Analysis, |

**Organization:** Team Ignition VIT Chennai  
**Project:** PIONEER Sounding Rocket  
**Competition:** Spaceport America Cup 2025

---

## 📄 License

This simulation code is part of the Pioneer Avionics project.  
See the repository [LICENSE](../LICENSE) for terms.

---

<div align="center">
  <sub>Built with 🚀 by Team Ignition</sub>
</div>
