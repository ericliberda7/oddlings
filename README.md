<div align="center">

# Oddlings

**Procedural C++20 creature simulation for M5Stack CoreS3 and Tab5**

[![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)](#firmware-downloads)
[![M5Stack CoreS3](https://img.shields.io/badge/CoreS3-320×240-D97736?style=flat-square)](#cores3-320240)
[![M5Stack Tab5](https://img.shields.io/badge/Tab5-1280×720-4A9E8E?style=flat-square)](#tab5-sanctuary-hub-1280720)
[![ESP-NOW](https://img.shields.io/badge/ESP--NOW-2.4_GHz-7A68C8?style=flat-square)](#esp-now-wireless-sync)
[![Download Firmware](https://img.shields.io/badge/Download-Firmware_.bin-2E8B57?style=flat-square)](#firmware-downloads)

<br />

<img src="website/assets/gif/tab5_sanctuary.gif" alt="Oddlings — Tab5 1280x720 Sanctuary Hub" width="100%" />

</div>

---

## Overview

Oddlings is a real-time C++20 creature simulation and software rasterizer for the **M5Stack CoreS3** (`320×240`, ESP32-S3) and **M5Stack Tab5** (`1280×720`, ESP32-P4).

Creatures are generated procedurally in C++ from a 64-bit seed (`uint64_t`) with no sprite sheets. Body geometry, spline tentacles, articulated limbs, watercolor shading, and pen-and-ink crosshatching are computed per scanline.

---

## Firmware Downloads

Download prebuilt `.bin` images directly below or from the [Releases](https://github.com/ericliberda7/oddlings/releases) page:

| Target Device | Binary File | Type & Flash Offset | Direct Download |
| :--- | :--- | :--- | :---: |
| **M5Stack CoreS3** (`ESP32-S3`) | `Oddlings.bin` | M5Launcher / MicroSD / OTA App (`0x10000`) | [**Download `Oddlings.bin`**](https://github.com/ericliberda7/oddlings/raw/main/sdcard/Oddlings.bin) |
| **M5Stack CoreS3** (`ESP32-S3`) | `Oddlings_Full_Merged.bin` | Full Merged Factory Image w/ Bootloader (`0x0000`) | [**Download `Oddlings_Full_Merged.bin`**](https://github.com/ericliberda7/oddlings/raw/main/sdcard/Oddlings_Full_Merged.bin) |
| **M5Stack Tab5 Hub** (`ESP32-P4`) | `Oddlings_Tab5_Hub.bin` | M5Launcher / MicroSD / OTA App (`0x10000`) | [**Download `Oddlings_Tab5_Hub.bin`**](https://github.com/ericliberda7/oddlings/raw/main/sdcard/Oddlings_Tab5_Hub.bin) |
| **M5Stack Tab5 Hub** (`ESP32-P4`) | `Oddlings_Tab5_Hub_Full_Merged.bin` | Full Merged Factory Image w/ Bootloader (`0x0000`) | [**Download `Oddlings_Tab5_Hub_Full_Merged.bin`**](https://github.com/ericliberda7/oddlings/raw/main/sdcard/Oddlings_Tab5_Hub_Full_Merged.bin) |

### Flashing Instructions

- **M5Launcher / MicroSD:** Copy [`Oddlings.bin`](https://github.com/ericliberda7/oddlings/raw/main/sdcard/Oddlings.bin) (CoreS3) or [`Oddlings_Tab5_Hub.bin`](https://github.com/ericliberda7/oddlings/raw/main/sdcard/Oddlings_Tab5_Hub.bin) (Tab5) to the root of a FAT32 MicroSD card and select it in M5Launcher.
- **esptool.py (Full Factory Image at `0x0000`):**
  ```bash
  esptool.py --chip esp32s3 write_flash 0x0000 Oddlings_Full_Merged.bin
  ```

---

## CoreS3 (`320×240`)

| **Noir Pen-and-Ink Mode** | **Touch & 1.9× Zoom** | **Feeding & Ball Physics** |
| :---: | :---: | :---: |
| <img src="website/assets/gif/cores3_noir_mode.gif" width="260" alt="Noir Pen-and-Ink Mode" /> | <img src="website/assets/gif/cores3_touch_loupe.gif" width="260" alt="Touch and 1.9x Zoom" /> | <img src="website/assets/gif/cores3_feeding_ball.gif" width="260" alt="Feeding and Ball Physics" /> |
| <sub>Toggle `NOIR: ON` for red-filter luminance separation and 45°/135° crosshatching.</sub> | <sub>Touch tracking, tentacle brushing, and double-tap 1.9× close-up zoom.</sub> | <sub>Drop food spores or spawn an elastic ball that reacts to screen tilt.</sub> |
| **IMU Tilt & Flip** | **16 Species & Hybrid Breeding** | **4-Stage Growth** |
| <img src="website/assets/gif/cores3_imu_flip.gif" width="260" alt="IMU Tilt and Flip" /> | <img src="website/assets/gif/cores3_species_parade.gif" width="260" alt="16 Species and Hybrid Breeding" /> | <img src="website/assets/gif/cores3_evolution.gif" width="260" alt="4-Stage Growth" /> |
| <sub>BMI270 6-axis gyro and accelerometer drive zero-G float and dizzy rolls.</sub> | <sub>16 base species (3 variants each) plus seed-based hybrid splicing.</sub> | <sub>Creatures progress from `BABY` → `JUVENILE` → `ADULT` → `ELDER*`.</sub> |
| **Voice Synth & Sleep** | **DNA Sculptor & Seed Keypad** | **ESP-NOW & On-Device OS (`5:UPD`)** |
| <img src="website/assets/gif/cores3_voice_sleep.gif" width="260" alt="Voice Synth and Sleep" /> | <img src="website/assets/gif/cores3_dna_sculptor.gif" width="260" alt="DNA Sculptor and Seed Keypad" /> | <img src="website/assets/gif/cores3_espnow_os.gif" width="260" alt="ESP-NOW and OS Menu" /> |
| <sub>4-voice formant audio responses, mic input, and sleep mode.</sub> | <sub>Edit dorsal parts, eye styles, and palettes, or load any 6-digit seed.</sub> | <sub>5-tab on-device menu (`1:PET`, `2:FAV`, `3:CFG`, `4:DNA`, `5:UPD`) and OTA tools.</sub> |

<p align="center">
  <img src="website/assets/img/unit_floater_showcase.png" alt="CoreS3 World Mode and Close-Up Mode" width="100%" />
</p>

---

## Noir Mode (Pen-and-Ink Crosshatching)

Toggling **`NOIR: ON`** in the `1:PET` or `3:CFG` menu switches the DMA scanline output to a monochrome pen-and-ink pass:

- **Panchromatic Red Filter:** Boosts warm creature tones against darker sky gradients (`warm_boost = ((r - b) * 3) >> 3`).
- **3-Tier Crosshatching:** Applies `45°` hatching in midtones, `135°` crosshatching in shadows, and horizontal engraving ticks in deep recesses with contour darkening.

<p align="center">
  <img src="website/assets/img/noir_mode_showcase.png" alt="Oddlings Noir Mode" width="100%" />
</p>

---

## Tab5 Sanctuary Hub (`1280×720`)

The Tab5 build pairs a **960×720 Sanctuary viewport** (3.0× render scale) with a **320×720 Command Deck**:

- **`1:PODS`:** Manage sanctuary pods and view nearby CoreS3 units over ESP-NOW.
- **`2:DNA`:** Live parent-to-offspring hybrid preview and trait controls.
- **`3:VAULT` & `4:SEED`:** 16-slot persistent seed bank, wireless OTA check, and 6-digit seed keypad.

<p align="center">
  <img src="website/assets/img/tab5_sanctuary_hub.png" alt="Tab5 1280x720 Sanctuary Hub" width="100%" />
</p>

---

## All 16 Species & 4-Stage Growth

Each of the 16 primary species includes 3 curated alternates (48 base morphs) across 16 procedural biomes, plus arbitrary hybrid combinations via 6-digit seeds (`#000001`–`#999999`).

<p align="center">
  <img src="website/assets/img/new_oddlings_mega_showcase.png" alt="All 16 Primary Species and 16 Worlds" width="100%" />
</p>

<p align="center">
  <img src="website/assets/img/evolution_traits_showcase.png" alt="4-Stage Evolution Progression" width="100%" />
</p>

<p align="center">
  <img src="website/assets/img/procedural_anatomy_sheet.png" alt="Procedural Component Graph" width="100%" />
</p>

---

## Firmware OTA Updates & M5Launcher

The on-device **`5:UPD`** tab includes built-in wireless HTTPS OTA and MicroSD update tools so you can update or switch firmwares directly on the device:

- **Wireless HTTPS OTA & MicroSD Update (`[INSTALL OS]` / `[CHECK FOR OS UPDATES]`):** Connects via 2.4 GHz Wi-Fi (`[WIFI CFG]` or `/wifi.txt` on MicroSD) or scans MicroSD (`/oddlings_update.bin`, `/update/Oddlings.bin`, `/oddlings/update.bin`), verifies the `0xE9` ESP32 image header, backs up all saved state to NVS and `/oddlings/save_v26.dat`, and flashes the new image directly to the OTA partition with an on-screen progress bar. On Tab5 (`3:VAULT` / `4:SEED`), wireless OTA updates can also be triggered directly from the sidebar.
- **Running Firmware Export (`[SAVE .BIN TO SD]`):** Reads the active flash partition and writes `/Oddlings.bin` to the MicroSD card for backup or sharing with another unit.
- **One-Tap M5Launcher Boot (`[BOOT LAUNCHER]`):** Switches the boot partition to a resident **M5Launcher** OTA slot or loads `/menu.bin` / `/launcher.bin` from MicroSD while keeping all saved creatures intact.

---

## ESP-NOW Wireless Sync

Nearby CoreS3 and Tab5 units discover each other on 2.4 GHz Channel 1 without Wi-Fi routers, using RSSI hysteresis filtering, replay-window protection, and CRC32-verified genome packets.

<p align="center">
  <img src="website/assets/img/espnow_p2p_encounter.png" alt="ESP-NOW Peer-to-Peer Encounter" width="100%" />
</p>

---

## Installation & Flashing

Prebuilt M5Launcher, OTA, and full factory `.bin` images are available in [`sdcard/`](sdcard/) and on the [GitHub Releases](https://github.com/ericliberda7/oddlings/releases) page:

```bash
# Flash full factory image (includes bootloader + partition table at 0x0000)
esptool.py --chip esp32s3 write_flash 0x0000 sdcard/Oddlings_Full_Merged.bin

# Or flash app partition only (at 0x10000)
esptool.py --chip esp32s3 write_flash 0x10000 sdcard/Oddlings.bin
```
