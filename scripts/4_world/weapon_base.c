modded class Weapon_Base extends Weapon
{
	Magazine SpawnAttachedMagazineKOTH( string magazineType = "", int flags = WeaponWithAmmoFlags.CHAMBER )
	{
		if ( GetMagazineTypeCount(0) == 0 )
			return null;
		

		if ( magazineType == "" )
		{
			if ( flags & WeaponWithAmmoFlags.MAX_CAPACITY_MAG)
				magazineType = GetMaxMagazineTypeName(0);
			else
				magazineType = GetRandomMagazineTypeName(0);
		}
		
		EntityAI magAI = GetInventory().CreateAttachment(magazineType);
		if (!magAI)
			return null;
		
		Magazine mag;
		if (!CastTo(mag, magAI))
			return null;
		
		if (flags & WeaponWithAmmoFlags.QUANTITY_RNG)
			mag.ServerSetAmmoCount(Math.RandomIntInclusive(0, mag.GetAmmoMax()));
		
		bool chamberRng = (flags & WeaponWithAmmoFlags.CHAMBER_RNG);
		bool chamber = (flags & WeaponWithAmmoFlags.CHAMBER) || chamberRng;		
		if (chamber || chamberRng)
			FillChamber(magazineType, flags);

		RandomizeFSMState();
		Synchronize();

		return mag;
	}

	ItemBase SpawnAttachmentKOTH(string attachmentClassname, out bool isMagazine)
	{
		EntityAI attachment = GetInventory().CreateAttachment(attachmentClassname);

		ItemBase item;
		if (!attachment || !Class.CastTo(item, attachment))
			return null;

		isMagazine = false;

		Magazine mag;
		if (!CastTo(mag, attachment))
		{
			return item;
		}
		
		FillChamber(attachmentClassname, WeaponWithAmmoFlags.CHAMBER);
		RandomizeFSMState();
		Synchronize();

		isMagazine = true;
		return item;
	}
}