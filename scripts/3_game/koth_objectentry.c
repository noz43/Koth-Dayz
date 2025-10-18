class KOTH_ObjectEntry
{
	string classname;
	vector position;
	vector orientation;
	bool absolutePlacement;
	bool alignToTerrain;
	bool placeOnSurface;

	void KOTH_ObjectEntry()
	{

	}

	void KOTH_ObjectEntry(string name, vector pos, vector ori, bool abs = false, bool align = true, bool place = true)
	{
		classname = name;
		position = pos;
		orientation = ori;
		absolutePlacement = abs;
		alignToTerrain = align;
		placeOnSurface = place;
	}

	void InitDefaults()
	{
		classname = "Wreck_Mi8";
		position = Vector(15, 0, 5);
		orientation = Math3D.GetRandomDir().VectorToAngles();
		absolutePlacement = false;
		alignToTerrain = true;
		placeOnSurface = true;
	}

	string GetClassName()
	{
		return classname;
	}

	vector GetPosition()
	{
		return position;
	}

	vector GetOrientation()
	{
		return orientation;
	}

	bool IsAbsolutePlacement()
	{
		return absolutePlacement;
	}
	
	bool IsAlignToTerrain()
	{
		return alignToTerrain;
	}
	
	bool IsPlaceOnSurface()
	{
		return placeOnSurface;
	}
}