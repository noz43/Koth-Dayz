enum eKOTHParticleState
{
	NO_SMOKE 	= 0
	START 	 	= 1,
	LOOP 	 	= 2,
	WIN			= 3,
	FAIL		= 4,
	COUNT 		= 5
}

class KOTH_Particle extends House
{
	protected ref Timer				m_TimerSmokeStart;
	protected ref Timer				m_TimerSmokeLoop;
	protected ref Timer				m_TimerSmokeEnd;
	
	protected ref Timer				m_TimerDefer;
	
	protected eKOTHParticleState	m_KOTHParticleState;
	protected eKOTHParticleState	m_LastKOTHParticleState;

	protected Particle 				m_ParticleSmoke;
	protected int					m_ParticleSmokeCurrentId;
	protected int					m_ParticleSmokeEndId;
	protected int					m_ParticleSmokeFailId;
	protected int					m_ParticleSmokeLoopId;
	protected int					m_ParticleSmokeStartId;
	protected int					m_ParticleSmokeWinId;
	protected vector				m_ParticleSmokePosition;
	
	protected EffectSound			m_SoundSmoke;
	protected string				m_SoundSmokeStartId;
	protected string				m_SoundSmokeLoopId;
	protected string				m_SoundSmokeEndId;

	void KOTH_Particle()
	{
		m_KOTHParticleState = eKOTHParticleState.NO_SMOKE;

		m_TimerSmokeLoop = new Timer;
		m_TimerDefer = new Timer;

		SetParticleSmokeStart(ParticleList.GRENADE_M18_YELLOW_LOOP); 
		SetParticleSmokeLoop(ParticleList.GRENADE_M18_WHITE_LOOP); 
		SetParticleSmokeWin(ParticleList.GRENADE_M18_GREEN_LOOP); 
		SetParticleSmokeFail(ParticleList.GRENADE_M18_RED_LOOP); 
		SetParticleSmokeEnd(ParticleList.INVALID); 

		RegisterNetSyncVariableInt("m_KOTHParticleState", eKOTHParticleState.NO_SMOKE, eKOTHParticleState.COUNT);
	}
	
	void ~KOTH_Particle()
	{
		SoundSmokeStop();
		DestroyParticle(m_ParticleSmoke);
	}

	protected void DestroyParticle(Particle p)
	{
		#ifndef SERVER
		if (p != null)
		{
			p.Stop();
		}
		#endif
	}

	protected void PlaySmokeParticle()
	{
		m_ParticleSmoke = ParticleManager.GetInstance().PlayOnObject(m_ParticleSmokeCurrentId, this, vector.Zero, vector.Zero, true);
	}

	protected void RefreshParticlesAndSounds()
	{
		eKOTHParticleState state = GetKOTHParticleState();

		if ( m_LastKOTHParticleState != state )
		{
			if ( state == eKOTHParticleState.START )
			{
				SoundSmokeEnd();
				SoundSmokeStart();
				
				DestroyParticle(m_ParticleSmoke);
				SetParticleSmokeCurrent(m_ParticleSmokeStartId);
				PlaySmokeParticle();
			}
			else if ( state == eKOTHParticleState.LOOP )
			{
				SoundSmokeStop();
				SoundSmokeLoop();
				
				DestroyParticle(m_ParticleSmoke);
				SetParticleSmokeCurrent(m_ParticleSmokeLoopId);
				PlaySmokeParticle();
			}
			else if ( state == eKOTHParticleState.WIN )
			{
				SoundSmokeStop();
				SoundSmokeEnd();
				
				DestroyParticle(m_ParticleSmoke);
				SetParticleSmokeCurrent(m_ParticleSmokeWinId);
				PlaySmokeParticle();
			}
			else if ( state == eKOTHParticleState.FAIL )
			{
				SoundSmokeStop();
				SoundSmokeEnd();
				
				DestroyParticle(m_ParticleSmoke);
				SetParticleSmokeCurrent(m_ParticleSmokeFailId);
				PlaySmokeParticle();
			}
			else if ( state == eKOTHParticleState.WIN )
			{
				SoundSmokeStop();
				SoundSmokeEnd();
				
				DestroyParticle(m_ParticleSmoke);
				SetParticleSmokeCurrent(m_ParticleSmokeWinId);
				PlaySmokeParticle();
			}
			else if ( state == eKOTHParticleState.NO_SMOKE )
			{
				SoundSmokeStop();

				DestroyParticle(m_ParticleSmoke);
			}
			
			m_LastKOTHParticleState = state;
		}
	}

	protected void SoundSmokeStart()
	{
		PlaySoundSetLoop(m_SoundSmoke, m_SoundSmokeStartId, 0.3, 1.0);
	}
	
	protected void SoundSmokeLoop()
	{
		PlaySoundSetLoop(m_SoundSmoke, m_SoundSmokeLoopId, 0.3, 1.0);
	}

	protected void SoundSmokeEnd()
	{
		PlaySoundSetLoop(m_SoundSmoke, m_SoundSmokeEndId, 1.0, 1.0);
	}
	
	protected void SoundSmokeStop()
	{
		StopSoundSet(m_SoundSmoke);
	}
	
	void Fail()
	{
		SetKOTHParticleState(eKOTHParticleState.FAIL);
	}

	void Start()
	{
		SetKOTHParticleState(eKOTHParticleState.START);
	}
	
	void Win()
	{
		SetKOTHParticleState(eKOTHParticleState.WIN);
	}

	void SetParticleSmokeCurrent(int particle)
	{
		m_ParticleSmokeCurrentId = particle;
	}
	
	void SetParticleSmokeStart(int particle)
	{
		m_ParticleSmokeStartId = particle;
	}

	void SetParticleSmokeLoop(int particle)
	{
		m_ParticleSmokeLoopId = particle;
	}

	void SetParticleSmokeFail(int particle)
	{
		m_ParticleSmokeFailId = particle;
	}
	
	void SetParticleSmokeWin(int particle)
	{
		m_ParticleSmokeWinId = particle;
	}

	void SetParticleSmokeEnd(int particle)
	{
		m_ParticleSmokeEndId = particle;
	}

	void SetSoundSmokeStart(string sound)
	{
		m_SoundSmokeStartId = sound;
	}

	void SetSoundSmokeLoop(string sound)
	{
		m_SoundSmokeLoopId = sound;
	}

	void SetSoundSmokeEnd(string sound)
	{
		m_SoundSmokeEndId = sound;
	}	

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		SoundSmokeStop();
		DestroyParticle(m_ParticleSmoke);
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		RefreshParticlesAndSounds();
	}

	eKOTHParticleState GetKOTHParticleState()
	{
		return m_KOTHParticleState;
	}

	void SetKOTHParticleState(eKOTHParticleState state)
	{
		if ( GetGame().IsServer() )
		{
			if ( m_KOTHParticleState != state )
			{
				m_KOTHParticleState = state;
				
				SetSynchDirty();
			}
		}
	}

}