enum eKOTHEndCases
{
	WIN = 0,
	FAIL,
	TIMEOUT
}

enum eKOTHState
{
	START = 0,
	PAUSED,
	RUNNING,
	WON,
	FAILED
}

enum eKOTHVehicleState
{
	START = 0,
	PAUSED,
	RUNNING,
	WON,
	FAILED
}

class KOTH_Event
{
	protected bool m_IsActive;
	protected bool m_IsFinished;
	protected bool m_IsRunning;

	protected Particle m_ParticleSmoke;

	protected float m_Progress;
	protected float m_TimeElapsed;
	protected float m_LastSyncedProgress = -1;

	protected int m_Fatalities;
	protected int m_LastPlayerCount;
	protected int m_Radius;
	protected int m_State;

	protected string m_Name;
	protected string m_LocationName;

	protected vector m_Position;

	protected ItemBase m_Crate;

	protected ref array<EntityAI> m_Enemies;
	protected ref array<Man> m_PlayersInZone;
	protected ref KOTH_Zone m_Zone;

	protected ref Timer m_DeleteTimer;
	protected ref Timer m_CrateDeleteTimer;

	protected KOTH_Particle m_Particle;
	protected MissionServer m_Mission;

	protected ref array<Object> m_Objects;

	#ifdef BASICMAP
	ref BasicMapCircleMarker m_BasicMapMarker;
	#endif

	#ifdef LBGROUP_SYSTEM_NEW
	ref LBServerMarker m_AdvancedGroupsMapMarker;
	#endif

	#ifdef EXPANSIONMODNAVIGATION
	ExpansionMarkerData m_ExpansionServerMarker;
	ExpansionMarkerModule m_ExpansionMarkerModule;
	#endif

	#ifdef VanillaPPMap
	ref MarkerInfo m_VPPMapMarker;
	#endif

	void KOTH_Event(KOTH_Zone zone)
	{
		m_LastPlayerCount = 0;
		m_Zone = zone;
		m_Objects = new array<Object>;
		Init();
	}

void ~KOTH_Event()
{
	if (m_Particle)
		m_Particle.DeleteSafe();

	if (m_CrateDeleteTimer)
		m_CrateDeleteTimer.Stop();

	SyncProgressToPlayers(false, 0.0);
	RemoveObjects();
}

	protected void SyncProgressToPlayers(bool inZone, float progress)
	{
		if (!GetGame().IsDedicatedServer() || !KOTH_Settings.IsUseProgressHUD())
			return;

		foreach (Man player : m_PlayersInZone)
		{
			if (player && player.GetIdentity())
			{
				GetGame().RPCSingleParam(player, eKOTH.RPC_KOTH_PROGRESS_SYNC, new Param2<bool, float>(inZone, progress), true, player.GetIdentity());
			}
		}

		if (!inZone)
		{
			array<Man> allPlayers = new array<Man>;
			GetGame().GetPlayers(allPlayers);
			
			foreach (Man allPlayer : allPlayers)
			{
				if (allPlayer && allPlayer.GetIdentity())
				{
					GetGame().RPCSingleParam(allPlayer, eKOTH.RPC_KOTH_PROGRESS_SYNC, new Param2<bool, float>(false, 0.0), true, allPlayer.GetIdentity());
				}
			}
		}
	}

	protected void CleanupEvent()
	{
		m_IsFinished = true;
		m_IsRunning = false;

		string locationMessage;
		if (KOTH_Settings.IsEstimateLocation())
			locationMessage = string.Format(" at %1", m_LocationName);

		m_Mission.GetKOTHManager().SendKOTHNotification(string.Format("Nobody cared about [%1]%2 enough to fight for it!", m_Name, locationMessage));

		RemoveEnemies();
		SetState(eKOTHState.FAILED);
		StopEvent();
	}

	protected void CreateContaminatedArea()
	{
		if (m_Zone.GetTimeGasDuration() > 0)
		{
			KOTH_ContaminatedArea area = KOTH_ContaminatedArea.Cast(GetGame().CreateObjectEx("KOTH_ContaminatedArea", m_Position, ECE_NOLIFETIME, RF_DEFAULT));
			area.SetAreaLifetime(m_Zone.GetTimeGasDuration());
		}
	}

	protected void CreateEnemies()
	{
		int enemyCount = Math.RandomIntInclusive(m_Zone.GetMinEnemyCount(), m_Zone.GetMaxEnemyCount());
		
		for (int i = 0; i < enemyCount; i++)
		{
			EntityAI entity;
			float radius = Math.RandomFloatInclusive(2.0, m_Radius);
			vector position = m_Position + (vector.RandomDir2D() * radius);
			position[1] = GetGame().SurfaceY(position[0], position[2]);

			entity = EntityAI.Cast(GetGame().CreateObjectEx(m_Zone.GetEnemies().GetRandomElement(), position, ECE_PLACE_ON_SURFACE|ECE_INITAI, RF_DEFAULT));

			if (entity)
				m_Enemies.Insert(entity);
		}
	}

protected void CreateReward()
{
	m_Crate = ItemBase.Cast(GetGame().CreateObjectEx(m_Zone.GetLootCrate(), m_Position, ECE_PLACE_ON_SURFACE, RF_DEFAULT));

	if (m_Crate)
	{
		m_Crate.SetTakeable(false);
		m_Crate.LockToParent();
		KOTH_Log.LogBasic(string.Format("KOTH Crate spawned and locked at %1", m_Position.ToString()));
	}

	array<ref KOTH_LootSet> rewardSets = KOTH_Loot.GetDesiredRewards(m_Zone.GetLootSets());
	for (int i = 0; i < m_Zone.GetRewardCount(); i++)
	{
		array<ref KOTH_LootItem> items = rewardSets.GetRandomElement().GetItems();
		foreach (KOTH_LootItem item : items)
		{
			SpawnItem(m_Crate, item);
		}
	}

	if (m_Zone.GetCrateLifeTime() > 0 && m_Crate)
	{
		KOTH_Log.LogBasic(string.Format("KOTH Crate will despawn in %1 seconds", m_Zone.GetCrateLifeTime()));
		
		if (!m_CrateDeleteTimer)
			m_CrateDeleteTimer = new Timer();
		
		m_CrateDeleteTimer.Run(m_Zone.GetCrateLifeTime(), this, "DeleteCrate", NULL, false);
	}
}

protected void DeleteCrate()
{
	if (m_Crate)
	{
		KOTH_Log.LogBasic(string.Format("Deleting crate from zone: %1", m_Name));
		GetGame().ObjectDelete(m_Crate);
		m_Crate = null;
	}
}

	protected void FailEvent()
	{
		m_IsFinished = true;
		m_IsRunning = false;

		string locationMessage;
		if (KOTH_Settings.IsEstimateLocation())
			locationMessage = string.Format(" at %1", m_LocationName);

		m_Mission.GetKOTHManager().SendKOTHNotification(string.Format("The fight for [%1]%2 was disappointing...", m_Name, locationMessage));
		m_Mission.GetKOTHManager().AddCooldown(m_Zone.GetName());
		
		if (KOTH_Settings.IsPunishLoss())
			CreateContaminatedArea();
		
		SetState(eKOTHState.FAILED);
		StopEvent();
	}

	protected void Init()
	{
		if (!Class.CastTo(m_Mission, GetGame().GetMission()))
			return;

		#ifdef EXPANSIONMODNAVIGATION
		CF_Modules<ExpansionMarkerModule>.Get(m_ExpansionMarkerModule);
		#endif

		m_Name = m_Zone.GetName();
		m_Position = m_Zone.GetPosition();
		m_Radius = m_Zone.GetRadius();
		m_IsActive = true;

		m_PlayersInZone = new array<Man>;
		m_Enemies = new array<EntityAI>;
		m_TimeElapsed = 0;

		m_Particle = KOTH_Particle.Cast(GetGame().CreateObjectEx("KOTH_Particle", m_Position, ECE_PLACE_ON_SURFACE, RF_DEFAULT));
		m_Particle.SetKOTHParticleState(eKOTHParticleState.START);

		CreateEnemies();

		array<ref KOTH_ObjectEntry> objects = m_Zone.GetObjects();
		foreach (KOTH_ObjectEntry object : objects)
		{
			SpawnObject(object);
		}
	}

	protected void PlaySmokeParticle(int particleID)
	{
		if (m_ParticleSmoke)
			m_ParticleSmoke.Stop();
		
		m_ParticleSmoke = ParticleManager.GetInstance().PlayInWorld(particleID, m_Position);
	}

	protected void RemoveEnemies()
	{
		foreach (EntityAI enemy : m_Enemies)
		{
			if (enemy)
				enemy.DeleteSafe();
		}
	}

	protected void RemoveObjects()
	{
		foreach (Object object : m_Objects)
		{
			if (object)
				GetGame().ObjectDelete(object);
		}
	}

	protected void SetActive(bool state)
	{
		m_IsActive = state;
	}

	protected void SetState(int state)
	{
		switch (state)
		{
			case eKOTHState.START:
				if (m_Particle)
					m_Particle.SetKOTHParticleState(eKOTHParticleState.START);
			break;
			
			case eKOTHState.PAUSED:
				if (m_Particle)
					m_Particle.SetKOTHParticleState(eKOTHParticleState.START);
			break;
			
			case eKOTHState.RUNNING:
				if (m_Particle)
					m_Particle.SetKOTHParticleState(eKOTHParticleState.LOOP);
			break;
			
			case eKOTHState.WON:
				if (m_Particle)
					m_Particle.SetKOTHParticleState(eKOTHParticleState.WIN);
			break;
			
			case eKOTHState.FAILED:
				if (m_Particle)
					m_Particle.SetKOTHParticleState(eKOTHParticleState.FAIL);
			break;

			default:
			break;
		}

		#ifdef BASICMAP
		m_Mission.GetKOTHManager().UpdateBasicMapMarkers();
		#endif

		m_State = state;
	}

	protected bool ShouldEnd(out int endCase)
	{
		if (m_IsFinished)
			return false;

		if (!m_IsRunning)
		{
			if (m_TimeElapsed > m_Zone.GetTimeDespawn())
			{
				endCase = eKOTHEndCases.TIMEOUT;
				return true;
			}
			else
				return false;
		}

		if (m_Progress >= 100)
		{
			if (m_Fatalities >= m_Zone.GetMinimumDeaths())
				endCase = eKOTHEndCases.WIN;
			else
				endCase = eKOTHEndCases.FAIL;

			return true;
		}
		else if (m_Zone.GetTimeLimit() > 0 && m_TimeElapsed >= m_Zone.GetTimeLimit())
		{
			endCase = eKOTHEndCases.FAIL;
			return true;
		}
		else
		{
			return false;
		}
	}

	protected bool ShouldStart()
	{
		if (m_IsRunning)
			return false;
		
		return !m_IsFinished && m_PlayersInZone.Count() >= m_Zone.GetMinimumPlayers() && m_TimeElapsed >= m_Zone.GetTimeStart();
	}

	protected void SpawnItem(ItemBase parent, KOTH_LootItem lootItem, bool isAttachment = false)
	{
		ItemBase newItem;
		Weapon_Base weapon;
		Magazine_Base mag;
		bool isMagazine;
		
		if (!parent)
			return;
		
		bool parentIsWeapon;
		parentIsWeapon = Class.CastTo(weapon, parent);
		
		if (isAttachment)
		{
			if (parentIsWeapon)
			{
				newItem = weapon.SpawnAttachmentKOTH(lootItem.GetName(), isMagazine);
			}
			else
			{
				newItem = ItemBase.Cast(parent.GetInventory().CreateAttachment(lootItem.GetName()));
			}
		}
		else
		{
			newItem = ItemBase.Cast(parent.GetInventory().CreateInInventory(lootItem.GetName()));
		}

		if (!newItem)
			return;

		if (lootItem.GetQuantity() > -1)
		{
			if (isMagazine && Class.CastTo(mag, newItem))
			{
				mag.ServerSetAmmoCount(lootItem.GetQuantity());
			}
			else
			{
				newItem.SetQuantity(lootItem.GetQuantity());
			}
		}

		if (isMagazine)
			return;

		array<ref KOTH_LootItem> attachmentsArray = lootItem.GetAttachments();
		foreach (KOTH_LootItem attachment : attachmentsArray)
		{
			SpawnItem(newItem, attachment, true);
		}

		array<ref KOTH_LootItem> cargoArray = lootItem.GetCargo();
		foreach (KOTH_LootItem cargo : cargoArray)
		{
			SpawnItem(newItem, cargo);
		}
	}

	protected void SpawnObject(KOTH_ObjectEntry objectEntry)
	{
		vector newOrientation, normal, position, orientation;
		
		orientation = objectEntry.GetOrientation();
		
		if (objectEntry.IsAbsolutePlacement())
			position = objectEntry.GetPosition();
		else
			position = m_Position + objectEntry.GetPosition();
		
		if (objectEntry.IsAlignToTerrain())
		{
			vector surfaceOrientation = GetGame().GetSurfaceOrientation(position[0], position[2]);
			orientation += surfaceOrientation;
		}

		Object object = GetGame().CreateObjectEx(objectEntry.GetClassName(), position, ECE_CREATEPHYSICS|ECE_UPDATEPATHGRAPH);
		
		if (!object)
			return;
		
		m_Objects.Insert(object);

		if (objectEntry.IsPlaceOnSurface())
		{
			vector min_max[2];
			object.ClippingInfo(min_max);

			float offset = (min_max[1][1] - min_max[0][1]) / 2;

			position[1] = GetGame().SurfaceY(position[0], position[2]) + offset;
		}

		object.SetPosition(position);
		object.SetOrientation(orientation);
	}

	protected void StartEvent()
	{
		string locationMessage;
		if (KOTH_Settings.IsEstimateLocation())
			locationMessage = string.Format(" at %1", m_LocationName);

		m_Mission.GetKOTHManager().SendKOTHNotification(string.Format("The fight for [%1]%2 has begun!", m_Name, locationMessage));

		m_IsRunning = true;
		SetState(eKOTHState.PAUSED);
	}

	protected void StopEvent()
	{
		m_DeleteTimer = new Timer();
		m_DeleteTimer.Run(60, this, "SetActive", new Param1<bool>(false), false);
	}

protected void WinEvent()
{
	m_IsFinished = true;
	m_IsRunning = false;

	string playerNames = "";
	string name;

	for (int i = 0; i < m_PlayersInZone.Count(); i++)
	{
		name = "";
		if (m_PlayersInZone[i] && m_PlayersInZone[i].GetIdentity())
			name = m_PlayersInZone[i].GetIdentity().GetName();

		if (i > 0)
			playerNames += ", ";

		playerNames += name;
	}

	string fatalities = "";
	if (m_Fatalities == 1)
		fatalities = string.Format(" %1 person gave their life in the attempt to capture it.", m_Fatalities);
	else if (m_Fatalities > 1)
		fatalities = string.Format(" %1 people gave their lives in the attempt to capture it.", m_Fatalities);

	string locationMessage;
	if (KOTH_Settings.IsEstimateLocation())
		locationMessage = string.Format(" at %1", m_LocationName);
	
	m_Mission.GetKOTHManager().SendKOTHNotification(string.Format("CAPTURE COMPLETE! %1 captured the hill [%2]%3!%4", playerNames, m_Name, locationMessage, fatalities));
	
	m_Mission.GetKOTHManager().AddCooldown(m_Zone.GetName());

	CreateReward();
	
	SetState(eKOTHState.WON);
	
	GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(PlayKOTHWinSound, 500, false, 500);
	
	StopEvent();
}

protected void PlayKOTHWinSound(int radius)
{
	if (!GetGame().IsDedicatedServer())
		return;
	
	array<Man> allPlayers = new array<Man>;
	GetGame().GetPlayers(allPlayers);
	
	foreach (Man player : allPlayers)
	{
		if (player && player.IsAlive() && player.GetIdentity())
		{
			float distance = vector.Distance(m_Position, player.GetPosition());
			if (distance <= radius)
			{
				if (player && player.GetIdentity())
				{
					GetGame().RPCSingleParam(player, eKOTH.RPC_KOTH_PLAY_ALARM_SOUND, new Param1<vector>(m_Position), true, player.GetIdentity());
				}
			}
		}
	}
}

	#ifdef BASICMAP
	BasicMapCircleMarker CreateBasicMapMarker()
	{
		if (!m_BasicMapMarker)
		{
			m_BasicMapMarker = new BasicMapCircleMarker(string.Format("[KOTH] %1", m_Zone.GetName()), m_Zone.GetPosition(), "BasicMap\\gui\\images\\flag.paa", {255,255,0});
			m_BasicMapMarker.SetShowCenterMarker(true);
			m_BasicMapMarker.SetRadius(m_Zone.GetRadius());
			m_BasicMapMarker.SetCanEdit(false);
		}
		
		return m_BasicMapMarker;
	}

	BasicMapCircleMarker GetBasicMapMarker()
	{
		if (m_BasicMapMarker)
			return m_BasicMapMarker;
		
		return NULL;
	}
	#endif

	#ifdef LBGROUP_SYSTEM_NEW
	void CreateAdvancedGroupsMapMarker()
	{
		if (!m_AdvancedGroupsMapMarker)
		{
			#ifdef LBmaster_Rework
			m_AdvancedGroupsMapMarker = LBStaticMarkerManager.Get.AddTempServerMarker(string.Format("[KOTH] %1", m_Zone.GetName()), m_Zone.GetPosition(), "LBmaster_Groups\\gui\\icons\\flag.paa", ARGB(255,255,255,0), false, false);
			#else
			m_AdvancedGroupsMapMarker = LBStaticMarkerManager.Get().AddTempServerMarker(string.Format("[KOTH] %1", m_Zone.GetName()), m_Zone.GetPosition(), "LBmaster_Groups\\gui\\icons\\flag.paa", ARGB(255,255,255,0), false, false);
			#endif
		}
	}

	LBServerMarker GetAdvancedGroupsMapMarker()
	{
		if (m_AdvancedGroupsMapMarker)
			return m_AdvancedGroupsMapMarker;
		
		return NULL;
	}
	#endif

	#ifdef EXPANSIONMODNAVIGATION
	void CreateExpansionServerMarker()
	{
		if (!CF_Modules<ExpansionMarkerModule>.Get(m_ExpansionMarkerModule))
			return;

		m_ExpansionServerMarker = m_ExpansionMarkerModule.CreateServerMarker("[KOTH] %1", "Territory", m_Zone.GetPosition(), ARGB(255,255,255,0), true);
	}

	void RemoveExpansionServerMarker()
	{
		if (!m_ExpansionServerMarker)
			return;
		
		m_ExpansionMarkerModule.RemoveServerMarker(m_ExpansionServerMarker.GetUID());
	}
	#endif

	#ifdef VanillaPPMap
	void CreateVPPMapMarker()
	{
		if (!GetGame().IsDedicatedServer())
			return;
		
		m_VPPMapMarker = new MarkerInfo("KOTH", "KOTH\\Icon\\KOTHFlag_ca.paa", Vector(255,255,0), Vector(m_Position[0], 0, m_Position[2]), true, true);
		m_Mission.AddKOTHMarkerVPP(m_VPPMapMarker);
		UpdateVPPMapMarker();
	}

	protected void UpdateVPPMapMarker()
	{
		if (!GetGame().IsDedicatedServer())
			return;

		m_Mission.UpdateKOTHMarkersVPP();
	}

	void RemoveVPPMapMarker()
	{
		if (!GetGame().IsDedicatedServer())
			return;

		m_Mission.RemoveKOTHMarkerVPP(m_VPPMapMarker);
		UpdateVPPMapMarker();
	}
	#endif
	
	string GetLocationName()
	{
		return m_LocationName;
	}

	vector GetPosition()
	{
		return m_Position;
	}

	int GetRadius()
	{
		return m_Radius;
	}

	KOTH_Zone GetZone()
	{
		return m_Zone;
	}

	bool IsActive()
	{
		return m_IsActive;
	}

	void SetLocationName(string locationName)
	{
		m_LocationName = locationName;
	}

	void Update()
	{
		if (ShouldStart())
		{
			StartEvent();
			return;
		}

		int endCase;
		if (ShouldEnd(endCase))
		{
			switch (endCase)
			{
				case eKOTHEndCases.WIN:
					WinEvent();
					break;
				case eKOTHEndCases.FAIL:
					FailEvent();
					break;
				case eKOTHEndCases.TIMEOUT:
					CleanupEvent();
					break;
			}
		}

		if (m_IsRunning)
		{
			int playerCount = m_PlayersInZone.Count();
			float amountToAdd = 100 / m_Zone.GetBaseCaptureTime();

			if (playerCount > 0 && m_LastPlayerCount == 0)
			{
				string firstPlayerName = "";
				if (m_PlayersInZone[0] && m_PlayersInZone[0].GetIdentity())
					firstPlayerName = m_PlayersInZone[0].GetIdentity().GetName();
				
				string locationMessage;
				if (KOTH_Settings.IsEstimateLocation())
					locationMessage = string.Format(" at %1", m_LocationName);
				
				m_Mission.GetKOTHManager().SendKOTHNotification(string.Format("%1 has started capturing [%2]%3!", firstPlayerName, m_Name, locationMessage));
			}

			if (playerCount > 0)
			{
				if (m_Progress <= 100)
				{
					m_Progress += amountToAdd;
				}

				if (m_LastPlayerCount <= 0)
					SetState(eKOTHState.RUNNING);

				m_LastPlayerCount = playerCount;
			}
			else
			{
				if (m_Progress > 0)
					m_Progress -= amountToAdd;

				if (m_LastPlayerCount > 0)
					SetState(eKOTHState.PAUSED);

				m_LastPlayerCount = playerCount;
			}

			float roundedProgress = Math.Round(m_Progress * 10) / 10;
			if (Math.AbsFloat(roundedProgress - m_LastSyncedProgress) >= 0.1)
			{
				SyncProgressToPlayers(true, m_Progress);
				m_LastSyncedProgress = roundedProgress;
			}
		}

		m_TimeElapsed++;
	}

	void UpdatePlayers(array<Man> players)
	{
		int i;
		PlayerBase player;

		for (i = 0; i < m_PlayersInZone.Count(); i++)
		{
			if (players.Find(m_PlayersInZone[i]) < 0)
			{
				if (Class.CastTo(player, m_PlayersInZone[i]) && !player.IsAlive())
				{
					if (player.GetKillerKOTH() && vector.Distance(m_Position, player.GetKillerKOTH().GetPosition()) > (m_Radius * 2))
					{
						m_Progress -= (100 / m_Zone.GetBaseCaptureTime()) * 10;
					}

					m_Fatalities++;
				}

				if (m_PlayersInZone[i] && m_PlayersInZone[i].GetIdentity())
				{
					GetGame().RPCSingleParam(m_PlayersInZone[i], eKOTH.RPC_KOTH_PROGRESS_SYNC, new Param2<bool, float>(false, 0.0), true, m_PlayersInZone[i].GetIdentity());
				}

				m_PlayersInZone.RemoveItem(m_PlayersInZone[i]);
			}
		}

		for (i = 0; i < players.Count(); i++)
		{
			if (m_PlayersInZone.Find(players[i]) < 0)
			{
				m_PlayersInZone.Insert(players[i]);
				
				if (players[i] && players[i].GetIdentity())
				{
					GetGame().RPCSingleParam(players[i], eKOTH.RPC_KOTH_PROGRESS_SYNC, new Param2<bool, float>(true, m_Progress), true, players[i].GetIdentity());
				}
			}
		}
	}
}

class KOTH_VehicleEvent
{
	protected bool m_IsActive;
	protected bool m_IsFinished;
	protected bool m_IsRunning;

	protected Object m_GroundDecal;
    protected ref Timer m_DecalRemovalTimer;

	protected float m_Progress;
	protected float m_TimeElapsed;
	protected float m_LastSyncedProgress = -1;

	protected int m_Fatalities;
	protected int m_LastPlayerCount;
	protected int m_Radius;
	protected int m_State;

	protected string m_Name;
	protected string m_LocationName;

	protected vector m_Position;

	protected ItemBase m_Crate;
	protected Car m_Vehicle; 

	protected ref array<Man> m_PlayersInZone;
	protected ref KOTH_Zone m_Zone;

	protected ref Timer m_DeleteTimer;
	protected MissionServer m_Mission;

	protected ref array<Object> m_Objects;

	#ifdef BASICMAP
	ref BasicMapCircleMarker m_BasicMapMarker;
	#endif

	#ifdef LBGROUP_SYSTEM_NEW
	ref LBServerMarker m_AdvancedGroupsMapMarker;
	#endif

	#ifdef EXPANSIONMODNAVIGATION
	ExpansionMarkerData m_ExpansionServerMarker;
	ExpansionMarkerModule m_ExpansionMarkerModule;
	#endif

	#ifdef VanillaPPMap
    ref MarkerInfo m_VPPMapMarker;
	#endif

	void KOTH_VehicleEvent(KOTH_Zone zone)
	{
		m_LastPlayerCount = 0;
		m_Zone = zone;
		m_Objects = new array<Object>;
		Init();
	}

void ~KOTH_VehicleEvent()
{
    if (m_GroundDecal)
        GetGame().ObjectDelete(m_GroundDecal);
    
    if (m_DecalRemovalTimer)
        m_DecalRemovalTimer.Stop();

    SyncProgressToPlayers(false, 0.0);
    RemoveObjects();
}

	protected void SyncProgressToPlayers(bool inZone, float progress)
	{
		if (!GetGame().IsDedicatedServer() || !KOTH_Settings.IsUseProgressHUD())
			return;

		foreach (Man player : m_PlayersInZone)
		{
			if (player && player.GetIdentity())
			{
				GetGame().RPCSingleParam(player, eKOTH.RPC_KOTH_PROGRESS_SYNC, new Param2<bool, float>(inZone, progress), true, player.GetIdentity());
			}
		}

		if (!inZone)
		{
			array<Man> allPlayers = new array<Man>;
			GetGame().GetPlayers(allPlayers);
			
			foreach (Man allPlayer : allPlayers)
			{
				if (allPlayer && allPlayer.GetIdentity())
				{
					GetGame().RPCSingleParam(allPlayer, eKOTH.RPC_KOTH_PROGRESS_SYNC, new Param2<bool, float>(false, 0.0), true, allPlayer.GetIdentity());
				}
			}
		}
	}

	protected void CleanupEvent()
	{
		m_IsFinished = true;
		m_IsRunning = false;

		string locationMessage;
		if (KOTH_Settings.IsEstimateLocation())
			locationMessage = string.Format(" at %1", m_LocationName);

m_Mission.GetKOTHManager().AddVehicleCooldown(m_Zone.GetName());

KOTH_Log.LogBasic("Vehicle KOTH: Players won! Creating vehicle reward...");
		
		SetState(eKOTHVehicleState.FAILED);
		StopEvent();

		    if (m_GroundDecal)
    {
        m_DecalRemovalTimer = new Timer();
        m_DecalRemovalTimer.Run(600, this, "RemoveGroundDecal", NULL, false);
    }
	}

	protected void CreateContaminatedArea()
	{
		if (m_Zone.GetTimeGasDuration() > 0)
		{
			KOTH_ContaminatedArea area = KOTH_ContaminatedArea.Cast( GetGame().CreateObjectEx( "KOTH_ContaminatedArea", m_Position, ECE_NOLIFETIME, RF_DEFAULT ));
			area.SetAreaLifetime(m_Zone.GetTimeGasDuration());
		}
	}

protected void CreateVehicle()
{
    KOTH_Log.LogBasic("Vehicle KOTH: Starting CreateVehicle()");
    
    array<string> vehicleClassnames = KOTH_Settings.GetVehicleClassnames();
    
    if (!vehicleClassnames || vehicleClassnames.Count() == 0)
    {
        KOTH_Log.LogCritical("Vehicle KOTH: No vehicle classnames configured!");
        return;
    }

    string selectedVehicle = vehicleClassnames.GetRandomElement();
    KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Selected classname: %1", selectedVehicle));
    
    vector vehiclePos = m_Position;
    vehiclePos[1] = GetGame().SurfaceY(vehiclePos[0], vehiclePos[2]) + 0.5;

    Object vehicleObject = GetGame().CreateObjectEx(selectedVehicle, vehiclePos, ECE_PLACE_ON_SURFACE | ECE_UPDATEPATHGRAPH | ECE_CREATEPHYSICS, RF_DEFAULT);
    
    if (!vehicleObject)
    {
        KOTH_Log.LogCritical(string.Format("Vehicle KOTH: CreateObjectEx failed for: %1", selectedVehicle));
        return;
    }
    
    m_Vehicle = Car.Cast(vehicleObject);
    
    if (!m_Vehicle)
    {
        KOTH_Log.LogCritical(string.Format("Vehicle KOTH: Failed to cast to Car: %1", selectedVehicle));
        GetGame().ObjectDelete(vehicleObject); 
        return;
    }

    m_Vehicle.SetPosition(vehiclePos);
    m_Vehicle.SetOrientation(vector.Zero); 
    m_Vehicle.PlaceOnSurface();
    m_Vehicle.Update();
    m_Vehicle.SetSynchDirty();

    KOTH_Log.LogBasic("Vehicle KOTH: Using native vehicle persistence (KOTH persistence disabled)");

    EquipVehicle(m_Vehicle);
    CreateGroundDecal();

    KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Successfully spawned %1 at %2", selectedVehicle, m_Vehicle.GetPosition().ToString()));
}

protected void CreateGroundDecal()
{
    if (!m_Vehicle)
        return;
    
    vector vehiclePos = m_Vehicle.GetPosition();
    
    vector groundPos;
    groundPos[0] = vehiclePos[0];
    groundPos[2] = vehiclePos[2];
    groundPos[1] = GetGame().SurfaceY(groundPos[0], groundPos[2]) + 0.05; 
    
    m_GroundDecal = GetGame().CreateObjectEx("decal_heli_army", groundPos, ECE_NONE);
    
    if (m_GroundDecal)
    {
        m_GroundDecal.SetOrientation("0 0 0"); 
        
        KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Ground decal created at %1", groundPos.ToString()));
    }
}

	protected void EquipVehicle(Car vehicle)
	{
		if (!vehicle)
			return;

		KOTH_Log.LogBasic("Vehicle KOTH: Equipping vehicle with essential parts...");

		
		EntityAI battery = vehicle.GetInventory().CreateAttachment("CarBattery");
		if (battery)
		{
			KOTH_Log.LogBasic("Vehicle KOTH: Added battery");
		}

		EntityAI radiator = vehicle.GetInventory().CreateAttachment("CarRadiator");
		if (radiator)
		{
			KOTH_Log.LogBasic("Vehicle KOTH: Added radiator");
		}

		EntityAI sparkPlug = vehicle.GetInventory().CreateAttachment("SparkPlug");
		if (sparkPlug)
		{
			KOTH_Log.LogBasic("Vehicle KOTH: Added spark plug");
		}

		
		vehicle.GetInventory().CreateAttachment("HeadlightH7");
		vehicle.GetInventory().CreateAttachment("HeadlightH7");
		KOTH_Log.LogBasic("Vehicle KOTH: Added headlights");

	
		string vehicleType = vehicle.GetType();
		
		if (vehicleType.Contains("CivilianSedan"))
		{
		
			vehicle.GetInventory().CreateAttachment("CivSedanDoors_Driver");
			vehicle.GetInventory().CreateAttachment("CivSedanDoors_CoDriver");
			vehicle.GetInventory().CreateAttachment("CivSedanDoors_BackLeft");
			vehicle.GetInventory().CreateAttachment("CivSedanDoors_BackRight");
			
			vehicle.GetInventory().CreateAttachment("CivSedanHood");
			vehicle.GetInventory().CreateAttachment("CivSedanTrunk");
			KOTH_Log.LogBasic("Vehicle KOTH: Added CivilianSedan doors, hood and trunk");
		}
		else if (vehicleType.Contains("Sedan_02"))
		{
		
			vehicle.GetInventory().CreateAttachment("Sedan_02_Door_1_1");
			vehicle.GetInventory().CreateAttachment("Sedan_02_Door_1_2");
			vehicle.GetInventory().CreateAttachment("Sedan_02_Door_2_1");
			vehicle.GetInventory().CreateAttachment("Sedan_02_Door_2_2");

			vehicle.GetInventory().CreateAttachment("Sedan_02_Hood");
			vehicle.GetInventory().CreateAttachment("Sedan_02_Trunk");
			KOTH_Log.LogBasic("Vehicle KOTH: Added Sedan_02 doors, hood and trunk");
		}
		else if (vehicleType.Contains("Hatchback_02"))
		{

			vehicle.GetInventory().CreateAttachment("Hatchback_02_Door_1_1");
			vehicle.GetInventory().CreateAttachment("Hatchback_02_Door_1_2");
			vehicle.GetInventory().CreateAttachment("Hatchback_02_Door_2_1");
			vehicle.GetInventory().CreateAttachment("Hatchback_02_Door_2_2");

			vehicle.GetInventory().CreateAttachment("Hatchback_02_Hood");
			vehicle.GetInventory().CreateAttachment("Hatchback_02_Trunk");
			KOTH_Log.LogBasic("Vehicle KOTH: Added Hatchback_02 doors, hood and trunk");
		}

		string wheelType = "CivSedanWheel"; 
		
		if (vehicleType.Contains("CivilianSedan"))
		{
			wheelType = "CivSedanWheel";
		}
		else if (vehicleType.Contains("Sedan_02"))
		{
			wheelType = "Sedan_02_Wheel";
		}
		else if (vehicleType.Contains("Hatchback_02"))
		{
			wheelType = "Hatchback_02_Wheel";
		}
		else if (vehicleType.Contains("Niva"))
		{
			wheelType = "NivaWheel";
		}
		
		KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Vehicle type: %1, using wheels: %2", vehicleType, wheelType));
		

		for (int i = 0; i < 4; i++)
		{
			EntityAI wheel = vehicle.GetInventory().CreateAttachment(wheelType);
			if (wheel)
			{
				KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Added wheel %1/4", i + 1));
			}
		}


		vehicle.Fill(CarFluid.FUEL, vehicle.GetFluidCapacity(CarFluid.FUEL));
		KOTH_Log.LogBasic("Vehicle KOTH: Filled fuel tank");

		vehicle.Fill(CarFluid.COOLANT, vehicle.GetFluidCapacity(CarFluid.COOLANT));
		KOTH_Log.LogBasic("Vehicle KOTH: Filled coolant");

		vehicle.SetSynchDirty();
		vehicle.Synchronize();

		KOTH_Log.LogBasic("Vehicle KOTH: Vehicle equipped and ready!");
	}

	protected void CreateVehicleReward()
	{
		if (!m_Vehicle)
		{
			KOTH_Log.LogCritical("Vehicle KOTH: Cannot create vehicle reward - vehicle is NULL!");
			return;
		}

		KOTH_Log.LogBasic("Vehicle KOTH: Starting CreateVehicleReward()");

		array<string> lootSets = KOTH_Settings.GetVehicleLootSets();
		if (lootSets.Count() == 0)
		{
			KOTH_Log.LogBasic("Vehicle KOTH: No vehicle loot sets found, using regular loot sets");
			lootSets = KOTH_Settings.GetLootSets();
		}

		if (lootSets.Count() == 0)
		{
			KOTH_Log.LogCritical("Vehicle KOTH: No loot sets available!");
			return;
		}

		KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Using %1 loot sets", lootSets.Count()));

		array<ref KOTH_LootSet> rewardSets = KOTH_Loot.GetDesiredRewards(lootSets);
		int rewardCount = KOTH_Settings.GetVehicleRewardCount();
		
		KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Creating %1 reward sets", rewardCount));

		for (int i = 0; i < rewardCount; i++)
		{
			if (rewardSets.Count() > 0)
			{
				KOTH_LootSet selectedSet = rewardSets.GetRandomElement();
				KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Selected loot set: %1", selectedSet.GetName()));
				
				array<ref KOTH_LootItem> items = selectedSet.GetItems();
				foreach (KOTH_LootItem item : items)
				{
					SpawnItemInVehicle(m_Vehicle, item);
				}
			}
		}

		KOTH_Log.LogBasic("Vehicle KOTH: Vehicle reward creation completed");
	}

	protected void SpawnItemInVehicle(Car vehicle, KOTH_LootItem lootItem, bool isAttachment = false)
	{
		if (!vehicle)
		{
			KOTH_Log.LogCritical("Vehicle KOTH: Cannot spawn item - vehicle is NULL!");
			return;
		}

		ItemBase newItem;
		Weapon_Base weapon;
		Magazine_Base mag;
		bool isMagazine;
		
		KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Spawning item: %1", lootItem.GetName()));
		
		newItem = ItemBase.Cast(vehicle.GetInventory().CreateInInventory(lootItem.GetName()));

		if (!newItem)
		{
			KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Failed to spawn item in vehicle: %1", lootItem.GetName()));
			return;
		}

		KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Successfully spawned item: %1", lootItem.GetName()));

		if (lootItem.GetQuantity() > -1)
		{
			if (isMagazine && Class.CastTo(mag, newItem))
			{
				mag.ServerSetAmmoCount(lootItem.GetQuantity());
			}
			else
			{
				newItem.SetQuantity(lootItem.GetQuantity());
			}
		}

		if (isMagazine)
			return;

		bool parentIsWeapon = Class.CastTo(weapon, newItem);
		
		array<ref KOTH_LootItem> attachmentsArray = lootItem.GetAttachments();
		foreach (KOTH_LootItem attachment : attachmentsArray)
		{
			if (parentIsWeapon)
			{
				ItemBase weaponAttachment = weapon.SpawnAttachmentKOTH(attachment.GetName(), isMagazine);
				if (weaponAttachment && attachment.GetQuantity() > -1)
				{
					if (isMagazine && Class.CastTo(mag, weaponAttachment))
					{
						mag.ServerSetAmmoCount(attachment.GetQuantity());
					}
					else
					{
						weaponAttachment.SetQuantity(attachment.GetQuantity());
					}
				}
			}
			else
			{
				ItemBase itemAttachment = ItemBase.Cast(newItem.GetInventory().CreateAttachment(attachment.GetName()));
				if (itemAttachment && attachment.GetQuantity() > -1)
					itemAttachment.SetQuantity(attachment.GetQuantity());
			}
		}

		array<ref KOTH_LootItem> cargoArray = lootItem.GetCargo();
		foreach (KOTH_LootItem cargo : cargoArray)
		{
			SpawnItemInVehicle(vehicle, cargo);
		}
	}

	protected void FailEvent()
	{
		m_IsFinished = true;
		m_IsRunning = false;

		string locationMessage;
		if (KOTH_Settings.IsEstimateLocation())
			locationMessage = string.Format(" at %1", m_LocationName);

		m_Mission.GetKOTHManager().SendKOTHNotification(string.Format("The battle for the vehicle at [%1]%2 was disappointing...", m_Name, locationMessage));
		m_Mission.GetKOTHManager().AddVehicleCooldown(m_Zone.GetName());
		
		if (KOTH_Settings.IsPunishLoss())
			CreateContaminatedArea();
		
		KOTH_Log.LogBasic("Vehicle KOTH: Event failed - no vehicle reward");
		
		SetState(eKOTHVehicleState.FAILED);
		StopEvent();
	}

protected void Init()
{
    if (!Class.CastTo(m_Mission, GetGame().GetMission()))
        return;

    #ifdef EXPANSIONMODNAVIGATION
    CF_Modules<ExpansionMarkerModule>.Get(m_ExpansionMarkerModule);
    #endif

    m_Name = m_Zone.GetName();
    m_Position = m_Zone.GetPosition();
    m_Radius = m_Zone.GetRadius();
    m_IsActive = true;

    m_PlayersInZone = new array<Man>;
    m_TimeElapsed = 0;

    array<ref KOTH_ObjectEntry> objects = m_Zone.GetObjects();
    foreach (KOTH_ObjectEntry object : objects)
    {
        SpawnObject(object);
    }
}

protected void RemoveVehicle()
{
    if (m_Vehicle)
    {
        KOTH_Log.LogBasic("Vehicle KOTH: Vehicle will remain in the world for players to use");
        m_Vehicle = null; 
    }
}

protected void RemoveGroundDecal()
{
    if (m_GroundDecal)
    {
        GetGame().ObjectDelete(m_GroundDecal);
        m_GroundDecal = null;
        KOTH_Log.LogBasic("Vehicle KOTH: Ground decal removed after 10 minutes");
    }
}

	protected void RemoveObjects()
	{
		foreach (Object object : m_Objects)
		{
			if (object)
				GetGame().ObjectDelete(object);
		}
	}

	protected void SetActive(bool state)
	{
		m_IsActive = state;
	}

protected void SetState(int state)
{
    switch (state)
    {
        case eKOTHVehicleState.START:
            
        break;
        
        case eKOTHVehicleState.PAUSED:

        break;
        
        case eKOTHVehicleState.RUNNING:
            
        break;
        
        case eKOTHVehicleState.WON:
            
        break;
        
        case eKOTHVehicleState.FAILED:
            
        break;

        default:
        break;
    }

    #ifdef BASICMAP
    m_Mission.GetKOTHManager().UpdateBasicMapMarkers();
    #endif

    m_State = state;
}

protected bool ShouldEnd(out int endCase)
{
	if (m_IsFinished)
		return false;

	if (!m_IsRunning)
	{
		if (m_TimeElapsed > m_Zone.GetTimeDespawn())
		{
			endCase = eKOTHEndCases.TIMEOUT;
			return true;
		}
		else
			return false;
	}

	if (m_Progress >= 100)
	{
		if (m_Fatalities >= m_Zone.GetMinimumDeaths())
			endCase = eKOTHEndCases.WIN;
		else
			endCase = eKOTHEndCases.FAIL;

		return true;
	}
	else if (m_Zone.GetTimeLimit() > 0 && m_TimeElapsed >= m_Zone.GetTimeLimit())
	{
		endCase = eKOTHEndCases.FAIL;
		return true;
	}
	else
	{
		return false;
	}
}

	protected bool ShouldStart()
	{
		if (m_IsRunning)
			return false;
		
		return !m_IsFinished && m_PlayersInZone.Count() >= KOTH_Settings.GetVehicleMinimumPlayers() && m_TimeElapsed >= m_Zone.GetTimeStart();
	}

	protected void SpawnItem(ItemBase parent, KOTH_LootItem lootItem, bool isAttachment = false)
	{
		ItemBase newItem;
		Weapon_Base weapon;
		Magazine_Base mag;
		bool isMagazine;
		
		if (!parent)
			return;
		
		bool parentIsWeapon;
		parentIsWeapon = Class.CastTo(weapon, parent);
		
		if (isAttachment)
		{
			if (parentIsWeapon)
			{
				newItem = weapon.SpawnAttachmentKOTH(lootItem.GetName(), isMagazine);
			}
			else
			{
				newItem = ItemBase.Cast(parent.GetInventory().CreateAttachment(lootItem.GetName()));
			}
		}
		else
		{
			newItem = ItemBase.Cast(parent.GetInventory().CreateInInventory(lootItem.GetName()));
		}

		if (!newItem)
			return;

		if (lootItem.GetQuantity() > -1)
		{
			if (isMagazine && Class.CastTo(mag, newItem))
			{
				mag.ServerSetAmmoCount(lootItem.GetQuantity());
			}
			else
			{
				newItem.SetQuantity(lootItem.GetQuantity());
			}
		}

		if (isMagazine)
			return;

		array<ref KOTH_LootItem> attachmentsArray = lootItem.GetAttachments();
		foreach (KOTH_LootItem attachment : attachmentsArray)
		{
			SpawnItem(newItem, attachment, true);
		}

		array<ref KOTH_LootItem> cargoArray = lootItem.GetCargo();
		foreach (KOTH_LootItem cargo : cargoArray)
		{
			SpawnItem(newItem, cargo);
		}
	}

	protected void SpawnObject(KOTH_ObjectEntry objectEntry)
	{
		vector newOrientation, normal, position, orientation;
		
		orientation = objectEntry.GetOrientation();
		
		if (objectEntry.IsAbsolutePlacement())
			position = objectEntry.GetPosition();
		else
			position = m_Position + objectEntry.GetPosition();
		
		if (objectEntry.IsAlignToTerrain())
		{
			vector surfaceOrientation = GetGame().GetSurfaceOrientation(position[0], position[2]);
			orientation += surfaceOrientation;
		}

		Object object = GetGame().CreateObjectEx(objectEntry.GetClassName(), position, ECE_CREATEPHYSICS|ECE_UPDATEPATHGRAPH);
		
		if (!object)
			return;
		
		m_Objects.Insert(object);

		if (objectEntry.IsPlaceOnSurface())
		{
			vector min_max[2];
			object.ClippingInfo(min_max);

			float offset = (min_max[1][1] - min_max[0][1]) / 2;

			position[1] = GetGame().SurfaceY(position[0], position[2]) + offset;
		}

		object.SetPosition(position);
		object.SetOrientation(orientation);
	}

protected void StartEvent()
{
	string locationMessage;
	if (KOTH_Settings.IsEstimateLocation())
		locationMessage = string.Format(" at %1", m_LocationName);

	m_Mission.GetKOTHManager().SendKOTHNotification(string.Format("The battle for the VEHICLE at [%1]%2 has begun!", m_Name, locationMessage));

	m_IsRunning = true;
	SetState(eKOTHVehicleState.PAUSED);
}

	protected void StopEvent()
	{
		m_DeleteTimer = new Timer();
		m_DeleteTimer.Run(60, this, "SetActive", new Param1<bool>(false), false);
	}
	
protected void WinEvent()
{
	m_IsFinished = true;
	m_IsRunning = false;

	string playerNames = "";
	string name;

	for (int i = 0; i < m_PlayersInZone.Count(); i++)
	{
		name = "";
		if (m_PlayersInZone[i] && m_PlayersInZone[i].GetIdentity())
			name = m_PlayersInZone[i].GetIdentity().GetName();

		if (i > 0)
			playerNames += ", ";

		playerNames += name;
	}

	string fatalities = "";
	if (m_Fatalities == 1)
		fatalities = string.Format(" %1 person gave their life in the attempt to capture it.", m_Fatalities);
	else if (m_Fatalities > 1)
		fatalities = string.Format(" %1 people gave their lives in the attempt to capture it.", m_Fatalities);

	string locationMessage;
	if (KOTH_Settings.IsEstimateLocation())
		locationMessage = string.Format(" at %1", m_LocationName);
	
	m_Mission.GetKOTHManager().SendKOTHNotification(string.Format("CAPTURE COMPLETE! %1 captured the VEHICLE [%2]%3!%4", playerNames, m_Name, locationMessage, fatalities));
	
	m_Mission.GetKOTHManager().AddVehicleCooldown(m_Zone.GetName());

	CreateVehicle();
	if (m_Vehicle)
	{
		CreateVehicleReward();
	}
	
	SetState(eKOTHVehicleState.WON);
	
	GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(PlayVehicleWinSound, 500, false);
	
	StopEvent();
}

protected void PlayVehicleWinSound()
{
	if (!GetGame().IsDedicatedServer())
		return;
	
	array<Man> playersToNotify = new array<Man>;
	
	foreach (Man player : m_PlayersInZone)
	{
		if (player && player.IsAlive() && player.GetIdentity())
		{
			playersToNotify.Insert(player);
		}
	}
	
	foreach (Man notifyPlayer : playersToNotify)
	{
		if (notifyPlayer && notifyPlayer.GetIdentity())
		{
			GetGame().RPCSingleParam(notifyPlayer, eKOTH.RPC_KOTH_PLAY_VEHICLE_SOUND, new Param1<vector>(m_Position), true, notifyPlayer.GetIdentity());
		}
	}
}


	#ifdef BASICMAP
	BasicMapCircleMarker CreateBasicMapMarker()
	{
		if (!m_BasicMapMarker)
		{
			m_BasicMapMarker = new BasicMapCircleMarker(string.Format("[VEHICLE KOTH] %1", m_Zone.GetName()), m_Zone.GetPosition(), "BasicMap\\gui\\images\\car.paa", {255,255,0});
			m_BasicMapMarker.SetShowCenterMarker(true);
			m_BasicMapMarker.SetRadius(m_Zone.GetRadius());
			m_BasicMapMarker.SetCanEdit(false);
		}
		
		return m_BasicMapMarker;
	}

	BasicMapCircleMarker GetBasicMapMarker()
	{
		if (m_BasicMapMarker)
			return m_BasicMapMarker;
		
		return NULL;
	}
	#endif

	#ifdef LBGROUP_SYSTEM_NEW
	void CreateAdvancedGroupsMapMarker()
	{
		if (!m_AdvancedGroupsMapMarker)
		{
			#ifdef LBmaster_Rework
			m_AdvancedGroupsMapMarker = LBStaticMarkerManager.Get.AddTempServerMarker(string.Format("[VEHICLE KOTH] %1", m_Zone.GetName()), m_Zone.GetPosition(), "LBmaster_Groups\\gui\\icons\\car.paa", ARGB(255,255,255,0), false, false);
			#else
			m_AdvancedGroupsMapMarker = LBStaticMarkerManager.Get().AddTempServerMarker(string.Format("[VEHICLE KOTH] %1", m_Zone.GetName()), m_Zone.GetPosition(), "LBmaster_Groups\\gui\\icons\\car.paa", ARGB(255,255,255,0), false, false);
			#endif
		}
	}

	LBServerMarker GetAdvancedGroupsMapMarker()
	{
		if (m_AdvancedGroupsMapMarker)
			return m_AdvancedGroupsMapMarker;
		
		return NULL;
	}
	#endif

	#ifdef EXPANSIONMODNAVIGATION
	void CreateExpansionServerMarker()
	{
		if (!CF_Modules<ExpansionMarkerModule>.Get(m_ExpansionMarkerModule))
        return;

		m_ExpansionServerMarker = m_ExpansionMarkerModule.CreateServerMarker("[VEHICLE KOTH] %1", "Vehicle", m_Zone.GetPosition(), ARGB(255,255,255,0), true);
	}

	void RemoveExpansionServerMarker()
	{
		if ( !m_ExpansionServerMarker )
			return;
		
		m_ExpansionMarkerModule.RemoveServerMarker( m_ExpansionServerMarker.GetUID() );
	}
	#endif

	#ifdef VanillaPPMap
	void CreateVPPMapMarker()
	{
		if (!GetGame().IsDedicatedServer())
			return;
		
		m_VPPMapMarker = new MarkerInfo("VEHICLE KOTH", "KOTH\\Icon\\KOTHVehicle_ca.paa", Vector(255,255,0), Vector(m_Position[0], 0, m_Position[2]), true, true);
		m_Mission.AddKOTHMarkerVPP(m_VPPMapMarker);
		UpdateVPPMapMarker();
	}

	protected void UpdateVPPMapMarker()
	{
		if (!GetGame().IsDedicatedServer())
			return;

		m_Mission.UpdateKOTHMarkersVPP();
	}

	void RemoveVPPMapMarker()
	{
		if (!GetGame().IsDedicatedServer())
			return;

		m_Mission.RemoveKOTHMarkerVPP(m_VPPMapMarker);
		UpdateVPPMapMarker();
	}
	#endif
	
	string GetLocationName()
	{
		return m_LocationName;
	}

	vector GetPosition()
	{
		return m_Position;
	}

	int GetRadius()
	{
		return m_Radius;
	}

	KOTH_Zone GetZone()
	{
		return m_Zone;
	}

	bool IsActive()
	{
		return m_IsActive;
	}

	Car GetVehicle()
	{
		return m_Vehicle;
	}

	void SetLocationName(string locationName)
	{
		m_LocationName = locationName;
	}

void Update()
{
	if (ShouldStart())
	{
		StartEvent();
		return;
	}

	int endCase;
	if (ShouldEnd(endCase))
	{
		switch (endCase)
		{
			case eKOTHEndCases.WIN:
				WinEvent();
				break;
			case eKOTHEndCases.FAIL:
				FailEvent();
				break;
			case eKOTHEndCases.TIMEOUT:
				CleanupEvent();
				break;
		}
	}

	if (m_IsRunning)
	{
		int playerCount = m_PlayersInZone.Count();
		float amountToAdd = 100 / KOTH_Settings.GetVehicleBaseCaptureTime();

		if (playerCount > 0 && m_LastPlayerCount == 0)
		{
			string firstPlayerName = "";
			if (m_PlayersInZone[0] && m_PlayersInZone[0].GetIdentity())
				firstPlayerName = m_PlayersInZone[0].GetIdentity().GetName();
			
			string locationMessage;
			if (KOTH_Settings.IsEstimateLocation())
				locationMessage = string.Format(" at %1", m_LocationName);
			
			m_Mission.GetKOTHManager().SendKOTHNotification(string.Format("%1 has started capturing the VEHICLE [%2]%3!", firstPlayerName, m_Name, locationMessage));
		}

		if (playerCount > 0)
		{
			if (m_Progress <= 100)
			{
				m_Progress += amountToAdd;
			}

			if (m_LastPlayerCount <= 0)
				SetState(eKOTHVehicleState.RUNNING);

			m_LastPlayerCount = playerCount;
		}
		else
		{
			if (m_Progress > 0)
				m_Progress -= amountToAdd;

			if (m_LastPlayerCount > 0)
				SetState(eKOTHVehicleState.PAUSED);

			m_LastPlayerCount = playerCount;
		}

		float roundedProgress = Math.Round(m_Progress * 10) / 10;
		if (Math.AbsFloat(roundedProgress - m_LastSyncedProgress) >= 0.1)
		{
			SyncProgressToPlayers(true, m_Progress);
			m_LastSyncedProgress = roundedProgress;
		}
	}

	m_TimeElapsed++;
}

	void UpdatePlayers(array<Man> players)
	{
		int i;
		PlayerBase player;

		for (i = 0; i < m_PlayersInZone.Count(); i++)
		{
			if (players.Find(m_PlayersInZone[i]) < 0)
			{
				if (Class.CastTo(player, m_PlayersInZone[i]) && !player.IsAlive())
				{
					if (player.GetKillerKOTH() && vector.Distance(m_Position, player.GetKillerKOTH().GetPosition()) > (m_Radius * 2))
					{
						m_Progress -= (100 / KOTH_Settings.GetVehicleBaseCaptureTime()) * 10;
					}

					m_Fatalities++;
				}

				if (m_PlayersInZone[i] && m_PlayersInZone[i].GetIdentity())
				{
					GetGame().RPCSingleParam(m_PlayersInZone[i], eKOTH.RPC_KOTH_PROGRESS_SYNC, new Param2<bool, float>(false, 0.0), true, m_PlayersInZone[i].GetIdentity());
				}

				m_PlayersInZone.RemoveItem(m_PlayersInZone[i]);
			}
		}

		for (i = 0; i < players.Count(); i++)
		{
			if (m_PlayersInZone.Find(players[i]) < 0)
			{
				m_PlayersInZone.Insert(players[i]);
				
				if (players[i] && players[i].GetIdentity())
				{
					GetGame().RPCSingleParam(players[i], eKOTH.RPC_KOTH_PROGRESS_SYNC, new Param2<bool, float>(true, m_Progress), true, players[i].GetIdentity());
				}
			}
		}
	}
}