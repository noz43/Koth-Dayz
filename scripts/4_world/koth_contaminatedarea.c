class KOTH_ContaminatedArea extends ContaminatedArea_Dynamic
{
	protected ref Timer 	m_Timer;
	protected float 		m_Lifetime = 300;
	
	void OnCreate()
	{
		if ( GetGame().IsServer() )
			m_PPERequesterIdx = GetRequesterIndex(m_PPERequesterType);
		
		SetSynchDirty();
		
		if ( m_DecayState == eAreaDecayStage.INIT )
		{
			vector areaPos = GetPosition();
			m_OffsetPos = areaPos;
			m_OffsetPos[1] = m_OffsetPos[1] + AIRBORNE_FX_OFFSET;
			
			array<vector> artilleryPoints = GetGame().GetMission().GetWorldData().GetArtyFiringPos();
			vector closestPoint = areaPos;
			int dist = 0;
			int temp;
			int index = 0;
			for ( int i = 0; i < artilleryPoints.Count(); i++ )
			{
				temp = vector.DistanceSq( artilleryPoints.Get( i ), areaPos );
				if ( temp < dist || dist == 0 )
				{
					dist = temp;
					index = i;
				}
			}
			
			closestPoint = artilleryPoints.Get( index );
			
			float delay = vector.Distance( closestPoint, areaPos );
			delay = delay / ARTILLERY_SHELL_SPEED;
			delay += AIRBORNE_EXPLOSION_DELAY;
			
			Param3<vector, vector, float> pos;
			array<ref Param> params;
			
			pos = new Param3<vector, vector, float>( closestPoint, areaPos, delay );
			params = new array<ref Param>;
			
			params.Insert( pos );
			GetGame().RPC( null, ERPCs.RPC_SOUND_ARTILLERY_SINGLE, params, true );
			
			m_FXTimer = new Timer( CALL_CATEGORY_GAMEPLAY );
			m_FXTimer.Run( delay, this, "PlayFX" );	
			
			delay += AREA_SETUP_DELAY;

			m_StartupTimer = new Timer( CALL_CATEGORY_GAMEPLAY );
			m_StartupTimer.Run( delay, this, "InitZone" );
		}
	}
	
	void OnUpdate()
	{
		if ( GetRemainingTime() < GetFinishDecayLifetime() )
			SetDecayState( eAreaDecayStage.DECAY_END );
		else if ( GetRemainingTime() < GetStartDecayLifetime() )
			SetDecayState( eAreaDecayStage.DECAY_START );

		m_Lifetime--;
		if (m_Lifetime <= 0)
		{
			m_Timer.Stop();
			Delete();
		}
		
		SetSynchDirty();
	}

	void SetAreaLifetime(float lifetime)
	{
		m_Lifetime = lifetime;
	}

	override float GetStartDecayLifetime()
	{
		return 20;
	}
	
	override float GetFinishDecayLifetime()
	{
		return 10;
	}
	
	override float GetRemainingTime()
	{
		return m_Lifetime;
	}

	override void EEInit()
	{
		if ( !m_ToxicClouds )
			m_ToxicClouds = new array<Particle>;
		
		m_Name = "Default Dynamic";
		m_Radius = 75;
		m_PositiveHeight = 7;
		m_NegativeHeight = 10;
		m_InnerRings = 1;
		m_InnerSpacing = 40;
		m_OuterSpacing = 30;
		m_OuterRingOffset = 0;
		m_Type = eZoneType.DYNAMIC;
		m_TriggerType = "ContaminatedTrigger_Dynamic";
		
		SetSynchDirty();
		
		m_OffsetPos = GetPosition();
		m_OffsetPos[1] = m_OffsetPos[1] + AIRBORNE_FX_OFFSET;

		if ( m_DecayState == eAreaDecayStage.INIT )
		{
			if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			{
				OnCreate();
			}
			else
			{
				PlayFlareVFX();
			}
		}
		
		if ( m_DecayState == eAreaDecayStage.LIVE )
			InitZone();
		else if ( GetGame().IsClient() && m_DecayState > eAreaDecayStage.LIVE )
			InitZoneClient();

		m_Timer = new Timer( CALL_CATEGORY_GAMEPLAY );
		m_Timer.Run(1, this, "OnUpdate", NULL, true);

		super.EEInit();
	}
}