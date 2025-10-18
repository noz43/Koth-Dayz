modded class CarScript
{
    protected bool m_IsKOTHVehicle = false;
    protected string m_KOTHVehicleID = "";
    private ref Timer m_SaveDelayTimer;
    
    void SetKOTHVehicle(bool state, string id = "")
    {
        m_IsKOTHVehicle = state;
        m_KOTHVehicleID = id;
        
        if (state)
        {
            ScheduleSave(1000);
        }
    }
    
    bool IsKOTHVehicle()
    {
        return m_IsKOTHVehicle;
    }
    
    string GetKOTHVehicleID()
    {
        return m_KOTHVehicleID;
    }
    
    private void ScheduleSave(int delayMs = 5000)
    {
        if (!m_IsKOTHVehicle || !GetGame().IsServer())
            return;
        
        if (m_SaveDelayTimer && m_SaveDelayTimer.IsRunning())
            m_SaveDelayTimer.Stop();
        
        if (!m_SaveDelayTimer)
            m_SaveDelayTimer = new Timer();
        
        m_SaveDelayTimer.Run(delayMs * 0.001, this, "SaveKOTHVehicle", NULL, false);
    }
    
    void SaveKOTHVehicle()
    {
        if (!m_IsKOTHVehicle || !GetGame().IsServer())
            return;
        
        Car car = Car.Cast(this);
        if (car)
            KOTH_VehicleDatabase.SaveVehicle(car);
    }
    
    override void EEItemAttached(EntityAI item, string slot_name)
    {
        super.EEItemAttached(item, slot_name);
        
        if (m_IsKOTHVehicle)
            ScheduleSave(5000);
    }
    
    override void EEItemDetached(EntityAI item, string slot_name)
    {
        super.EEItemDetached(item, slot_name);
        
        if (m_IsKOTHVehicle)
            ScheduleSave(5000);
    }
    
    override void EECargoIn(EntityAI item)
    {
        super.EECargoIn(item);
        
        if (m_IsKOTHVehicle)
            ScheduleSave(5000);
    }
    
    override void EECargoOut(EntityAI item)
    {
        super.EECargoOut(item);
        
        if (m_IsKOTHVehicle)
            ScheduleSave(5000);
    }
    
    override void EEDelete(EntityAI parent)
    {
        super.EEDelete(parent);
        
        if (m_IsKOTHVehicle)
        {
            if (m_SaveDelayTimer && m_SaveDelayTimer.IsRunning())
                m_SaveDelayTimer.Stop();
            
            KOTH_VehicleDatabase.RemoveVehicle(m_KOTHVehicleID);
        }
    }
    
    override void OnEngineStart()
    {
        super.OnEngineStart();
        
        if (m_IsKOTHVehicle)
            ScheduleSave(10000);
    }
    
    override void OnEngineStop()
    {
        super.OnEngineStop();
        
        if (m_IsKOTHVehicle)
            ScheduleSave(2000);
    }
}