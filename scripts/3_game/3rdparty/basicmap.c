#ifdef BASICMAP
modded class BasicMapController extends Managed
{
	override void Init()
	{
		if (IsInit)
			SetMarkers("KOTH", new array<autoptr BasicMapMarker>);
		
		super.Init();
	}
}
#endif