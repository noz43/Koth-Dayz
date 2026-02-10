# 🏆 DayZ KOTH (King of the Hill) Mod

[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/noz43/dayz-koth)
[![DayZ](https://img.shields.io/badge/DayZ-1.26-green.svg)](https://dayz.com)
[![License](https://img.shields.io/badge/license-MIT-orange.svg)](LICENSE)

A comprehensive King of the Hill modification for DayZ servers featuring dynamic zone capture events, vehicle KOTH mechanics with time-based spawning (18:00-00:00), and an integrated persistence system.

---

## 📋 Table of Contents

- [Features](#-features)
- [Installation](#-installation)
- [Configuration Files](#-configuration-files)
  - [Map Config (chernarus.json)](#map-config-chernarusjson)
  - [Loot Config (loot.json)](#loot-config-lootjson)

---

## ✨ Features

- **🎯 Dynamic KOTH Zones** - Multiple configurable capture zones
- **🚗 Vehicle KOTH Events** - Capture vehicles that spawn between 18:00-00:00
- **💾 Integrated Persistence** - Vehicles auto-save to database
- **🎁 Customizable Loot** - JSON-based reward configuration
- **📊 Real-time UI** - Progress HUD, map markers, and notifications
- **⚡ Multi-Event Support** - Run multiple events simultaneously

---

## 🚀 Installation

1. Extract to your DayZ server directory
2. Add to server startup: `-mod=@YourOtherMods;@DayZ-KOTH`
3. Configure your files (see below)
4. Restart server

---

## ⚙️ Configuration Files

### Map Config (chernarus.json)

**Location**: `config/chernarus.json`

> **⚠️ Important**: File must be named after your map (e.g., `livonia.json`, `namalsk.json`)
```json
{
  "configVersion": 3,                    // Version control for mod updates
  
  // ===== MAIN SETTINGS =====
  "enabled": true,                       // Master switch: enable/disable entire mod
  "loggingLevel": 2,                     // Console log detail: 1=Errors only, 2=Info, 3=Debug
  
  // ===== UI SETTINGS =====
  "useLocationText": false,              // Show zone name on screen during capture
  "useMapMarker": true,                  // Display KOTH marker on player's map
  "useNotifications": true,              // Show popup messages for events
  "useProgressHUD": true,                // Display capture progress bar on screen
  
  // ===== TIMING SETTINGS (all values in seconds) =====
  "baseCaptureTime": 300,                // Default time to capture a zone (5 min)
  "maxTimeBetweenEvents": 1800,          // Maximum wait before next event (30 min)
  "minTimeBetweenEvents": 900,           // Minimum wait before next event (15 min)
  
  // ===== PLAYER REQUIREMENTS =====
  "minPlayerCount": 1,                   // Min online players to start any event
  "maxEvents": 2,                        // Max simultaneous KOTH events running
  "minimumPlayers": 1,                   // Min players inside zone to start capture
  "maximumPlayers": 4,                   // Max players allowed in zone (prevents zerging)
  
  // ===== REGULAR KOTH REWARDS =====
  "rewardCount": 1,                      // How many loot sets to give from list below
  "flagClassname": "Flag_DayZ",          // Flag object spawned in zone center
  "lootCrate": "SeaChest",               // Container type for reward items
  "crateLifeTime": 3600,                 // How long crate stays before despawn (1 hour)
  "lootSets": [                          // Available reward sets (defined in loot.json)
    "AK74",                              // AK74 weapon with magazine
    "M4A1",                              // M4A1 weapon with magazine
    "SurvivalKit"                        // Medical items and food
  ],
  
  // ===== VEHICLE KOTH SETTINGS =====
  "vehicleKothEnabled": true,            // Enable vehicle capture events
  "vehicleKothChance": 50.0,             // % chance of vehicle event vs regular (0-100)
  "maxVehicleEvents": 1,                 // Max simultaneous vehicle events
  "vehicleBaseCaptureTime": 180,         // Time to capture vehicle (3 min)
  "vehicleMinimumPlayers": 1,            // Min players near vehicle to capture
  "vehicleMaximumPlayers": 3,            // Max players allowed near vehicle
  "vehicleRewardCount": 1,               // How many vehicle loot sets to give
  "vehicleMinTimeBetweenEvents": 1800,   // Min wait between vehicle events (30 min)
  "vehicleMaxTimeBetweenEvents": 3600,   // Max wait between vehicle events (1 hour)
  "vehicleClassnames": [                 // Vehicle types that can spawn
    "CivilianSedan",                     // Regular 4-door sedan
    "Hatchback_02"                       // Small civilian car
  ],
  "vehicleLootSets": [                   // Reward sets for vehicles (from loot.json)
    "VehicleRepair"                      // Tools and parts for repairs
  ],
  
  // ===== REGULAR KOTH ZONES =====
  "zones": [
    {
      "zoneName": "NWAF",                // Zone display name shown to players
      "zonePosition": [4675.0, 337.0, 10300.0],  // [X, Y, Z] map coordinates
      "zoneRadius": 50,                  // Capture area radius in meters
      "baseCaptureTime": -1,             // Override capture time (-1 = use default)
      "objects": [                       // Decorative objects spawned in zone
        {
          "classname": "Wreck_Mi8",      // Object type (helicopter wreck)
          "position": [15, 0, 5],        // [X, Y, Z] relative to zone center
          "orientation": [0, 45, 0],     // [Pitch, Yaw, Roll] rotation angles
          "absolutePlacement": false,    // false = relative, true = absolute coords
          "alignToTerrain": true,        // Auto-adjust to ground slope
          "placeOnSurface": true         // Snap object to ground level
        }
      ]
    },
    {
      "zoneName": "Tisy",
      "zonePosition": [1677.6, 451.0, 14348.9],
      "zoneRadius": 40,
      "baseCaptureTime": -1,
      "objects": [
        {
          "classname": "Wreck_Mi8",
          "position": [10, 0, 10],
          "orientation": [0, 90, 0],
          "absolutePlacement": false,
          "alignToTerrain": true,
          "placeOnSurface": true
        }
      ]
    },
    {
      "zoneName": "Balota",
      "zonePosition": [4975.0, 10.0, 2525.0],
      "zoneRadius": 35,
      "baseCaptureTime": -1,
      "objects": [
        {
          "classname": "Wreck_Mi8",
          "position": [20, 0, 0],
          "orientation": [0, 180, 0],
          "absolutePlacement": false,
          "alignToTerrain": true,
          "placeOnSurface": true
        }
      ]
    }
  ],
  
  // ===== VEHICLE SPAWN ZONES (18:00-00:00 only) =====
  "vehicleZones": [
    {
      "zoneName": "Elektro",             // Vehicle spawn location name
      "zonePosition": [10400.0, 56.0, 2100.0],  // [X, Y, Z] spawn coordinates
      "zoneRadius": 30,                  // Area where vehicle can appear
      "baseCaptureTime": -1,             // Override time (-1 = use vehicleBaseCaptureTime)
      "objects": []                      // Usually empty (vehicle is the marker)
    },
    {
      "zoneName": "Cherno",
      "zonePosition": [6800.0, 10.0, 2650.0],
      "zoneRadius": 40,
      "baseCaptureTime": -1,
      "objects": []
    }
  ]
}
```

---

### Loot Config (loot.json)

**Location**: `config/loot.json`
```json
{
  "lootSets": [
    {
      "name": "AK74",                    // Unique ID for this loot set
      "items": [
        {
          "name": "AK74",                // Item classname to spawn
          "quantity": -1,                // -1 = default amount (1 item)
          "attachments": [               // Items attached to this item
            {
              "name": "Mag_AK74_30Rnd",  // Magazine classname
              "quantity": 30,            // Rounds in magazine (30/30)
              "attachments": [],         // Magazine has no attachments
              "cargo": []                // Magazine has no cargo
            }
          ],
          "cargo": []                    // Weapon internal storage (empty)
        }
      ]
    },
    {
      "name": "M4A1",
      "items": [
        {
          "name": "M4A1",
          "quantity": -1,
          "attachments": [
            {
              "name": "Mag_STANAG_30Rnd",
              "quantity": 30,
              "attachments": [],
              "cargo": []
            }
          ],
          "cargo": []
        }
      ]
    },
    {
      "name": "SurvivalKit",             // Backpack with medical/food items
      "items": [
        {
          "name": "TaloonBag_Blue",      // Container item (backpack)
          "quantity": -1,
          "attachments": [],             // Backpack has no attachments
          "cargo": [                     // Items inside the backpack
            {
              "name": "Bandage",         // Medical bandage
              "quantity": -1,            // Default amount (1 bandage)
              "attachments": [],
              "cargo": []
            },
            {
              "name": "WaterBottle",     // Water container
              "quantity": 100,           // Liquid amount: 100 = 100% full
              "attachments": [],
              "cargo": []
            },
            {
              "name": "PeachesCan",      // Food item
              "quantity": -1,            // Default amount (1 can)
              "attachments": [],
              "cargo": []
            }
          ]
        }
      ]
    },
    {
      "name": "VehicleRepair",           // Tools for vehicle maintenance
      "items": [
        {
          "name": "Wrench",              // Repair tool
          "quantity": -1,                // Default amount (1 wrench)
          "attachments": [],             // Tools have no attachments
          "cargo": []                    // Tools have no storage
        },
        {
          "name": "SparkPlug",           // Vehicle part
          "quantity": -1,
          "attachments": [],
          "cargo": []
        },
        {
          "name": "CarBattery",          // Vehicle part
          "quantity": -1,
          "attachments": [],
          "cargo": []
        }
      ]
    }
  ]
}
```

**Quantity Rules**:
- **`-1`** = Default (usually 1 item)
- **Number** = Specific amount (e.g., `30` = 30 rounds, `100` = 100% full)

---

## ⏰ Time-Based Vehicle Spawning

Vehicle KOTH events **only spawn between 18:00 and 00:00** (6 PM to midnight).

| Time | Vehicle Events |
|------|----------------|
| 18:00 - 00:00 | ✅ Can spawn |
| 00:00 - 18:00 | ❌ No spawns |

> Regular KOTH zones work 24/7. Captured vehicles persist forever.

---

## 💾 Persistence System

**Automatic vehicle persistence** - no config needed!

When a vehicle is captured:
- ✅ Position saved to database
- ✅ Damage state preserved
- ✅ Inventory kept
- ✅ Fuel level saved
- ✅ Survives server restarts

---

## 🐛 Troubleshooting

| Problem | Solution |
|---------|----------|
| No events starting | Check `enabled: true` and `minPlayerCount` |
| Vehicles not spawning | Verify time is 18:00-00:00 and `vehicleKothEnabled: true` |
| Empty loot crates | Validate JSON syntax, check item classnames |
| No map markers | Set `useMapMarker: true` |

---


## 📝 License

MIT License - Free to use and modify

---


</div>
