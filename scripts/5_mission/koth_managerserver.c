class KOTH_ManagerServer
{
	const float MAIN_THREAD_INTERVAL = 10.0;
	protected bool m_IsActive;

	protected ref array<ref KOTH_Event> m_ActiveEvents;
	protected ref array<ref KOTH_VehicleEvent> m_ActiveVehicleEvents;
	protected ref Timer m_SpawnDelayTimer;
	protected ref Timer m_MainTimer;
	protected ref Timer m_EventTimer;

	protected bool m_HasBasicMap = false;
	protected bool m_HasLBGroups = false;
	protected bool m_HasExpansion = false;
	protected bool m_HasVPPMap = false;

	#ifdef BASICMAP
	ref array<ref BasicMapMarker> m_BasicMapMarkers;
	#endif

	protected float m_TimeToNextEvent;
	protected float m_TimeSinceLastEvent;
	protected float m_TimeToNextVehicleEvent;
	protected float m_TimeSinceLastVehicleEvent;
	protected bool m_VehicleEventSpawnedThisSession;

	protected ref array<Man> m_Players;
	protected ref map<string, float> m_CooldownZones;
	protected ref map<string, float> m_VehicleCooldownZones;

	void KOTH_ManagerServer()
	{
		DetectAvailableMods();
		Init();
	}

	protected void DetectAvailableMods()
	{
		#ifdef BASICMAP
		m_HasBasicMap = true;
		#endif

		#ifdef LBGROUP_SYSTEM_NEW
		m_HasLBGroups = true;
		#endif

		#ifdef EXPANSIONMODNAVIGATION
		m_HasExpansion = true;
		#endif

		#ifdef VanillaPPMap
		m_HasVPPMap = true;
		#endif
	}

	protected void CleanupEvents()
	{
		if (m_ActiveEvents.Count() < 1 && m_ActiveVehicleEvents.Count() < 1)
		{
			m_EventTimer.Stop();
			return;
		}

		for (int i = m_ActiveEvents.Count() - 1; i >= 0; i--)
		{
			if (!m_ActiveEvents[i].IsActive())
			{
m_TimeToNextEvent = Math.RandomFloatInclusive(
    KOTH_Settings.GetMinTimeBetweenEvents(), 
    KOTH_Settings.GetMaxTimeBetweenEvents());

KOTH_Log.LogBasic(string.Format(
    "KOTH event ended. Next event will spawn in %.0f seconds (%.1f minutes).",
    m_TimeToNextEvent, m_TimeToNextEvent / 60.0));

				#ifdef BASICMAP
				if (m_HasBasicMap && m_BasicMapMarkers)
				{
					BasicMapCircleMarker marker = m_ActiveEvents[i].GetBasicMapMarker();
					if (marker && m_BasicMapMarkers.Find(marker) > -1)
					{
						m_BasicMapMarkers.RemoveItem(marker);
						UpdateBasicMapMarkers();
					}
				}
				#endif

				#ifdef LBGROUP_SYSTEM_NEW
				if (m_HasLBGroups)
				{
					LBServerMarker lbMarker = m_ActiveEvents[i].GetAdvancedGroupsMapMarker();
					if (lbMarker)
					{
						#ifdef LBmaster_Rework
						LBStaticMarkerManager.Get.RemoveServerMarker(lbMarker);
						#else
						LBStaticMarkerManager.Get().RemoveServerMarker(lbMarker);
						#endif
					}
				}
				#endif

				#ifdef EXPANSIONMODNAVIGATION
				if (m_HasExpansion)
				{
					m_ActiveEvents[i].RemoveExpansionServerMarker();
				}
				#endif
				
				#ifdef VanillaPPMap
				if (m_HasVPPMap)
				{
					m_ActiveEvents[i].RemoveVPPMapMarker();
				}
				#endif
				
				m_ActiveEvents.Remove(i);
			}
		}

		for (int j = m_ActiveVehicleEvents.Count() - 1; j >= 0; j--)
		{
			if (!m_ActiveVehicleEvents[j].IsActive())
			{
m_TimeSinceLastVehicleEvent = 0.0;
m_TimeToNextVehicleEvent = Math.RandomFloatInclusive(
    KOTH_Settings.GetVehicleMinTimeBetweenEvents(),
    KOTH_Settings.GetVehicleMaxTimeBetweenEvents());

KOTH_Log.LogBasic(string.Format(
    "Vehicle KOTH event ended. Next vehicle event will spawn in %.0f seconds (%.1f minutes).",
    m_TimeToNextVehicleEvent, m_TimeToNextVehicleEvent / 60.0));

				#ifdef BASICMAP
				if (m_HasBasicMap && m_BasicMapMarkers)
				{
					BasicMapCircleMarker vehicleMarker = m_ActiveVehicleEvents[j].GetBasicMapMarker();
					if (vehicleMarker && m_BasicMapMarkers.Find(vehicleMarker) > -1)
					{
						m_BasicMapMarkers.RemoveItem(vehicleMarker);
						UpdateBasicMapMarkers();
					}
				}
				#endif

				#ifdef LBGROUP_SYSTEM_NEW
				if (m_HasLBGroups)
				{
					LBServerMarker vehicleLbMarker = m_ActiveVehicleEvents[j].GetAdvancedGroupsMapMarker();
					if (vehicleLbMarker)
					{
						#ifdef LBmaster_Rework
						LBStaticMarkerManager.Get.RemoveServerMarker(vehicleLbMarker);
						#else
						LBStaticMarkerManager.Get().RemoveServerMarker(vehicleLbMarker);
						#endif
					}
				}
				#endif

				#ifdef EXPANSIONMODNAVIGATION
				if (m_HasExpansion)
				{
					m_ActiveVehicleEvents[j].RemoveExpansionServerMarker();
				}
				#endif
				
				#ifdef VanillaPPMap
				if (m_HasVPPMap)
				{
					m_ActiveVehicleEvents[j].RemoveVPPMapMarker();
				}
				#endif
				
				m_ActiveVehicleEvents.Remove(j);
			}
		}
	}

	protected string GetLocation(vector zonePosition)
	{
		string currentName, locationName, localizedName, worldName;
		vector currentPosition, locationPosition;
		float currentDistance;
		float lowestDistance = int.MAX;

		GetGame().GetWorldName(worldName);
		string configEntry = "CfgWorlds " + worldName + " Names";

		for (int i = 0; i < GetGame().ConfigGetChildrenCount(configEntry); i++)
		{
			GetGame().ConfigGetChildName(configEntry, i, currentName);
			string tempName = currentName;
			tempName.ToLower();
			if (!tempName.Contains("settlement") && !tempName.Contains("local") && !tempName.Contains("railroadstation") && !tempName.Contains("ruin") && !tempName.Contains("area") && !tempName.Contains("marine"))
				continue;

			array<float> positionFloatArray = new array<float>();
			GetGame().ConfigGetFloatArray(configEntry + " " + currentName + " position", positionFloatArray);
			currentPosition[0] = positionFloatArray[0];
			currentPosition[2] = positionFloatArray[1];
			currentPosition[1] = GetGame().SurfaceY(currentPosition[0], currentPosition[2]);

			currentDistance = vector.Distance(currentPosition, zonePosition);
			if (currentDistance < lowestDistance)
			{
				locationName = string.Format("%1", currentName.SubstringInverted(currentName, 0, currentName.IndexOf("_") + 1));
				lowestDistance = currentDistance;
				locationPosition = currentPosition;
			}
		}

		return locationName;
	}

	protected void Init()
	{
		bool kotHEnabled = KOTH_Settings.IsEnabled();
		
		if (kotHEnabled)
		{
			m_ActiveEvents = new array<ref KOTH_Event>;
			m_ActiveVehicleEvents = new array<ref KOTH_VehicleEvent>;
			m_CooldownZones = new map<string, float>;
			m_VehicleCooldownZones = new map<string, float>;
			m_EventTimer = new Timer();
			m_MainTimer = new Timer();
			m_SpawnDelayTimer = new Timer();
			m_Players = new array<Man>;
			m_VehicleEventSpawnedThisSession = false;

			if (KOTH_Settings.IsUseMapMarker() && (m_HasBasicMap || m_HasLBGroups || m_HasExpansion || m_HasVPPMap))
			{
				#ifdef BASICMAP
				if (m_HasBasicMap)
				{
					m_BasicMapMarkers = new array<ref BasicMapMarker>;
				}
				#endif
			}
			
			float timeSpawn = KOTH_Settings.GetTimeSpawn();
			
			if (timeSpawn > 0)
			{
				m_SpawnDelayTimer.Run(timeSpawn, this, "StartMainThread", NULL, false);
			}
			else
			{
				StartMainThread();
			}
		}
	}

	protected void StartMainThread()
	{
		if (!m_MainTimer)
			return;
		
		m_MainTimer.Run(MAIN_THREAD_INTERVAL, this, "Update", NULL, true);
	}

	protected bool CanStartEvent()
	{
		return m_ActiveEvents.Count() < KOTH_Settings.GetMaxEvents();
	}

	protected bool CanStartVehicleEvent()
	{
		return KOTH_Settings.IsVehicleKothEnabled() && m_ActiveVehicleEvents.Count() < KOTH_Settings.GetMaxVehicleEvents();
	}

	protected bool ShouldForceVehicleEvent()
	{
		if (!KOTH_Settings.IsVehicleGuaranteedPerRestart())
			return false;

		if (!m_VehicleEventSpawnedThisSession)
		{
			float sessionTime = m_TimeSinceLastVehicleEvent;
			float maxSessionTime = 9000.0; 
			
			if (sessionTime >= maxSessionTime)
			{
				KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Forcing spawn - no vehicle this session (%.0f seconds elapsed)", sessionTime));
				return true;
			}
		}
		
		return false;
	}
	
	protected bool IsVehicleSpawnTimeWindow()
	{
		int year, month, day, hour, minute, second;
		GetYearMonthDayUTC(year, month, day);
		GetHourMinuteSecondUTC(hour, minute, second);
		
		hour = hour + 1;
		if (hour >= 24)
			hour = hour - 24;
		
		if (hour >= 15 || hour <= 1)
		{
			KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Current time %1:%2 - SPAWN WINDOW ACTIVE", hour, minute));
			return true;
		}
		
		return false;
	}

	protected void StartEvent()
	{
		if (CanStartEvent())
		{
			StartRegularEvent();
		}
	}

	protected void StartRegularEvent()
	{
		bool zoneFound = false;
		int zoneAttempts = 0;
		KOTH_Zone kothZone;
		
		array<ref KOTH_Zone> availableZones = KOTH_Settings.GetZones();

		while (!zoneFound)
		{
			bool zoneConflict;
			kothZone = availableZones.GetRandomElement();

			if (m_CooldownZones.Contains(kothZone.GetName()))
				zoneConflict = true;

			if (!zoneConflict)
			{
				foreach (KOTH_Event checkEvent : m_ActiveEvents)
				{
					if (checkEvent.GetZone().GetName() == kothZone.GetName())
						zoneConflict = true;
				}
			}
			
			if (!zoneConflict)
			{
				zoneFound = true;
				continue;
			}

			zoneAttempts++;

			if (zoneAttempts > 10)
				return;
		}

		KOTH_Event kothEvent = new KOTH_Event(kothZone);
		m_ActiveEvents.Insert(kothEvent);
		
		string locationName = GetLocation(kothZone.GetPosition());
		kothEvent.SetLocationName(locationName);

		string notificationMessage = string.Format("New KOTH event [%1] starting", kothZone.GetName());

		if (KOTH_Settings.IsEstimateLocation())
			notificationMessage = string.Format("%1 at %2", notificationMessage, locationName);
		else
			notificationMessage = string.Format("%1...", notificationMessage);

		SendKOTHNotification(notificationMessage);

		if (KOTH_Settings.IsUseMapMarker())
		{
			CreateEventMarkers(kothEvent);
		}

		if (!m_EventTimer.IsRunning())
			m_EventTimer.Run(1, this, "UpdateEvents", NULL, true);

		KOTH_Log.LogBasic(string.Format("Started regular KOTH event: %1", kothZone.GetName()));
	}

	protected void StartVehicleEvent()
	{
		bool zoneFound = false;
		int zoneAttempts = 0;
		KOTH_Zone kothZone;
		
		array<ref KOTH_Zone> availableZones = KOTH_Settings.GetVehicleZones();

		if (availableZones.Count() == 0)
		{
			KOTH_Log.LogCritical("No vehicle zones configured!");
			return;
		}

		while (!zoneFound)
		{
			bool zoneConflict;
			kothZone = availableZones.GetRandomElement();

			if (m_VehicleCooldownZones.Contains(kothZone.GetName()))
				zoneConflict = true;

			if (!zoneConflict)
			{
				foreach (KOTH_VehicleEvent checkEvent : m_ActiveVehicleEvents)
				{
					if (checkEvent.GetZone().GetName() == kothZone.GetName())
						zoneConflict = true;
				}
			}
			
			if (!zoneConflict)
			{
				zoneFound = true;
				continue;
			}

			zoneAttempts++;

			if (zoneAttempts > 10)
				return;
		}

		KOTH_VehicleEvent kothVehicleEvent = new KOTH_VehicleEvent(kothZone);
		m_ActiveVehicleEvents.Insert(kothVehicleEvent);
		
		string locationName = GetLocation(kothZone.GetPosition());
		kothVehicleEvent.SetLocationName(locationName);

		string notificationMessage = string.Format("New VEHICLE KOTH event [%1] starting", kothZone.GetName());

		if (KOTH_Settings.IsEstimateLocation())
			notificationMessage = string.Format("%1 at %2", notificationMessage, locationName);
		else
			notificationMessage = string.Format("%1...", notificationMessage);

		SendKOTHNotification(notificationMessage);

		if (KOTH_Settings.IsUseMapMarker())
		{
			CreateVehicleEventMarkers(kothVehicleEvent);
		}

		if (!m_EventTimer.IsRunning())
			m_EventTimer.Run(1, this, "UpdateEvents", NULL, true);

		m_TimeSinceLastVehicleEvent = 0;
		m_TimeToNextVehicleEvent = Math.RandomFloatInclusive(KOTH_Settings.GetVehicleMinTimeBetweenEvents(), KOTH_Settings.GetVehicleMaxTimeBetweenEvents());
		m_VehicleEventSpawnedThisSession = true;

		KOTH_Log.LogBasic(string.Format("Started vehicle KOTH event: %1", kothZone.GetName()));
	}

	protected void CreateEventMarkers(KOTH_Event kothEvent)
	{
		#ifdef BASICMAP
		if (m_HasBasicMap && m_BasicMapMarkers)
		{
			BasicMapCircleMarker marker = kothEvent.CreateBasicMapMarker();
			if (marker)
			{
				m_BasicMapMarkers.Insert(marker);
				UpdateBasicMapMarkers();
			}
		}
		#endif

		#ifdef LBGROUP_SYSTEM_NEW
		if (m_HasLBGroups)
		{
			kothEvent.CreateAdvancedGroupsMapMarker();
		}
		#endif

		#ifdef EXPANSIONMODNAVIGATION
		if (m_HasExpansion)
		{
			kothEvent.CreateExpansionServerMarker();
		}
		#endif
		
		#ifdef VanillaPPMap
		if (m_HasVPPMap)
		{
			kothEvent.CreateVPPMapMarker();
		}
		#endif
	}

	protected void CreateVehicleEventMarkers(KOTH_VehicleEvent kothVehicleEvent)
	{
		#ifdef BASICMAP
		if (m_HasBasicMap && m_BasicMapMarkers)
		{
			BasicMapCircleMarker marker = kothVehicleEvent.CreateBasicMapMarker();
			if (marker)
			{
				m_BasicMapMarkers.Insert(marker);
				UpdateBasicMapMarkers();
			}
		}
		#endif

		#ifdef LBGROUP_SYSTEM_NEW
		if (m_HasLBGroups)
		{
			kothVehicleEvent.CreateAdvancedGroupsMapMarker();
		}
		#endif

		#ifdef EXPANSIONMODNAVIGATION
		if (m_HasExpansion)
		{
			kothVehicleEvent.CreateExpansionServerMarker();
		}
		#endif
		
		#ifdef VanillaPPMap
		if (m_HasVPPMap)
		{
			kothVehicleEvent.CreateVPPMapMarker();
		}
		#endif
	}

	protected array<Man> SyncPlayers(KOTH_Event targetEvent)
	{
		array<Man> eventPlayers = new array<Man>;

		foreach (Man player : m_Players)
		{
			if (player && player.IsAlive() && vector.Distance(targetEvent.GetPosition(), player.GetPosition()) <= targetEvent.GetRadius())
			{
				eventPlayers.Insert(player);
			}
		}

		return eventPlayers;
	}

	protected array<Man> SyncVehiclePlayers(KOTH_VehicleEvent targetEvent)
	{
		array<Man> eventPlayers = new array<Man>;

		foreach (Man player : m_Players)
		{
			if (player && player.IsAlive() && vector.Distance(targetEvent.GetPosition(), player.GetPosition()) <= targetEvent.GetRadius())
			{
				eventPlayers.Insert(player);
			}
		}

		return eventPlayers;
	}

	protected void Update()
	{
		m_TimeSinceLastEvent += MAIN_THREAD_INTERVAL;
		m_TimeSinceLastVehicleEvent += MAIN_THREAD_INTERVAL;
		
		UpdateCooldowns(MAIN_THREAD_INTERVAL);
		UpdateVehicleCooldowns(MAIN_THREAD_INTERVAL);
		
		UpdatePlayers();
		int playerCount = m_Players.Count();
		int minPlayerCount = KOTH_Settings.GetMinPlayerCount();

		if (m_TimeToNextEvent == 0)
		{
			m_TimeToNextEvent = Math.RandomFloatInclusive(KOTH_Settings.GetMinTimeBetweenEvents(), KOTH_Settings.GetMaxTimeBetweenEvents());
			KOTH_Log.LogBasic(string.Format("Next regular KOTH event in %.0f seconds (%.1f minutes)", m_TimeToNextEvent, m_TimeToNextEvent / 60.0));
		}
		
		if (m_TimeSinceLastEvent >= m_TimeToNextEvent)
		{
			if (playerCount >= minPlayerCount)
			{
				StartEvent();
			}
			else
			{
				m_TimeSinceLastEvent = m_TimeToNextEvent - 60.0;
			}
		}

		if (KOTH_Settings.IsVehicleKothEnabled())
		{
			bool isSpawnWindow = IsVehicleSpawnTimeWindow();
			bool shouldForce = ShouldForceVehicleEvent();
			
			if ((isSpawnWindow && !m_VehicleEventSpawnedThisSession) || shouldForce)
			{
				if (m_TimeToNextVehicleEvent == 0)
				{
					if (shouldForce)
					{
						m_TimeToNextVehicleEvent = 60.0;
						KOTH_Log.LogBasic("Vehicle KOTH: FORCING spawn soon (guaranteed system)");
					}
					else
					{
						m_TimeToNextVehicleEvent = Math.RandomFloatInclusive(KOTH_Settings.GetVehicleMinTimeBetweenEvents(), KOTH_Settings.GetVehicleMaxTimeBetweenEvents());
						KOTH_Log.LogBasic(string.Format("Next vehicle KOTH event in %.0f seconds (%.1f minutes)", m_TimeToNextVehicleEvent, m_TimeToNextVehicleEvent / 60.0));
					}
				}
				
				if (m_TimeSinceLastVehicleEvent >= m_TimeToNextVehicleEvent)
				{
					if (playerCount >= minPlayerCount && CanStartVehicleEvent())
					{
						StartVehicleEvent();
					}
					else
					{
						m_TimeSinceLastVehicleEvent = 0.0;
						m_TimeToNextVehicleEvent = 0.0;
					}
				}
			}
		}
	}

	protected void UpdateEvents()
	{
		CleanupEvents();
		UpdatePlayers();

		for (int i = 0; i < m_ActiveEvents.Count(); i++)
		{
			if (m_ActiveEvents[i].IsActive())
			{
				m_ActiveEvents[i].UpdatePlayers(SyncPlayers(m_ActiveEvents[i]));
				m_ActiveEvents[i].Update();
			}
		}

		for (int j = 0; j < m_ActiveVehicleEvents.Count(); j++)
		{
			if (m_ActiveVehicleEvents[j].IsActive())
			{
				m_ActiveVehicleEvents[j].UpdatePlayers(SyncVehiclePlayers(m_ActiveVehicleEvents[j]));
				m_ActiveVehicleEvents[j].Update();
			}
		}
	}

	protected void UpdatePlayers()
	{
		GetGame().GetPlayers(m_Players);
	}

	protected void UpdateCooldowns(float deltaTime)
	{
		array<string> zonesToRemove = new array<string>;
		
		foreach (string zoneName, float zoneTime : m_CooldownZones)
		{
			if (zoneTime - deltaTime <= 0)
				zonesToRemove.Insert(zoneName);
			else
				m_CooldownZones.Set(zoneName, zoneTime - deltaTime);
		}
		
		foreach (string zone : zonesToRemove)
		{
			m_CooldownZones.Remove(zone);
		}
	}

	protected void UpdateVehicleCooldowns(float deltaTime)
	{
		array<string> zonesToRemove = new array<string>;
		
		foreach (string zoneName, float zoneTime : m_VehicleCooldownZones)
		{
			if (zoneTime - deltaTime <= 0)
				zonesToRemove.Insert(zoneName);
			else
				m_VehicleCooldownZones.Set(zoneName, zoneTime - deltaTime);
		}
		
		foreach (string zone : zonesToRemove)
		{
			m_VehicleCooldownZones.Remove(zone);
		}
	}

	void AddCooldown(string zone)
	{
		if (KOTH_Settings.GetTimeZoneCooldown() <= 0)
			return;
		
		if (!m_CooldownZones.Contains(zone))
			m_CooldownZones.Insert(zone, KOTH_Settings.GetTimeZoneCooldown());
	}

	void AddVehicleCooldown(string zone)
	{
		if (KOTH_Settings.GetTimeZoneCooldown() <= 0)
			return;
		
		if (!m_VehicleCooldownZones.Contains(zone))
			m_VehicleCooldownZones.Insert(zone, KOTH_Settings.GetTimeZoneCooldown());
	}

	void SendKOTHNotification(string message)
	{
		if (!KOTH_Settings.IsUseNotifications())
			return;
		
		UpdatePlayers();

		for (int i = 0; i < m_Players.Count(); ++i)
		{
			NotificationSystem.SendNotificationToPlayerExtended(m_Players[i], 5, "King of the Hill", message, "set:dayz_gui image:icon_flag");
		}
	}

	void UpdateBasicMapMarkers(PlayerIdentity target = NULL)
	{
		#ifdef BASICMAP
		if (m_HasBasicMap && m_BasicMapMarkers)
			BasicMap().SetMarkersRemote("KOTH-SAVIOR", m_BasicMapMarkers, target);
		#endif
	}

	bool HasBasicMap() { return m_HasBasicMap; }
	bool HasLBGroups() { return m_HasLBGroups; }
	bool HasExpansion() { return m_HasExpansion; }
	bool HasVPPMap() { return m_HasVPPMap; }

	array<ref KOTH_Event> GetActiveEvents() { return m_ActiveEvents; }
	array<ref KOTH_VehicleEvent> GetActiveVehicleEvents() { return m_ActiveVehicleEvents; }
}