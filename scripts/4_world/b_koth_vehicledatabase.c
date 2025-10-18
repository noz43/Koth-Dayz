class KOTH_VehicleEntry
{
    string vehicleID;
    string classname;
    vector position;
    vector orientation;
    
    float fuel;
    float coolant;
    float oil;
    float brake;
    float health;
    
    ref array<ref KOTH_AttachmentEntry> attachments;
    ref array<string> cargoItems;
    
    void KOTH_VehicleEntry()
    {
        attachments = new array<ref KOTH_AttachmentEntry>;
        cargoItems = new array<string>;
    }
}

class KOTH_AttachmentEntry
{
    string classname;
    float health;
    int quantity;
    
    void KOTH_AttachmentEntry(string cls = "", float hp = 1.0, int qty = -1)
    {
        classname = cls;
        health = hp;
        quantity = qty;
    }
}

class KOTH_VehicleDatabase
{
    private static const string DATABASE_FILE = "$profile:KOTH-SAVIOR/VehicleDB.json";
    private static const float AUTOSAVE_INTERVAL = 300.0;
    
    private static ref map<string, ref KOTH_VehicleEntry> m_Database;
    private static bool m_IsShuttingDown = false;
    private static bool m_IsFullyLoaded = false;
    
    static void Init()
    {
        if (!m_Database)
            m_Database = new map<string, ref KOTH_VehicleEntry>;
        
        LoadDatabase();
        
        KOTH_Log.LogBasic("Vehicle KOTH: Database initialized, waiting for vehicles to spawn before auto-save");
    }
    
    static void StartAutoSave()
    {
        m_IsFullyLoaded = true;
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AutoSave, AUTOSAVE_INTERVAL * 1000, true);
        KOTH_Log.LogBasic("Vehicle KOTH: Auto-save started (every 5 minutes)");
    }
    
    static void OnServerShutdown()
    {
        m_IsShuttingDown = true;
        KOTH_Log.LogBasic("Vehicle KOTH: Server shutdown detected, performing final save...");
        
        ForceSaveAllTrackedVehicles();
        
        KOTH_Log.LogBasic("Vehicle KOTH: Final save completed");
    }
    
    private static void ForceSaveAllTrackedVehicles()
    {
        if (!m_Database)
            return;
        
        array<Object> allObjects = new array<Object>;
        GetGame().GetObjectsAtPosition(vector.Zero, 999999, allObjects, null);
        
        int savedCount = 0;
        foreach (Object obj : allObjects)
        {
            CarScript vehicleScript = CarScript.Cast(obj);
            if (vehicleScript)
            {
                if (vehicleScript.IsKOTHVehicle())
                {
                    Car car = Car.Cast(vehicleScript);
                    if (car)
                    {
                        UpdateVehicleInDatabase(car);
                        savedCount++;
                    }
                }
            }
        }
        
        if (savedCount > 0)
        {
            SaveDatabase();
            KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Emergency saved %1 vehicles", savedCount));
        }
    }
    
    private static void UpdateVehicleInDatabase(Car vehicle)
    {
        if (!vehicle)
            return;
        
        CarScript vehicleScript = CarScript.Cast(vehicle);
        if (!vehicleScript)
            return;
        
        string vehicleID = vehicleScript.GetKOTHVehicleID();
        if (vehicleID == "")
            return;
        
        KOTH_VehicleEntry entry = CreateEntryFromVehicle(vehicle, vehicleID);
        
        if (m_Database.Contains(vehicleID))
            m_Database.Set(vehicleID, entry);
        else
            m_Database.Insert(vehicleID, entry);
    }
    
    private static KOTH_VehicleEntry CreateEntryFromVehicle(Car vehicle, string vehicleID)
    {
        KOTH_VehicleEntry entry = new KOTH_VehicleEntry();
        entry.vehicleID = vehicleID;
        entry.classname = vehicle.GetType();
        entry.position = vehicle.GetPosition();
        entry.orientation = vehicle.GetOrientation();
        entry.health = vehicle.GetHealth01("", "");
        
        entry.fuel = vehicle.GetFluidFraction(CarFluid.FUEL);
        entry.coolant = vehicle.GetFluidFraction(CarFluid.COOLANT);
        entry.oil = vehicle.GetFluidFraction(CarFluid.OIL);
        entry.brake = vehicle.GetFluidFraction(CarFluid.BRAKE);
        
        for (int i = 0; i < vehicle.GetInventory().AttachmentCount(); i++)
        {
            EntityAI att = vehicle.GetInventory().GetAttachmentFromIndex(i);
            if (att)
            {
                KOTH_AttachmentEntry attEntry = new KOTH_AttachmentEntry();
                attEntry.classname = att.GetType();
                attEntry.health = att.GetHealth01("", "");
                
                ItemBase itemAtt = ItemBase.Cast(att);
                if (itemAtt)
                    attEntry.quantity = itemAtt.GetQuantity();
                
                entry.attachments.Insert(attEntry);
            }
        }
        
        CargoBase cargo = vehicle.GetInventory().GetCargo();
        if (cargo)
        {
            for (int j = 0; j < cargo.GetItemCount(); j++)
            {
                EntityAI item = cargo.GetItem(j);
                if (item)
                    entry.cargoItems.Insert(item.GetType());
            }
        }
        
        return entry;
    }
    
    static void SaveVehicle(Car vehicle)
    {
        if (!vehicle || !GetGame().IsServer())
            return;
        
        UpdateVehicleInDatabase(vehicle);
        SaveDatabase();
    }
    
    static void RemoveVehicle(string vehicleID)
    {
        if (vehicleID == "" || !m_Database)
            return;
        
        if (m_Database.Contains(vehicleID))
        {
            m_Database.Remove(vehicleID);
            SaveDatabase();
            KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Removed vehicle %1 from database", vehicleID));
        }
    }
    
    static void LoadAllVehicles()
    {
        int dbCount = -1;
        if (m_Database)
            dbCount = m_Database.Count();
        KOTH_Log.LogBasic(string.Format("Vehicle KOTH: LoadAllVehicles called, database count: %1", dbCount));
        
        if (!m_Database || m_Database.Count() == 0)
        {
            KOTH_Log.LogBasic("Vehicle KOTH: No vehicles to load (database empty)");
            m_IsFullyLoaded = true;
            StartAutoSave();
            return;
        }
        
        KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Starting to load %1 vehicles", m_Database.Count()));
        
        foreach (string id, KOTH_VehicleEntry entry : m_Database)
        {
            KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Loading vehicle %1 (%2)", entry.classname, entry.vehicleID));
            SpawnVehicleFromEntry(entry);
        }
        
        KOTH_Log.LogBasic("Vehicle KOTH: Finished loading all vehicles");
        StartAutoSave();
    }
    
    private static void SpawnVehicleFromEntry(KOTH_VehicleEntry entry)
    {
        Object obj = GetGame().CreateObjectEx(entry.classname, entry.position, ECE_PLACE_ON_SURFACE | ECE_UPDATEPATHGRAPH | ECE_CREATEPHYSICS, RF_DEFAULT);
        
        if (!obj)
        {
            KOTH_Log.LogCritical(string.Format("Vehicle KOTH: Failed to spawn %1", entry.classname));
            return;
        }
        
        Car vehicle = Car.Cast(obj);
        if (!vehicle)
        {
            GetGame().ObjectDelete(obj);
            return;
        }
        
        CarScript vehicleScript = CarScript.Cast(vehicle);
        if (vehicleScript)
            vehicleScript.SetKOTHVehicle(true, entry.vehicleID);
        
        vehicle.SetOrientation(entry.orientation);
        vehicle.PlaceOnSurface();
        vehicle.SetHealth("", "", entry.health * vehicle.GetMaxHealth("", ""));
        
        foreach (KOTH_AttachmentEntry attEntry : entry.attachments)
        {
            EntityAI att = vehicle.GetInventory().CreateAttachment(attEntry.classname);
            if (att)
            {
                att.SetHealth("", "", attEntry.health * att.GetMaxHealth("", ""));
                
                if (attEntry.quantity > -1)
                {
                    ItemBase itemAtt = ItemBase.Cast(att);
                    if (itemAtt)
                        itemAtt.SetQuantity(attEntry.quantity);
                }
            }
        }
        
        vehicle.Fill(CarFluid.FUEL, vehicle.GetFluidCapacity(CarFluid.FUEL) * entry.fuel);
        vehicle.Fill(CarFluid.COOLANT, vehicle.GetFluidCapacity(CarFluid.COOLANT) * entry.coolant);
        vehicle.Fill(CarFluid.OIL, vehicle.GetFluidCapacity(CarFluid.OIL) * entry.oil);
        vehicle.Fill(CarFluid.BRAKE, vehicle.GetFluidCapacity(CarFluid.BRAKE) * entry.brake);
        
        foreach (string cargoItem : entry.cargoItems)
        {
            vehicle.GetInventory().CreateInInventory(cargoItem);
        }
        
        vehicle.SetSynchDirty();
        
        KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Restored vehicle %1 (%2)", entry.classname, entry.vehicleID));
    }
    
    static void AutoSave()
    {
        if (!m_IsFullyLoaded)
        {
            KOTH_Log.LogBasic("Vehicle KOTH: Skipping auto-save (system not fully loaded yet)");
            return;
        }
        
        int dbCount2 = 0;
        if (m_Database)
            dbCount2 = m_Database.Count();
        KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Auto-spawn initialized; %1 vehicles in database", dbCount2));
        SaveDatabase();
    }
    
    private static void SaveDatabase()
    {
        if (!m_Database)
            return;
        
        array<ref KOTH_VehicleEntry> entries = new array<ref KOTH_VehicleEntry>;
        
        foreach (string id, KOTH_VehicleEntry entry : m_Database)
        {
            entries.Insert(entry);
        }
        
        JsonFileLoader<array<ref KOTH_VehicleEntry>>.JsonSaveFile(DATABASE_FILE, entries);
        KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Saved %1 vehicles to database", entries.Count()));
    }
    
    private static void LoadDatabase()
    {
        if (!FileExist(DATABASE_FILE))
        {
            KOTH_Log.LogBasic("Vehicle KOTH: No database file found, starting fresh");
            return;
        }
        
        array<ref KOTH_VehicleEntry> entries = new array<ref KOTH_VehicleEntry>;
        JsonFileLoader<array<ref KOTH_VehicleEntry>>.JsonLoadFile(DATABASE_FILE, entries);
        
        foreach (KOTH_VehicleEntry entry : entries)
        {
            m_Database.Insert(entry.vehicleID, entry);
        }
        
        KOTH_Log.LogBasic(string.Format("Vehicle KOTH: Loaded %1 vehicles from database file", entries.Count()));
    }
}