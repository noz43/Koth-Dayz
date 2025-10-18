🏴‍☠️ KOTH+ Vehicle Expansion Mod
King of The Hill (KOTH) Enhanced for DayZ

This mod expands the traditional King of The Hill (KOTH) gameplay by introducing:

🚗 Vehicle KOTH events — capture and control vehicles just like regular zones.

💾 Persistence system — vehicles remain saved and will not respawn if one already exists.

⚙️ Full JSON configuration — simple and powerful setup for all zones, events, and loot.

📦 Features
🔹 Standard KOTH Events

Players capture static KOTH zones for rewards.

Configurable zones defined per map (e.g., chernarus.json).

Customizable capture time, loot sets, and map markers.

🔹 Vehicle KOTH Events

Randomly selected vehicle capture events.

Configurable spawn chance, vehicle types, and reward loot.

Integrated persistence system prevents duplicate spawns if a vehicle already exists.

🔹 Dynamic Loot Rewards

Fully customizable loot sets defined in loot.json.

Weapons, tools, survival items — or define your own sets.

Supports nested attachments, cargo, and item hierarchies.

🔹 JSON Configuration System

Every setting is stored in .json files inside your mod’s config folder.

Each map (Chernarus, Livonia, etc.) uses its own configuration file:

config/
├── loot.json
├── chernarus.json
└── livonia.json


Makes it easy to modify without recompiling or editing source code.

⚙️ Configuration Overview
🗺️ Map Configuration (Example: chernarus.json)
{
  "enabled": true,
  "loggingLevel": 2,
  "useLocationText": false,
  "useMapMarker": true,
  "useNotifications": true,
  "useProgressHUD": true,

  "baseCaptureTime": 300,
  "maxTimeBetweenEvents": 1800,
  "minTimeBetweenEvents": 900,

  "minPlayerCount": 1,
  "maxEvents": 2,

  "rewardCount": 1,
  "flagClassname": "Flag_DayZ",
  "lootCrate": "SeaChest",
  "crateLifeTime": 3600,

  "lootSets": ["AK74", "M4A1", "SurvivalKit"],

  "vehicleKothEnabled": true,
  "vehicleKothChance": 50.0,
  "maxVehicleEvents": 1,
  "vehicleBaseCaptureTime": 180,
  "vehicleRewardCount": 1,
  "vehicleMinTimeBetweenEvents": 1800,
  "vehicleMaxTimeBetweenEvents": 3600,

  "vehicleClassnames": ["CivilianSedan", "Hatchback_02"],
  "vehicleLootSets": ["VehicleRepair"],

  "zones": [
    { "zoneName": "NWAF", "zonePosition": [4675.0, 337.0, 10300.0], "zoneRadius": 50 },
    { "zoneName": "Tisy", "zonePosition": [1677.6, 451.0, 14348.9], "zoneRadius": 40 },
    { "zoneName": "Balota", "zonePosition": [4975.0, 10.0, 2525.0], "zoneRadius": 35 }
  ],

  "vehicleZones": [
    { "zoneName": "Elektro", "zonePosition": [10400.0, 56.0, 2100.0], "zoneRadius": 30 },
    { "zoneName": "Cherno", "zonePosition": [6800.0, 10.0, 2650.0], "zoneRadius": 40 }
  ]
}

🎁 Loot Configuration (loot.json)
{
  "lootSets": [
    {
      "name": "AK74",
      "items": [
        {
          "name": "AK74",
          "attachments": [
            { "name": "Mag_AK74_30Rnd", "quantity": 30 }
          ]
        }
      ]
    },
    {
      "name": "M4A1",
      "items": [
        {
          "name": "M4A1",
          "attachments": [
            { "name": "Mag_STANAG_30Rnd", "quantity": 30 }
          ]
        }
      ]
    },
    {
      "name": "SurvivalKit",
      "items": [
        {
          "name": "TaloonBag_Blue",
          "cargo": [
            { "name": "Bandage" },
            { "name": "WaterBottle", "quantity": 100 },
            { "name": "PeachesCan" }
          ]
        }
      ]
    },
    {
      "name": "VehicleRepair",
      "items": [
        { "name": "Wrench" },
        { "name": "SparkPlug" },
        { "name": "CarBattery" }
      ]
    }
  ]
}
