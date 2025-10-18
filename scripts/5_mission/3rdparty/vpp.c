#ifdef VanillaPPMap
modded class MissionServer extends MissionBase
{
	void AddKOTHMarkerVPP(MarkerInfo newMarker)
	{
		m_ServerMarkersCache.AddKOTHMarker(newMarker);
	}

	void RemoveKOTHMarkerVPP(MarkerInfo marker)
	{
		m_ServerMarkersCache.RemoveKOTHMarker(marker);
	}
	
	void UpdateKOTHMarkersVPP()
	{
		if (KOTH_Settings.IsUseMapMarker())
		{
			array<ref MarkerInfo> markerInfos = m_ServerMarkersCache.GetCachedArray();
			GetRPCManager().SendRPC( "RPC_MissionGameplay", "SessionMarkers", new Param1<ref array<ref MarkerInfo>>(markerInfos), true );
		}
	}
}
#endif