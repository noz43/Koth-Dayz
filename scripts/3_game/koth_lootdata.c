class KOTH_LootData
{
	ref array<ref KOTH_LootSet> lootSets = new array<ref KOTH_LootSet>;

	void InitDefaults()
	{
		KOTH_LootSet newSet;
		KOTH_LootItem newItem;
		KOTH_LootItem subItem;
		KOTH_LootItem subSubItem;
		KOTH_LootItem subSubSubItem;
		KOTH_LootItem subSubSubSubItem;
		
		newSet = RegisterSet("AK74");
		newItem = newSet.RegisterItem("AK74");
		subItem = newItem.AddAttachment("AK74_Hndgrd");
		subItem = newItem.AddAttachment("Mag_AK74_30Rnd", 20);
		subItem = newItem.AddAttachment("AK74_WoodBttstck");
		subItem = newItem.AddAttachment("KobraOptic");
		subSubItem = subItem.AddAttachment("Battery9V");
		subSubItem.SetQuantity(50);
		
		newSet = RegisterSet("AKM");
		newItem = newSet.RegisterItem("AKM");
		subItem = newItem.AddAttachment("Mag_AKM_Drum75Rnd", 20);
		subItem = newItem.AddAttachment("AK_RailHndgrd");
		subItem = newItem.AddAttachment("AK_PlasticBttstck");
		subItem = newItem.AddAttachment("PSO1Optic");
		subSubItem = subItem.AddAttachment("Battery9V", 15);
		subSubItem.SetQuantity(50);
		
		newSet = RegisterSet("Ammo Pouch");
		newItem = newSet.RegisterItem("PlateCarrierPouches");
		subItem = newItem.AddCargo("Ammo_556x45", 15);
		subItem = newItem.AddCargo("Ammo_556x45", 15);
		subItem = newItem.AddCargo("Ammo_556x45", 15);
		subItem = newItem.AddCargo("Ammo_556x45", 15);
		subItem = newItem.AddCargo("Ammo_308Win", 10);
		subItem = newItem.AddCargo("Ammo_308Win", 10);
		subItem = newItem.AddCargo("Ammo_308Win", 10);
		subItem = newItem.AddCargo("Ammo_308Win", 10);

		newSet = RegisterSet("LoadedPlate");
		newItem = newSet.RegisterItem("PlateCarrierVest");
		subItem = newItem.AddAttachment("PlateCarrierHolster");
		subSubItem = subItem.AddAttachment("FNX45");
		subSubSubItem = subSubItem.AddAttachment("PistolSuppressor");
		subSubSubItem = subSubItem.AddAttachment("Mag_FNX45_15Rnd");
		subSubSubItem = subSubItem.AddAttachment("FNP45_MRDSOptic");
		subSubSubSubItem = subSubSubItem.AddAttachment("Battery9v");
		subItem = newItem.AddAttachment("PlateCarrierPouches");
		subSubItem = subItem.AddCargo("Mag_FNX45_15Rnd");
		subSubItem = subItem.AddCargo("Mag_FNX45_15Rnd");
		subSubItem = subItem.AddCargo("Mag_FNX45_15Rnd");
		subSubItem = subItem.AddCargo("Mag_FNX45_15Rnd");
		subSubItem = subItem.AddCargo("Mag_FNX45_15Rnd");
		subItem = newItem.AddAttachment("M18SmokeGrenade_Red");
		subItem = newItem.AddAttachment("M18SmokeGrenade_Green");
		subItem = newItem.AddAttachment("M18SmokeGrenade_Purple");
		subItem = newItem.AddAttachment("M18SmokeGrenade_White");

		newSet = RegisterSet("VehicleRepair");
		newItem = newSet.RegisterItem("FieldShovel");
		newItem = newSet.RegisterItem("Wrench");
		newItem = newSet.RegisterItem("Screwdriver");
		newItem = newSet.RegisterItem("Pliers");
		newItem = newSet.RegisterItem("LugWrench");
		newItem = newSet.RegisterItem("TireRepairKit");
		newItem = newSet.RegisterItem("SparkPlug");
		newItem = newSet.RegisterItem("CarBattery");
		newItem = newSet.RegisterItem("CarRadiator");
		newItem = newSet.RegisterItem("HeadlightH7");
		newItem = newSet.RegisterItem("HeadlightH7");
		newItem = newSet.RegisterItem("EngineBelt");
		
		newSet = RegisterSet("MilitaryVehicle");
		newItem = newSet.RegisterItem("M4A1");
		subItem = newItem.AddAttachment("M4_RISHndgrd");
		subItem = newItem.AddAttachment("M4_MPBttstck");
		subItem = newItem.AddAttachment("Mag_STANAG_30Rnd", 30);
		subItem = newItem.AddAttachment("M4_Suppressor");
		subItem = newItem.AddAttachment("ACOGOptic");
		subSubItem = subItem.AddAttachment("Battery9V", 75);
		newItem = newSet.RegisterItem("HighCapacityVest_Black");
		subItem = newItem.AddCargo("Mag_STANAG_30Rnd", 30);
		subItem = newItem.AddCargo("Mag_STANAG_30Rnd", 30);
		subItem = newItem.AddCargo("Mag_STANAG_30Rnd", 30);
		subItem = newItem.AddCargo("Mag_STANAG_30Rnd", 30);
		subItem = newItem.AddCargo("RGD5Grenade");
		subItem = newItem.AddCargo("RGD5Grenade");
		newItem = newSet.RegisterItem("GorkaEJacket_Summer");
		newItem = newSet.RegisterItem("GorkaPants_Summer");
		newItem = newSet.RegisterItem("MilitaryBoots_Bluerock");
		newItem = newSet.RegisterItem("Beret_Black");
		
		newSet = RegisterSet("SurvivalKit");
		newItem = newSet.RegisterItem("MountainBag_Blue");
		subItem = newItem.AddCargo("SalineBag");
		subItem = newItem.AddCargo("SalineBag");
		subItem = newItem.AddCargo("BloodBagFull");
		subItem = newItem.AddCargo("Morphine");
		subItem = newItem.AddCargo("Epinephrine");
		subItem = newItem.AddCargo("Bandage");
		subItem = newItem.AddCargo("Bandage");
		subItem = newItem.AddCargo("Bandage");
		subItem = newItem.AddCargo("DisinfectantSpray", 50);
		subItem = newItem.AddCargo("CharcoalTablets");
		subItem = newItem.AddCargo("TetracyclineAntibiotics");
		subItem = newItem.AddCargo("PurificationTablets");
		subItem = newItem.AddCargo("CanOpener");
		subItem = newItem.AddCargo("Matchbox");
		subItem = newItem.AddCargo("PeachesCan");
		subItem = newItem.AddCargo("BakedBeansCan");
		subItem = newItem.AddCargo("WaterBottle", 100);
		newItem = newSet.RegisterItem("FirefighterAxe");
		newItem = newSet.RegisterItem("CombatKnife");
		newItem = newSet.RegisterItem("Compass");
		newItem = newSet.RegisterItem("Binoculars");
	}
	
	array<ref KOTH_LootSet> GetDesiredRewards(array<string> setNames = NULL)
	{
		if (setNames && setNames.Count() > 0)
		{
			array<ref KOTH_LootSet> desiredSets = new array<ref KOTH_LootSet>;
			for (int i = 0; i < lootSets.Count(); i++)
			{
				if (setNames.Find(lootSets[i].GetName()) > -1)
					desiredSets.Insert(lootSets[i]);
			}
			
			return desiredSets;
		}
		else
		{
			return lootSets;
		}
	}

	KOTH_LootSet RegisterSet(string name)
	{
		KOTH_LootSet lootSet = new KOTH_LootSet(name);
		lootSets.Insert(lootSet);

		return lootSet;
	}
}