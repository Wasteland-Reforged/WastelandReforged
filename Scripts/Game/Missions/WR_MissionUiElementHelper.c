class WR_MissionMarkerHelper
{
	static SCR_MapMarkerBase CreateMarker(notnull WR_Mission mission)
	{	
		ref SCR_MapMarkerBase marker = new SCR_MapMarkerBase();
		
		// Set Marker Properties
		marker.SetType(SCR_EMapMarkerType.PLACED_CUSTOM);
		marker.SetCustomText(mission.GetDefinition().m_sName);
		marker.SetIconEntry(mission.GetDefinition().m_eMissionIcon);
		marker.SetColorEntry(mission.GetDefinition().m_eMissionColor);
		marker.SetWorldPos(mission.GetLocation().GetOrigin()[0], mission.GetLocation().GetOrigin()[2]);

		return marker;
	}
	
	static void ShowMarker(SCR_MapMarkerBase marker)
	{
		// Get marker manager
		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
		//SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		
		if (!markerManager) {
			Print("[WASTELAND] WR_MissionManagerComponent: Marker manager is null.", LogLevel.ERROR);
			return;
		}

		markerManager.InsertStaticMarker(marker, isLocal: false, isServerMarker: true);	
	}
	
	static void DeleteMarker(SCR_MapMarkerBase marker)
	{
		if (!marker)
			return;
		
		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
		//SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		
		if (!markerManager)
		{
			Print("[WASTELAND] WR_MissionManagerComponent: Marker manager is null.", LogLevel.ERROR);
			return;
		}
		
		markerManager.RemoveStaticMarker(marker);
	}
	
	static void UpdateMarkerColor(SCR_MapMarkerBase marker, int color)
	{
		if (!marker)
			return;
		
		DeleteMarker(marker);
		marker.SetColorEntry(color);
		ShowMarker(marker)
	}
	

}