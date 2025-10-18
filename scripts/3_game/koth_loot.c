class KOTH_Loot
{
	private static string m_Directory = "$profile:KOTH-SAVIOR";
	private static string m_Path = m_Directory + "/Loot.json";
	
	static ref KOTH_LootData m_Data = new KOTH_LootData;

	protected static void OnLoaded()
	{
		KOTH_Log.LogVerbose("Loot config loaded.");
	}

	static bool LoadData()
	{
		if (!FileExist(m_Directory))
			MakeDirectory(m_Directory);

		if (!FileExist(m_Path))
		{
			KOTH_Log.LogVerbose("Writing default loot config.");
			m_Data.InitDefaults();
			SaveData();
		}
		else
		{
			JsonFileLoader<KOTH_LootData>.JsonLoadFile( m_Path, m_Data );
			SaveData(); 
		}
		
		OnLoaded();

		return true;
	}
	
	static bool SaveData()
	{
		JsonFileLoader<KOTH_LootData>.JsonSaveFile( m_Path, m_Data );
		return true;
	}
	
	static array<ref KOTH_LootSet> GetDesiredRewards(array<string> setNames = NULL)
	{
		return m_Data.GetDesiredRewards(setNames);
	}
}