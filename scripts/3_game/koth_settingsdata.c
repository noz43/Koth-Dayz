class KOTH_SettingsData
{
	int configVersion;
	
	bool enabled;
	int loggingLevel;

	bool useLocationText;
	bool useMapMarker;
	bool useNotifications;
	bool useProgressHUD;
	bool reduceProgressOnAbandoned;
	bool reduceProgressOnDeathFromOutside;
	bool requireFlagConstruction;
	bool estimateLocation;
	bool celebrateWin;
	bool punishLoss;

	bool vehicleKothEnabled;
	float vehicleKothChance;
	int maxVehicleEvents;
	float vehicleBaseCaptureTime;
	int vehicleMinimumPlayers;
	int vehicleMaximumPlayers;
	int vehicleRewardCount;
	
	// NOUVEAU: Paramètres de timing séparés pour véhicules
	float vehicleMinTimeBetweenEvents;
	float vehicleMaxTimeBetweenEvents;
	bool vehicleGuaranteedPerRestart;

	float baseCaptureTime;
	float maxTimeBetweenEvents;
	float minTimeBetweenEvents;
	float playerTimeMultiplier;
	float timeDespawn;
	float timeLimit;
	float timeStart;

	float timeSpawn;
	float timeZoneCooldown;
	float timeGasDuration;

	int minPlayerCount;
	
	int maxEnemyCount;
	int minEnemyCount;
	int maxEvents;
	int minimumDeaths;
	int minimumPlayers;
	int maximumPlayers;

	int rewardCount;

	string flagClassname;

	ref array<string> enemies = 
	{
		"ZmbM_PatrolNormal_Autumn",
		"ZmbM_PatrolNormal_Flat",
		"ZmbM_PatrolNormal_PautRev",
		"ZmbM_PatrolNormal_Summer",
		"ZmbM_SoldierNormal",
		"ZmbM_usSoldier_normal_Desert",
		"ZmbM_usSoldier_normal_Woodland"
	};

	string lootCrate;
	int crateLifeTime;
	
	ref array<string> lootSets = {};

	ref array<string> vehicleClassnames = {};
	ref array<string> vehicleLootSets = {};

	ref array<ref KOTH_Zone> zones = new array<ref KOTH_Zone>;
	ref array<ref KOTH_Zone> vehicleZones = new array<ref KOTH_Zone>;
	
	void InitDefaults(string worldName)
	{
		configVersion = 3;
		
		enabled = true;
		loggingLevel = 2;
		
		useLocationText = false;
		useMapMarker = true;
		useNotifications = true;
		useProgressHUD = true;
		reduceProgressOnAbandoned = true;
		reduceProgressOnDeathFromOutside = true;
		requireFlagConstruction = false;
		celebrateWin = true;
		estimateLocation = false;
		punishLoss = false;

		vehicleKothEnabled = true;
		vehicleKothChance = 100.0;
		maxVehicleEvents = 1;
		vehicleBaseCaptureTime = 600;
		vehicleMinimumPlayers = 1;
		vehicleMaximumPlayers = 6;
		vehicleRewardCount = 1;
		
		// NOUVEAU: Timing séparé pour véhicules
		vehicleMinTimeBetweenEvents = 3600;
		vehicleMaxTimeBetweenEvents = 7200;
		vehicleGuaranteedPerRestart = true;

		baseCaptureTime = 900;
		maxTimeBetweenEvents = 2700;
		minTimeBetweenEvents = 1800;
		playerTimeMultiplier = 1.5;
		timeDespawn = 120;
		timeLimit = 0;
		timeStart = 30;
		timeSpawn = 120;
		timeZoneCooldown = 120;
		timeGasDuration = 300;

		minPlayerCount = 1;

		maxEnemyCount = 20;
		minEnemyCount = 5;
		maxEvents = 2;
		minimumDeaths = 0;
		minimumPlayers = 1;
		maximumPlayers = 4;

		rewardCount = 2;

		flagClassname = "Flag_DayZ";

		lootCrate = "KOTH_Chest";

		InitVehicleDefaults();
		
		worldName.ToLower();
		
		switch (worldName)
		{
			case "chernarusplus":
			case "chernarusplusgloom":
				RegisterZone("Tisy Military", Vector(1677.6, 451.0, 14348.9), 50);
				RegisterZone("Grozovy Military", Vector(3669.2, 363.0, 14802.8), 20);
				RegisterZone("Kamensk Military", Vector(7105.2, 355.0, 14582.8), 20);
				RegisterZone("Novy Lug Military", Vector(9559.2, 242.0, 11802.8), 20);
				RegisterZone("Krasno Airfield", Vector(11871.2, 140.0, 12552.8), 20);
				RegisterZone("Staroye Military", Vector(10432.2, 283.0, 5954.8), 20);
				RegisterZone("Pavlovo Military", Vector(2194.2, 91.0, 3363.8), 20);
				RegisterZone("Myshkino Military", Vector(1161.2, 185.0, 7248.8), 20);
				RegisterZone("VMC Military", Vector(4564.2, 317.0, 8280.8), 20);
				RegisterZone("Zelengorsk Military", Vector(2515.2, 193.0, 5078.8), 20);
				RegisterZone("Vybor Airfield", Vector(4643.2, 339.0, 10464.8), 20);
				RegisterZone("Kamensk Military Base", Vector(8006.3, 339.5, 14627.9), 20);

				RegisterVehicleZone("NWAF Parking", Vector(4675.0, 337.0, 10300.0), 75);
				RegisterVehicleZone("Balota Airfield", Vector(4975.0, 10.0, 2525.0), 60);
				RegisterVehicleZone("Cherno Industrial", Vector(6800.0, 10.0, 2650.0), 50);
				RegisterVehicleZone("Elektro Industrial", Vector(10400.0, 56.0, 2100.0), 50);
				RegisterVehicleZone("Solnichniy Factory", Vector(13400.0, 19.0, 6100.0), 40);
			break;

			case "namalsk":
				RegisterZone("Airfield", Vector(6247.6, 21.1, 9314.9), 50);
				RegisterZone("Factory", Vector(6473.2, 16.0, 9305.8), 20);
				RegisterZone("BK-T07", Vector(4408.0, 15.0, 10736.4), 20);
				RegisterZone("BK-M06", Vector(4847.4, 13.2, 10869.5), 50);
				RegisterZone("Citadel", Vector(4685.6, 23.5, 8910.3), 50);
				RegisterZone("Eagle's Nest", Vector(4084.0, 70.3, 9212.82), 25);
				RegisterZone("RefugeeCamp", Vector(7310.3, 45.5, 10525.86), 20);
				RegisterZone("Seraja", Vector(4202.8, 390.65, 6607.4), 50);
				RegisterZone("Tara", Vector(7254.5, 238.9, 7077.0), 50);
				RegisterZone("Uranium Mine", Vector(5074.4, 39.5, 8172.8), 50);
				RegisterZone("Vorkuta", Vector(6685.6, 15.0, 11158.7), 50);

				RegisterVehicleZone("Airfield Hangar", Vector(6200.0, 21.0, 9250.0), 60);
				RegisterVehicleZone("Lubjansk Depot", Vector(8300.0, 45.0, 9800.0), 45);
				RegisterVehicleZone("Sebjan Dam", Vector(4500.0, 15.0, 11900.0), 50);
			break;
			
			case "deerisle":
				RegisterZone("Alcatraz", Vector(15757.2, 22.3, 5102.7), 75);
				RegisterZone("Area 42 Prison", Vector(13282.0, 12.9, 9833.4), 20);
				RegisterZone("Broken Houses", Vector(14285.1, 2.5, 6458.3), 20);
				RegisterZone("Crater", Vector(14145.9, 134.1, 2642.0), 25);
				RegisterZone("Fort Knox", Vector(3779.5, 16.8, 9050.9), 20);
				RegisterZone("Paris Island", Vector(2481.5, 35.1, 3352.0), 100);
				RegisterZone("Prison", Vector(5522.8, 34.85, 681.0), 25);
				RegisterZone("Rockenheim", Vector(9790.0, 40.0, 11093.1), 20);

				RegisterVehicleZone("Harbour District", Vector(12500.0, 5.0, 7800.0), 70);
				RegisterVehicleZone("Industrial Complex", Vector(8500.0, 25.0, 4500.0), 55);
				RegisterVehicleZone("Airport Terminal", Vector(6800.0, 45.0, 11200.0), 65);
			break;
			
			default:
				RegisterZone("Dummy", Vector(0.0, 0.0, 0.0), 50);
				RegisterVehicleZone("Vehicle Dummy", Vector(100.0, 0.0, 100.0), 75);
			break;
		}
	}

	protected void InitVehicleDefaults()
	{
		vehicleClassnames = {
			"CivilianSedan",
			"CivilianSedan_Wine",
			"CivilianSedan_Black",
			"Sedan_02",
			"Hatchback_02"
		};

		vehicleLootSets = {
			"VehicleRepair",
			"MilitaryVehicle", 
			"SurvivalKit"
		};
	}
	
	protected void RegisterZone(string name, vector position, int radius)
	{
		KOTH_Zone zone = new KOTH_Zone(name, position, radius);
		zone.InitDefaults();
		zones.Insert(zone);
	}

	protected void RegisterVehicleZone(string name, vector position, int radius)
	{
		KOTH_Zone zone = new KOTH_Zone(name, position, radius);
		zone.InitDefaults();
		vehicleZones.Insert(zone);
	}
}