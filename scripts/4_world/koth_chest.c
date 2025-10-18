class KOTH_Chest extends SeaChest
{
	override bool CanPutInCargo( EntityAI parent )
	{
		return false;
	}

	override bool CanPutIntoHands( EntityAI parent )
	{
		return false;
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		
		CheckForDestroy();	
	}
	
	override void EECargoOut(EntityAI item)
	{
		super.EECargoOut(item);

		CheckForDestroy();	
	}

	override bool IsPrepareToDelete()
	{
		return IsEmpty();
	}
}