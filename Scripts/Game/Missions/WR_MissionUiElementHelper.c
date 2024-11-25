

class WR_MissionUiElementHelper
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
		if (!markerManager) {
			Print("[WASTELAND] WR_MissionManagerComponent: Marker manager is null.", LogLevel.ERROR);
			return;
		}

		markerManager.InsertStaticMarker(marker, isLocal: false, isServerMarker: true);	
	}
	
	static void DeleteMarker(SCR_MapMarkerBase marker)
	{
		if (!marker) return;
		
		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
		if (!markerManager) {
			Print("[WASTELAND] WR_MissionManagerComponent: Marker manager is null.", LogLevel.ERROR);
			return;
		}
		
		markerManager.RemoveStaticMarker(marker);
	}
	
	static void UpdateMarkerColor(SCR_MapMarkerBase marker, int color)
	{
		if (!marker) return;
		marker.SetColorEntry(color);
	}
	
	// TODO: make this configurable from world editor
	private static string m_sSoundEventNameNotification = "SOUND_HUD_TASK_CREATED";
	
	static void SendMissionNotificationPending(WR_Mission mission, float delayInMinutes)
	{
		string title = "Objective starting soon: " + mission.GetDefinition().m_sName;
		string description = "Starting in: " + delayInMinutes + " minutes.";
		
		ShowHintWithSound(title, description);
	}
	
	static void SendMissionNotificationStarted(WR_Mission mission)
	{
		string title = "New Objective Started: " + mission.GetDefinition().m_sName;
		string description = mission.GetDefinition().m_sDescription;
		
		ShowHintWithSound(title, description);
	}
	
	static void SendMissionNotificationCompleted(WR_Mission mission)
	{
		string title = "Objective Complete: " + mission.GetDefinition().m_sName;
		string description = "The mission was completed.";
		
		ShowHintWithSound(title, description);
	}
	
	static void SendMissionNotificationTimeout(WR_Mission mission)
	{
		string title = "Objective Timed out: " + mission.GetDefinition().m_sName;
		string description = "The mission was not completed in time.";
		
		ShowHintWithSound(title, description);
	}
	
	static void SendMissionNotificationDestroyed(WR_Mission mission)
	{
		string title = "Objective Destroyed: " + mission.GetDefinition().m_sName;
		string description = "Someone destroyed the mission reward";
		
		ShowHintWithSound(title, description);
	}
	
	static void ShowHintWithSound(string title, string description)
	{
		SCR_HintUIInfo hintInfo = SCR_HintUIInfo.CreateInfo(
			description,
			title,
			10, // TODO: read from config
			EHint.UNDEFINED,
			EFieldManualEntryId.NONE,
			true // TODO: read from config
		);
		
		SCR_HintManagerComponent.ShowHint(hintInfo, isSilent: true, ignoreShown: true);
		PlayNotificationSound();
	}
	
	private static void PlayNotificationSound()
	{
		SCR_UISoundEntity uiSound = SCR_UISoundEntity.GetInstance();
		//uiSound.SetSignalValueStr("", 1.00); // TODO: figure out how to set volume properly
		uiSound.SoundEvent(m_sSoundEventNameNotification, force: true);
	}
	
	
	
	*/
}