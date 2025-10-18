class KOTH_Zone
{
	string zoneName;
	vector zonePosition;
	int zoneRadius;
	
	/* Overrides */
	float baseCaptureTime;
	float playerTimeMultiplier;
	float timeDespawn;
	float timeLimit;
	float timeStart;
	float timeGasDuration;

	int maxEnemyCount;
	int minEnemyCount;

	int minimumDeaths;
	int minimumPlayers;
	int maximumPlayers;

	int rewardCount;

	string flagClassname;
	
	ref array<ref KOTH_ObjectEntry> objects = new array<ref KOTH_ObjectEntry>;
	ref array<string> enemies = new array<string>;

	string lootCrate;
	int crateLifeTime;
	
	ref array<string> lootSets = {};

	void KOTH_Zone(string name, vector position, int radius)
	{
		zoneName = name;
		zonePosition = position;
		zoneRadius = radius;
	}

	void InitDefaults()
	{
		baseCaptureTime = -1;
		playerTimeMultiplier = -1;
		timeDespawn = -1;
		timeLimit = -1;
		timeStart = -1;
		timeGasDuration = -1;

		maxEnemyCount = -1;
		minEnemyCount = -1;

		minimumDeaths = -1;
		minimumPlayers = -1;
		maximumPlayers = -1;

		rewardCount = -1;

		flagClassname = string.Empty;
		
		ref KOTH_ObjectEntry object = new KOTH_ObjectEntry;
		object.InitDefaults();
		objects.Insert(object);

		lootCrate = "";
		crateLifeTime = -1;
		
		lootSets = {};
	}
	
	string GetName()
	{
		return zoneName;
	}
	
	vector GetPosition()
	{
		return zonePosition;
	}
	
	int GetRadius()
	{
		return zoneRadius;
	}
	
	float GetBaseCaptureTime()
	{
		if (baseCaptureTime == -1)
			return KOTH_Settings.GetBaseCaptureTime();
		else
			return baseCaptureTime;
	}

	int GetCrateLifeTime()
	{
		if (crateLifeTime == -1)
			return KOTH_Settings.GetCrateLifeTime();
		else
			return crateLifeTime;
	}
	
	string GetFlagClassname()
	{
		if (flagClassname == string.Empty)
			return KOTH_Settings.GetFlagClassname();
		else
			return flagClassname;
	}

	array<string> GetEnemies() 
	{
		if (enemies.Count() == 0)
			return KOTH_Settings.GetEnemies();
		else
			return enemies;
	}

	string GetLootCrate()
	{
		if (lootCrate == string.Empty)
			return KOTH_Settings.GetLootCrate();
		else
			return lootCrate;
	}

	array<string> GetLootSets() 
	{
		if (lootSets.Count() == 0)
			return KOTH_Settings.GetLootSets();
		else
			return lootSets;
	}

	int GetMaxEnemyCount()
	{
		if (maxEnemyCount == -1)
			return KOTH_Settings.GetMaxEnemyCount();
		else
			return maxEnemyCount;
	}

	int GetMaximumPlayers()
	{
		if (maximumPlayers == -1)
			return KOTH_Settings.GetMaximumPlayers();
		else
			return maximumPlayers;
	}

	int GetMinEnemyCount()
	{
		if (minEnemyCount == -1)
			return KOTH_Settings.GetMinEnemyCount();
		else
			return minEnemyCount;
	}

	int GetMinimumDeaths()
	{
		if (minimumDeaths == -1)
			return KOTH_Settings.GetMinimumDeaths();
		else
			return minimumDeaths;
	}

	int GetMinimumPlayers()
	{
		if (minimumPlayers == -1)
			return KOTH_Settings.GetMinimumPlayers();
		else
			return minimumPlayers;
	}

	array<ref KOTH_ObjectEntry> GetObjects() 
	{
		return objects;
	}

	float GetPlayerTimeMultiplier()
	{
		if (playerTimeMultiplier == -1)
			return KOTH_Settings.GetPlayerTimeMultiplier();
		else
			return playerTimeMultiplier;
	}

	int GetRewardCount() 
	{
		if (rewardCount == -1)
			return KOTH_Settings.GetRewardCount();
		else
			return rewardCount; 
	}

	float GetTimeDespawn()
	{
		if (timeDespawn == -1)
			return KOTH_Settings.GetTimeDespawn();
		else
			return timeDespawn;
	}

	float GetTimeGasDuration()
	{
		if (timeGasDuration == -1)
			return KOTH_Settings.GetTimeGasDuration();
		else
			return timeGasDuration;
	}

	float GetTimeLimit()
	{
		if (timeLimit == -1)
			return KOTH_Settings.GetTimeLimit();
		else
			return timeLimit;
	}

	float GetTimeStart()
	{
		if (timeStart == -1)
			return KOTH_Settings.GetTimeStart();
		else
			return timeStart;
	}

}