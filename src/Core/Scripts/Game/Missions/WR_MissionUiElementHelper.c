class WR_StaticMarkerHelper
{
	static int ShowMarker(WR_StaticMarkerParameters parameters)
	{
		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
		
		if (!markerManager)
		{
			Print("[WASTELAND] WR_StaticMarkerHelper: Marker manager is null.", LogLevel.ERROR);
			return -1;
		}

		SCR_MapMarkerBase marker = GetMarker(parameters);
		markerManager.InsertStaticMarker(marker, isLocal: false, isServerMarker: true);
		
		int markerId = marker.GetMarkerID();
		return markerId;
	}
	
	static void DeleteMarker(int markerId)
	{
		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
		
		if (!markerManager)
		{
			Print("[WASTELAND] WR_StaticMarkerHelper: Marker manager is null.", LogLevel.ERROR);
			return;
		}
		
		SCR_MapMarkerBase marker = markerManager.GetStaticMarkerByID(markerId);
		if (!marker)
		{
			Print("[WASTELAND] WR_StaticMarkerHelper: Marker with ID " + markerId + " is null.", LogLevel.WARNING);
			return;
		}
		
		markerManager.RemoveStaticMarker(marker);
	}
	
	static int UpdateMarker(int markerId, WR_StaticMarkerParameters parameters)
	{
		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
		
		if (!markerManager)
		{
			Print("[WASTELAND] WR_StaticMarkerHelper: Marker manager is null.", LogLevel.ERROR);
			return -1;
		}
		
		DeleteMarker(markerId);
		int newMarkerId = ShowMarker(parameters);
		
		return newMarkerId;
	}
	
	private static SCR_MapMarkerBase GetMarker(WR_StaticMarkerParameters parameters)
	{
		ref SCR_MapMarkerBase marker = new SCR_MapMarkerBase();
		
		marker.SetType(SCR_EMapMarkerType.PLACED_CUSTOM);
		marker.SetCustomText(parameters.m_sCustomText);
		marker.SetIconEntry(parameters.m_iIconEntry);
		marker.SetColorEntry(parameters.m_iColorEntry);
		marker.SetWorldPos(parameters.m_vWorldPos[0], parameters.m_vWorldPos[2]);

		return marker;
	}
}