#ifdef VanillaPPMap
modded class ServerMarkersCache extends JsonCacheManager<VPPMapConfig> 
{
	protected ref array<ref MarkerInfo> m_VPPKOTHMarkers = new array<ref MarkerInfo>;
	
	array<ref MarkerInfo> GetKOTHMarkers()
	{
		return m_VPPKOTHMarkers;
	}

	void AddKOTHMarker(MarkerInfo newMarker)
	{
		if (m_VPPKOTHMarkers.Find(newMarker) > -1)
			return;
		
		m_VPPKOTHMarkers.Insert(newMarker);
	}

	void RemoveKOTHMarker(MarkerInfo marker)
	{
		int markerIndex = m_VPPKOTHMarkers.Find(marker);
		if (markerIndex > -1)
			m_VPPKOTHMarkers.Remove(markerIndex);
	}

    override ref array<ref MarkerInfo> GetCachedArray(string addressPort = "") 
	{
		array<ref MarkerInfo> oldMarkerArray = super.GetCachedArray(addressPort);
		array<ref MarkerInfo> newMarkerArray = new array<ref MarkerInfo>;

		foreach ( MarkerInfo oldMarker : oldMarkerArray )
		{
			newMarkerArray.Insert(oldMarker);
		}


		foreach ( MarkerInfo newMarker : m_VPPKOTHMarkers )
		{
			newMarkerArray.Insert(newMarker);
		}
		
        return newMarkerArray;
    }
}

modded class DayZGame 
{
	override void SetServerMarkers(ref array<ref MarkerInfo> staticMarkers) 
	{
		m_ServerMarkers.Clear();

        foreach (MarkerInfo marker : staticMarkers)
		{
			m_ServerMarkers.Insert(marker);
		}

        foreach(ref MarkerInfo mkr: m_ServerMarkers)
        {
            mkr.Set3DForcedDisabled(!mkr.Is3DActive());
        }

        m_ServerMarkersSet = true;
        Display3dMarkers();
    }
}
#endif