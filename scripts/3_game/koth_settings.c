class KOTH_Settings
{
	private static string m_Directory = "$profile:KOTH-SAVIOR";
	private static string m_Path = m_Directory + "/%1.json";
	private static string m_WorldName;
	
	static ref KOTH_SettingsData m_Data = new KOTH_SettingsData;

	static bool LoadData()
	{
		GetGame().GetWorldName(m_WorldName);
		
		if (!FileExist(m_Directory))
			MakeDirectory(m_Directory);

		if (!FileExist(string.Format(m_Path, m_WorldName)))
		{
			KOTH_Log.LogVerbose("Writing default config.");
			m_Data.InitDefaults(m_WorldName);
			SaveData();
		}
		else
		{
			JsonFileLoader<KOTH_SettingsData>.JsonLoadFile( string.Format(m_Path, m_WorldName), m_Data );
			UpgradeData();
			SaveData();
		}
		
		OnLoaded();

		return true;
	}
	
	static bool SaveData()
	{
		GetGame().GetWorldName(m_WorldName);
		JsonFileLoader<KOTH_SettingsData>.JsonSaveFile( string.Format(m_Path, m_WorldName), m_Data );
		return true;
	}

	static void UpgradeData()
	{
		if (m_Data.configVersion < 1)
		{
			m_Data.timeGasDuration = 300;
			
			foreach (KOTH_Zone zone : m_Data.zones)
			{
				zone.timeGasDuration = -1;
			}

			m_Data.configVersion = 1;
		}

		if (m_Data.configVersion < 2)
		{
			m_Data.vehicleKothEnabled = true;
			m_Data.vehicleKothChance = 25.0;
			m_Data.maxVehicleEvents = 1;
			m_Data.vehicleBaseCaptureTime = 600;
			m_Data.vehicleMinimumPlayers = 1;
			m_Data.vehicleMaximumPlayers = 6;
			m_Data.vehicleRewardCount = 1;

			if (!m_Data.vehicleClassnames)
				m_Data.vehicleClassnames = new array<string>;
			if (!m_Data.vehicleLootSets)
				m_Data.vehicleLootSets = new array<string>;
			if (!m_Data.vehicleZones)
				m_Data.vehicleZones = new array<ref KOTH_Zone>;

			m_Data.configVersion = 2;
		}

		if (m_Data.configVersion < 3)
		{
			m_Data.vehicleMinTimeBetweenEvents = 3600;
			m_Data.vehicleMaxTimeBetweenEvents = 7200;
			m_Data.vehicleGuaranteedPerRestart = true;
			m_Data.configVersion = 3;
		}
	}
	
	static void OnLoaded()
	{
		KOTH_Log.LogVerbose("Config loaded.");
	}
	
	static void SyncDataSend(notnull Man player)
	{
		GetGame().RPCSingleParam(player, eKOTH.RPC_KOTH_CONFIG_SYNC, new Param1<KOTH_SettingsData>(m_Data), true, player.GetIdentity());
	}

	static void OnRPC(Man player, ParamsReadContext ctx)
	{
		Param1<KOTH_SettingsData> data = new Param1<KOTH_SettingsData>(null);
		
		if (ctx.Read(data))
			m_Data = data.param1;
		else
			KOTH_Log.LogCritical("Failed to read configuration data.");
		
		player.OnGameplayDataHandlerSync();
	}

	static bool IsEnabled() { return m_Data.enabled; }
	static bool IsUseLocationText() { return m_Data.useLocationText; }
	static bool IsUseMapMarker() { return m_Data.useMapMarker; }
	static bool IsUseNotifications() { return m_Data.useNotifications; }
	static bool IsUseProgressHUD() { return m_Data.useProgressHUD; }

	static bool IsReduceProgressOnAbandoned() { return m_Data.reduceProgressOnAbandoned; }
	static bool IsReduceProgressOnDeathFromOutside() { return m_Data.reduceProgressOnDeathFromOutside; }
	static bool IsRequireFlagConstruction() { return m_Data.requireFlagConstruction; }
	static bool IsCelebrateWin() { return m_Data.celebrateWin; }
	static bool IsPunishLoss() { return m_Data.punishLoss; }
	static bool IsEstimateLocation() { return m_Data.estimateLocation; }

	static bool IsVehicleKothEnabled() 
	{ 
		if (!m_Data.vehicleKothEnabled)
			return false;
		return m_Data.vehicleKothEnabled; 
	}
	
	static float GetVehicleKothChance() { return m_Data.vehicleKothChance; }
	static int GetMaxVehicleEvents() { return m_Data.maxVehicleEvents; }
	static float GetVehicleBaseCaptureTime() { return m_Data.vehicleBaseCaptureTime; }
	static int GetVehicleMinimumPlayers() { return m_Data.vehicleMinimumPlayers; }
	static int GetVehicleMaximumPlayers() { return m_Data.vehicleMaximumPlayers; }
	static int GetVehicleRewardCount() { return m_Data.vehicleRewardCount; }

	static float GetVehicleMinTimeBetweenEvents() { return m_Data.vehicleMinTimeBetweenEvents; }
	static float GetVehicleMaxTimeBetweenEvents() { return m_Data.vehicleMaxTimeBetweenEvents; }
	static bool IsVehicleGuaranteedPerRestart() { return m_Data.vehicleGuaranteedPerRestart; }

	static float GetBaseCaptureTime() { return m_Data.baseCaptureTime; }
	static float GetMaxTimeBetweenEvents() { return m_Data.maxTimeBetweenEvents; }
	static float GetMinTimeBetweenEvents() { return m_Data.minTimeBetweenEvents; }
	static float GetPlayerTimeMultiplier() { return m_Data.playerTimeMultiplier; }
	static float GetTimeDespawn() { return m_Data.timeDespawn; }
	static float GetTimeLimit() { return m_Data.timeLimit; }
	static float GetTimeSpawn() { return m_Data.timeSpawn; }
	static float GetTimeStart() { return m_Data.timeStart; }
	static float GetTimeZoneCooldown() { return m_Data.timeZoneCooldown; }
	static float GetTimeGasDuration() { return m_Data.timeGasDuration; }

	static int GetLoggingLevel() { return m_Data.loggingLevel; }
	static int GetMinPlayerCount() { return m_Data.minPlayerCount; }

	static int GetMaxEnemyCount() { return m_Data.maxEnemyCount; }
	static int GetMinEnemyCount() { return m_Data.minEnemyCount; }

	static int GetMaxEvents() { return m_Data.maxEvents; }
	static int GetMinimumDeaths() { return m_Data.minimumDeaths; }
	static int GetMinimumPlayers() { return m_Data.minimumPlayers; }
	static int GetMaximumPlayers() { return m_Data.maximumPlayers; }

	static int GetRewardCount() { return m_Data.rewardCount; }

	static string GetFlagClassname() { return m_Data.flagClassname; }

	static array<string> GetEnemies() {	return m_Data.enemies; }

	static string GetLootCrate()
	{
		return m_Data.lootCrate;
	}

	static int GetCrateLifeTime()
	{
		return m_Data.crateLifeTime;
	}

	static array<string> GetLootSets() { return m_Data.lootSets; }
	static array<ref KOTH_Zone> GetZones() { return m_Data.zones; }

	static array<string> GetVehicleClassnames() 
	{ 
		if (!m_Data.vehicleClassnames)
		{
			KOTH_Log.LogCritical("Vehicle classnames array is NULL!");
			return new array<string>;
		}
		return m_Data.vehicleClassnames; 
	}
	
	static array<string> GetVehicleLootSets() 
	{ 
		if (!m_Data.vehicleLootSets)
		{
			KOTH_Log.LogCritical("Vehicle loot sets array is NULL!");
			return new array<string>;
		}
		return m_Data.vehicleLootSets; 
	}
	
	static array<ref KOTH_Zone> GetVehicleZones() 
	{ 
		if (!m_Data.vehicleZones)
		{
			KOTH_Log.LogCritical("Vehicle zones array is NULL!");
			return new array<ref KOTH_Zone>;
		}
		return m_Data.vehicleZones; 
	}
}