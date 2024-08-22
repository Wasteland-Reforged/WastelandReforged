class WR_MissionUiElementHelper
{
	// TODO: make this configurable from world editor
	private static string m_sSoundEventNameNotification = "SOUND_HUD_TASK_CREATED";
	
	static void SendMissionNotificationByStatus(WR_Mission mission)
	{
		string title = "";
		string description = "";
		
		switch (mission.GetStatus())
		{
			case WR_MissionStatus.InProgress:
				title = "New Objective: " + mission.GetName();
				description = mission.GetDescription();
				break;
			case WR_MissionStatus.Complete:
				title = "Objective Complete: " + mission.GetName();
				description = "The mission was completed.";
				break;
			default:
				break;
		}
				
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
	
	// TODO: maybe change this to be a create or update method?
	static void CreateMarker(WR_Mission mission)
	{
		ref SCR_MapMarkerBase marker = new SCR_MapMarkerBase();
		
		marker.SetType(SCR_EMapMarkerType.PLACED_CUSTOM);
		marker.SetCustomText(mission.GetName());
		marker.SetIconEntry(mission.GetDefinition().m_eMissionIcon);
		marker.SetColorEntry(mission.GetDefinition().m_eMissionColor);
		marker.SetWorldPos(mission.GetPosition()[0], mission.GetPosition()[2]);
		
		// Save marker record to mission object so we can update or delete it from the map later
		mission.SetMarker(marker);
		// TODO: return this, dont set it here
		
		// Draw marker on map
		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
		if (!markerManager)
		{
			Print("[WASTELAND] WR_MissionManagerComponent: Marker manager is null.", LogLevel.ERROR);
			return;
		}

		//markerManager.InsertStaticMarker(marker, isLocal: false, isServerMarker: true);
		markerManager.InsertStaticMarker(marker, isLocal: false, isServerMarker: true);
	}
	
	static void DeleteMarker(WR_Mission mission)
	{
		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
		markerManager.RemoveStaticMarker(mission.GetMarker());
	}
}