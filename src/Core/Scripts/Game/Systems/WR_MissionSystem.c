class WR_MissionSystem : GameSystem
{
	ref WR_Logger<WR_MissionSystem> logger = new WR_Logger<WR_MissionSystem>(this);
	
	[Attribute("", UIWidgets.Object, "Mission system configuration.")];
	protected ref WR_MissionSystemConfig m_Config;
	
	float m_fMissionCreationTimeElaspedS = 0;
	float m_fMissionCreationTickrateS = 15.0;

	ref SCR_WeightedArray<WR_MissionDefinition> m_aDefinitions;
	ref array<ref WR_Mission> m_aMissions = {};
	WR_MissionDefinition m_LastMissionDefinition = null;
	
	WR_MissionNotificationComponent m_NotifComponent;
	
	private override void OnStarted()
	{
		logger.LogNormal("Starting mission system...");
		
		if (!m_Config)
		{
			logger.LogError("No mission system config was specified! Cannot create any missions.");
			this.Enable(false);
		}
		
		InitializeDefinitionArray();
		logger.LogNormal("Mission system started.");
	}
	
	private override void OnUpdate(ESystemPoint point)
	{
		// Advance time elapsed
		float timeSlice = GetWorld().GetTimeSlice();
		m_fMissionCreationTimeElaspedS += timeSlice;
		
		// Check if mission needs creation
		if (m_fMissionCreationTimeElaspedS >= m_fMissionCreationTickrateS)
		{
			if (GetActiveMissionCount() < GetMaxActiveMissionSlots())
				TryCreateNewMission();
			
			m_fMissionCreationTimeElaspedS = 0;
		}
		
		if (!m_aMissions || m_aMissions.Count() == 0)
			return;
		
		// Advance mission statuses
		for (int i = 0; i < m_aMissions.Count();)
		{
			if (AdvanceMissionStatus(m_aMissions[i]))
				i++;
		
			// Only increment index if the mission was not deleted.
			// Avoids index out of bounds errors.
		}
	}
	
	private override void OnCleanup()
	{
		logger.LogNormal("Mission system stopped.");
	}
	
	private void InitializeDefinitionArray()
	{
		m_aDefinitions = new SCR_WeightedArray<WR_MissionDefinition>();
		foreach (WR_MissionDefinition def : m_Config.m_aMissionDefinitions)
		{
			if (def.m_iWeight == 0)
				continue;
			
			m_aDefinitions.Insert(def, def.m_iWeight);
		}
	}
	
	private bool AdvanceMissionStatus(WR_Mission mission)		//Returns false if a mission was deleted this pass
	{
		if (!m_NotifComponent)
			SetMissionNotificationComponent();
		
		float delay;
		WorldTimestamp now = GetGame().GetWorld().GetTimestamp();
		WR_StaticMarkerParameters markerParameters = mission.GetMarkerParameters();
		
		switch (mission.GetStatus())
		{
			case WR_MissionStatus.Pending:
			{
				delay = WR_Utils.MinutesToMilliseconds(m_Config.m_fNewMissionNotificationDelayM);
				
				// Check if mission ready to start
				if (now.DiffMilliseconds(mission.GetLastTimestamp()) > delay)
				{
					if (mission.StartMission())
					{
						int markerId = WR_StaticMarkerHelper.ShowMarker(markerParameters);
						mission.SetMarkerId(markerId);
						
						m_NotifComponent.SendNotification(mission);
					}
					else
					{
						ChangeMissionLocation(mission);
					}
				}
				
				break;
			}
			case WR_MissionStatus.Malformed:
			{
				WR_StaticMarkerHelper.DeleteMarker(mission.GetMarkerId());
				mission.GetLocation().SetIsHostingMission(false);
				mission.DeleteMissionEntities(includeRewards: true);
				m_aMissions.RemoveItemOrdered(mission);
				
				return false;
			}
			case WR_MissionStatus.InProgress:
			{
				delay = WR_Utils.MinutesToMilliseconds(m_Config.m_fMissionTimeLimitM);
				
				// Check for reward stolen
				if (mission.AreAllRewardsStolen())
				{
					mission.SetMissionStatus(WR_MissionStatus.Complete);
					mission.SetCompletionType(WR_MissionCompletionType.Stolen);
				}
				
				// Check for mission timeout
				if (now.DiffMilliseconds(mission.GetLastTimestamp()) > delay)
				{
					mission.SetMissionStatus(WR_MissionStatus.Complete);
					mission.SetCompletionType(WR_MissionCompletionType.TimedOut);
					mission.DeleteMissionEntities(includeRewards: true);
				}
				
				break;
			}
			case WR_MissionStatus.Complete:
			{
				int newMarkerId = WR_StaticMarkerHelper.UpdateMarker(mission.GetMarkerId(), markerParameters);
				mission.SetMarkerId(newMarkerId);
				
				m_NotifComponent.SendNotification(mission);
				
				logger.LogNormal(string.Format("Mission ended: %1 (ID: %2)", mission.GetDefinition().m_sName, mission.GetMissionId()));
				mission.SetMissionStatus(WR_MissionStatus.AwaitingMarkerCleanup);
				
				break;
			}

			case WR_MissionStatus.AwaitingMarkerCleanup:
			{
				delay = WR_Utils.MinutesToMilliseconds(m_Config.m_fMissionMapMarkerCleanupDelayM);
				
				// Check if marker ready to be cleaned up
				if (now.DiffMilliseconds(mission.GetLastTimestamp()) > delay)
				{
					WR_StaticMarkerHelper.DeleteMarker(mission.GetMarkerId());
					mission.SetMissionStatus(WR_MissionStatus.AwaitingCleanup);
					logger.LogNormal(string.Format("Deleted mission map marker: %1 (ID: %2)", mission.GetDefinition().m_sName, mission.GetMissionId()));
				}
				
				break;
			}
			case WR_MissionStatus.AwaitingCleanup:
			{
				delay = WR_Utils.MinutesToMilliseconds(m_Config.m_fMissionCleanupDelayM);
				
				//Check if mission is ready to be cleaned up
				if (now.DiffMilliseconds(mission.GetLastTimestamp()) > delay)
				{
					mission.DeleteMissionEntities(includeRewards: true);
					logger.LogNormal(string.Format("Mission entities cleaned up: %1 (ID: %2)", mission.GetDefinition().m_sName, mission.GetMissionId()));
					mission.GetLocation().SetIsHostingMission(false);
					m_aMissions.RemoveItemOrdered(mission);
					return false;
				}
				break;
			}
			default:
			{
				logger.LogError(string.Format("Invalid mission status: %1 (ID: %2)", mission.GetDefinition().m_sName, mission.GetMissionId()));
			}
			
		}
		return true;
	}
	
	private void TryCreateNewMission()
	{
		WR_MissionDefinition definition = GetRandomUniqueMissionDefinition();
		if (!definition)
		{
			logger.LogError("Unable to generate unique mission definition! Cannot start new mission.");
			return;
		}
		
		// Choose a random vacant location for the mission
		WR_MissionLocationEntity location = GetRandomVacantMissionLocation(definition.m_eSize);
		if (!location)
		{
			logger.LogError("No vacant mission locations! Cannot start new mission.");
			return;
		}
		
		// Initialize new mission and increment mission count. If mission fails to spawn, AdvanceMissionStatuses catches it next frame anyways so we dont handle it here
		WR_Mission mission = new WR_Mission(location, definition);

		logger.LogNormal(string.Format("Mission created: %1 (ID: %2, Location: %3)", mission.GetDefinition().m_sName, mission.GetMissionId(), mission.GetLocation().GetName()));
			
		
		// Send preliminary notification if a mission start delay exists
		if (m_Config.m_fNewMissionNotificationDelayM > 0)
		{
			if (!m_NotifComponent)
				SetMissionNotificationComponent();

			m_NotifComponent.SendNotification(mission);
		}

		m_LastMissionDefinition = definition;
		location.SetIsHostingMission(true);
		m_aMissions.Insert(mission);
	}
	
	private void ChangeMissionLocation(WR_Mission mission) //Called when a mission fails to spawn at the original location
	{
		WR_MissionLocationEntity location = GetRandomVacantMissionLocation(mission.GetDefinition().m_eSize);
		if (!location)
		{
			logger.LogError("No vacant mission locations! Cannot start new mission.");
			return;
		}
		
		mission.m_Location = location;
		logger.LogNormal(string.Format("Mission location changed for: %1 (ID: %2, Location: %3)", mission.GetDefinition().m_sName, mission.GetMissionId(), mission.GetLocation().GetName()));
	}
	
	private void SetMissionNotificationComponent()
	{
		WR_GameModeWasteland gameMode = WR_GameModeWasteland.Cast(GetGame().GetGameMode());
		if (!gameMode)
			logger.LogError("Wasteland game mode entity not found! Cannot send mission notifications. Place a 'GameMode_Wasteland' prefab in the world to resolve.");
		
		WR_MissionNotificationComponent notifComponent = WR_MissionNotificationComponent.Cast(gameMode.FindComponent(WR_MissionNotificationComponent));
		if (!notifComponent)
			logger.LogError("Mission notification component not found on game mode! Cannot send mission notifications. Add a 'WR_MissionNotificationComponent' to the game mode to resolve.");
		
		m_NotifComponent = notifComponent;
	}
	
	private int GetMaxActiveMissionSlots()
	{
		if (m_Config.m_aPlayercountMissionThresholds.Count() == 0)
		{
			logger.LogWarning("No player count thresholds set in the mission system config. Cannot start a mission.");
			return 0;
		}
		
		// Get number of players currently in the game
		int playerCount = GetGame().GetPlayerManager().GetPlayerCount();
		
		// Ensure threshold list is sorted
		array<int> thresholds = m_Config.m_aPlayercountMissionThresholds;
		thresholds.Sort();
		
		int slots = 0;
		foreach (int threshold : m_Config.m_aPlayercountMissionThresholds)
		{
			if (playerCount >= threshold)
				slots += 1;
			else
				break;
		}
		
		return slots;
	}
	
	private int GetActiveMissionCount()
	{
		int count = 0;
		foreach (WR_Mission mission : m_aMissions)
		{
			WR_MissionStatus status = mission.GetStatus();
			if (	status == WR_MissionStatus.Pending || status == WR_MissionStatus.InProgress)
				count += 1;
		}
		
		return count;
	}
	
	// Return a mission definition that is different than the last one spawned. If only one definition exists, returns that one.
	private WR_MissionDefinition GetRandomUniqueMissionDefinition()
	{
		if (!m_aDefinitions || m_aDefinitions.Count() == 0)
		{
			logger.LogError("No mission definitions have been defined! Cannot start new mission.");
			return null;
		}
		
		if (!ValidateMissionDifficulties())
		{
			logger.LogError("No mission definitions found that meet difficulty playercount threshold requirements! Cannot start new mission.");
			return null;
		}
		
		if (m_aDefinitions.Count() == 1)
		{
			return m_aDefinitions[0];
		}

		int currentPlayers = GetGame().GetPlayerManager().GetPlayerCount();
		WR_MissionDefinition definition = null;
		
		int maxAttempts = 1000;
		for (int i = 0; i < maxAttempts; i++)
		{
			m_aDefinitions.GetRandomValue(definition);
			if (!definition) break;
			
			// With reasonable certainty, ensure the chosen mission is different from the previously started mission
			// and is within the playercount thresholds for difficulty.
			if (m_LastMissionDefinition && definition.m_eType == m_LastMissionDefinition.m_eType)
				continue;
			if (definition.m_eDifficulty == WR_MissionDifficulty.FREE && currentPlayers < m_Config.m_freeMissionThreshold)
				continue;
			if (definition.m_eDifficulty == WR_MissionDifficulty.HARD && currentPlayers < m_Config.m_hardMissionThreshold)
				continue;
			
			break;
		}

		return definition;
	}
	
	private WR_MissionLocationEntity GetRandomVacantMissionLocation(WR_MissionLocationSize requiredSize)
	{			
		array<WR_MissionLocationEntity> vacantLocations = WR_MissionLocationEntity.GetAllVacantLocations(requiredSize);
		if (vacantLocations.Count() == 0)
			return null;

		return vacantLocations.GetRandomElement();
	}
	
	// Ensures that, given the relevant mission difficulty playercount thresholds are greater than 0, at least one easy mission is present in the mission system config.
	private bool ValidateMissionDifficulties()
	{
		// If no thresholds are set, all mission difficulties are valid, regardless of current player population.
		if (m_Config.m_freeMissionThreshold == 0 && m_Config.m_freeMissionThreshold == 0)
			return true;
			
		// Find one mission with easy difficulty.
		array<WR_MissionDefinition> missionDefs = {};
		m_aDefinitions.ToArray(missionDefs);
		foreach (WR_MissionDefinition missionDef : missionDefs)
		{
			if (missionDef.m_eDifficulty == WR_MissionDifficulty.EASY)
				return true;
		}
		
		return false;
	}
}