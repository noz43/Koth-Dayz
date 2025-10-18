modded class PlayerBase extends ManBase
{
    protected PlayerBase m_KilledByKOTH;
    
    protected ref KOTH_HUD m_KOTHHud;
    protected bool m_IsInKOTHZone = false;
    protected float m_KOTHProgress = 0.0;

    void PlayerBase()
    {
        if (GetGame().IsClient() || !GetGame().IsMultiplayer())
        {
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(InitKOTHHUD, 1000, false);
        }
    }

    void ~PlayerBase()
    {
        if (m_KOTHHud)
        {
            delete m_KOTHHud;
            m_KOTHHud = null;
        }
    }

    protected void InitKOTHHUD()
    {
        if (!m_KOTHHud && (GetGame().IsClient() || !GetGame().IsMultiplayer()))
        {
            if (!KOTH_Settings.IsUseProgressHUD())
                return;
            
            m_KOTHHud = new KOTH_HUD();
        }
    }

    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        switch (rpc_type)
        {
            case eKOTH.RPC_KOTH_CONFIG_SYNC:
            {
                if (GetGame().IsClient())
                {
                    KOTH_Settings.OnRPC(this, ctx);
                }
                break;
            }
            case eKOTH.RPC_KOTH_PROGRESS_SYNC:
            {
                if (GetGame().IsClient())
                {
                    OnKOTHProgressSync(ctx);
                }
                break;
            }
            case eKOTH.RPC_KOTH_PLAY_ALARM_SOUND:
            {
                if (GetGame().IsClient())
                {
                    Param1<vector> alarmData = new Param1<vector>(vector.Zero);
                    if (ctx.Read(alarmData))
                    {
                        PlaySoundKOTHAlarm(alarmData.param1);
                    }
                }
                break;
            }
            case eKOTH.RPC_KOTH_PLAY_VEHICLE_SOUND:
            {
                if (GetGame().IsClient())
                {
                    Param1<vector> vehicleData = new Param1<vector>(vector.Zero);
                    if (ctx.Read(vehicleData))
                    {
                        PlaySoundVehicleWin(vehicleData.param1);
                    }
                }
                break;
            }
        }
    }

    protected void PlaySoundKOTHAlarm(vector position)
    {
        EffectSound sound = SEffectManager.PlaySound("KOTH_Alarm_SoundSet", position);
        if (sound)
        {
            sound.SetSoundAutodestroy(true);
        }
    }

    protected void PlaySoundVehicleWin(vector position)
    {
        EffectSound sound = SEffectManager.PlaySound("KOTH_VehicleWin_SoundSet", position);
        if (sound)
        {
            sound.SetSoundAutodestroy(true);
        }
    }

    protected void OnKOTHProgressSync(ParamsReadContext ctx)
    {
        Param2<bool, float> data = new Param2<bool, float>(false, 0.0);
        
        if (!ctx.Read(data))
            return;
        
        bool inZone = data.param1;
        float progress = data.param2;
        
        if (progress < 0.0) progress = 0.0;
        if (progress > 100.0) progress = 100.0;
        
        bool stateChanged = (m_IsInKOTHZone != inZone);
        
        m_IsInKOTHZone = inZone;
        m_KOTHProgress = progress;
        
        UpdateKOTHHUD();
    }

    protected void UpdateKOTHHUD()
    {
        if (!KOTH_Settings.IsUseProgressHUD())
            return;
        
        if (!m_KOTHHud && m_IsInKOTHZone)
        {
            InitKOTHHUD();
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(UpdateKOTHHUDDelayed, 100, false);
            return;
        }
        
        if (!m_KOTHHud)
            return;
        
        if (m_IsInKOTHZone)
        {
            m_KOTHHud.SetVisible(true);
            m_KOTHHud.UpdateProgress(m_KOTHProgress);
        }
        else
        {
            m_KOTHHud.SetVisible(false);
        }
    }

    protected void UpdateKOTHHUDDelayed()
    {
        UpdateKOTHHUD();
    }

    void UpdateKOTHHUDPosition()
    {
        if (m_KOTHHud)
        {
            m_KOTHHud.OnScreenSizeChanged();
        }
    }

    override void OnGameplayDataHandlerSync()
    {
        super.OnGameplayDataHandlerSync();
    }
    
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);
        
        if (!GetGame().IsDedicatedServer())
            return;

        if (killer.IsEntityAI())
            Class.CastTo(m_KilledByKOTH, EntityAI.Cast(killer).GetHierarchyRootPlayer());
    }
    
    PlayerBase GetKillerKOTH()
    {
        if (m_KilledByKOTH != this)
            return m_KilledByKOTH;
        
        return NULL;
    }

    void ShowKOTHHUDTest(float progress = 50.0)
    {
        if (!m_KOTHHud)
        {
            InitKOTHHUD();
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(ShowKOTHHUDTestDelayed, 500, false, progress);
        }
        else
        {
            ShowKOTHHUDTestDelayed(progress);
        }
    }

    protected void ShowKOTHHUDTestDelayed(float progress)
    {
        if (m_KOTHHud)
        {
            m_KOTHHud.SetVisible(true);
            m_KOTHHud.UpdateProgress(progress);
        }
    }

    void HideKOTHHUDTest()
    {
        if (m_KOTHHud)
        {
            m_KOTHHud.SetVisible(false);
        }
    }
}