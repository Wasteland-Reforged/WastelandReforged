class WR_MissionSystem : GameSystem
{
	ref WR_Logger<WR_MissionSystem> logger = new WR_Logger<WR_MissionSystem>(this);
	
	[Attribute("", UIWidgets.Object, "Mission system configuration.")];
	protected ref WR_MissionSystemConfig m_Config;
	
	ref SCR_WeightedArray<WR_MissionDefinition> m_aDefinitions;
	ref array<ref WR_Mission> m_aActiveMissions = {};
	WR_MissionDefinition m_LastMissionDefinition = null;
	
	WR_MissionNotificationComponent m_NotifComponent;
	
	ref array<ref WR_Mission> m_aPendingTimedNotifications = {};
	WorldTimestamp m_LastNotificationSendTime = null;
	WorldTimestamp m_WorldStartTime;

	ref array<int> m_aPlayercountMissionThresholds;
	int m_iNotificationDelayMs = WR_Constants.s_iMissionHintDisplayDurationMs + WR_Constants.s_iMissionHintPaddingDurationMs;
	int m_iInitialDelayMs;
	
	private override void OnStarted()
	{
		logger.LogNormal("Starting mission system...");
		
		if (!m_Config)
		{
			logger.LogError("No mission system config was specified! Cannot create any missions.");
			this.Enable(false);
		}
		
		InitializeDefinitions();
		InitializePlayercountThresholds();
		InitializeMissionNotificationComponent();
		
		m_WorldStartTime = GetGame().GetWorld().GetTimestamp();
		m_iInitialDelayMs = WR_Utils.MinutesToMilliseconds(m_Config.m_fInitialMissionDelayM);

		logger.LogNormal("Mission system started.");
	}
	
	private override void OnUpdate(ESystemPoint point)
	{
		// Do nothing until	the initial mission delay time has passed
		WorldTimestamp now = GetGame().GetWorld().GetTimestamp();

		if (now.DiffMilliseconds(m_WorldStartTime) <= m_iInitialDelayMs)
			return;
		
		// Handle timed notifications
		HandleTimedNotificationQueue();
		
		// Handle existing missions
		if (m_aActiveMissions && m_aActiveMissions.Count() > 0)
		{
			for (int i = 0; i < m_aActiveMissions.Count();)
			{
				if (AdvanceMissionStatus(m_aActiveMissions[i]))
					i++;
			
				// Only increment index if the mission was not deleted.
				// Avoids index out of bounds errors.
			}
		}
		
		// Attempt to create a new mission
		TryCreateNewMission();
	}
	
	private override void OnCleanup()
	{
		logger.LogNormal("Mission system cleaned up.");
	}
	
	private override void OnStopped()
	{
		logger.LogNormal("Mission system stopped.");
	}

	private void InitializeDefinitions()
	{
		m_aDefinitions = new SCR_WeightedArray<WR_MissionDefinition>();
		foreach (WR_MissionDefinition def : m_Config.m_aMissionDefinitions)
		{
			m_aDefinitions.Insert(def, def.m_iWeight);
		}
	}

	private void InitializePlayercountThresholds()
	{
		m_aPlayercountMissionThresholds	= m_Config.m_aPlayercountMissionThresholds;
		m_aPlayercountMissionThresholds.Sort(); // Threshold list must be sorted for thresholds to work properly.
	}

	private void InitializeMissionNotificationComponent()
	{
		WR_GameModeWasteland gameMode = WR_GameModeWasteland.Cast(GetGame().GetGameMode());
		if (!gameMode)
		{
			logger.LogError("Wasteland game mode entity not found! Cannot send mission notifications. Place a 'GameMode_Wasteland' prefab in the world to resolve. Stopping mission system...");
			Enable(false);
		}
			
		WR_MissionNotificationComponent notifComponent = WR_MissionNotificationComponent.Cast(gameMode.FindComponent(WR_MissionNotificationComponent));
		if (!notifComponent)
		{
			logger.LogError("Mission notification component not found on game mode! Cannot send mission notifications. Add a 'WR_MissionNotificationComponent' to the game mode to resolve. Stopping mission system...");
			Enable(false);
		}
			
		m_NotifComponent = notifComponent;
	}
	
	private bool AdvanceMissionStatus(WR_Mission mission)	// Returns false if a mission was deleted this pass
	{
		int delayMs;
		WorldTimestamp now = GetGame().GetWorld().GetTimestamp();
		WR_StaticMarkerParameters markerParameters = mission.GetMarkerParameters();
		
		switch (mission.GetStatus())
		{
			case WR_MissionStatus.Pending:
			{
				// Check for mission ready to start
				delayMs = WR_Utils.MinutesToMilliseconds(m_Config.m_fNewMissionNotificationDelayM);
				if (now.DiffMilliseconds(mission.GetLastStatusChangeTime()) > delayMs)
				{
					if (mission.StartMission())
					{
						int markerId = WR_StaticMarkerHelper.ShowMarker(markerParameters);
						mission.SetMarkerId(markerId);
						
						//SendNotification(mission);
						EnqueueTimedNotification(mission);
						
						logger.LogNormal(string.Format("Mission started: %1 (ID: %2)", mission.GetDefinition().m_sName, mission.GetMissionId()));
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
				m_aActiveMissions.RemoveItemOrdered(mission);
				
				return false;
			}
			case WR_MissionStatus.InProgress:
			{
				// Check for reward stolen
				if (mission.AreAllRewardsStolen())
				{
					mission.SetMissionStatus(WR_MissionStatus.Complete);
					mission.SetCompletionType(WR_MissionCompletionType.Stolen);
				}
				
				// Check for mission timeout
				delayMs = WR_Utils.MinutesToMilliseconds(m_Config.m_fMissionTimeLimitM);
				if (now.DiffMilliseconds(mission.GetLastStatusChangeTime()) > delayMs)
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
				
				SendNotification(mission);
				
				logger.LogNormal(string.Format("Mission ended: %1 (ID: %2)", mission.GetDefinition().m_sName, mission.GetMissionId()));
				mission.SetMissionStatus(WR_MissionStatus.AwaitingMarkerCleanup);
				
				break;
			}
			case WR_MissionStatus.AwaitingMarkerCleanup:
			{
				// Check for marker ready to be cleaned up
				delayMs = WR_Utils.MinutesToMilliseconds(m_Config.m_fMissionMapMarkerCleanupDelayM);
				if (now.DiffMilliseconds(mission.GetLastStatusChangeTime()) > delayMs)
				{
					WR_StaticMarkerHelper.DeleteMarker(mission.GetMarkerId());
					mission.SetMissionStatus(WR_MissionStatus.AwaitingCleanup);
					logger.LogNormal(string.Format("Deleted mission map marker: %1 (ID: %2)", mission.GetDefinition().m_sName, mission.GetMissionId()));
				}
				
				break;
			}
			case WR_MissionStatus.AwaitingCleanup:
			{
				delayMs = WR_Utils.MinutesToMilliseconds(m_Config.m_fMissionCleanupDelayM);
				
				// Check for mission ready to be cleaned up
				if (now.DiffMilliseconds(mission.GetLastStatusChangeTime()) > delayMs)
				{
					mission.DeleteMissionEntities(includeRewards: true);
					logger.LogNormal(string.Format("Mission entities cleaned up: %1 (ID: %2)", mission.GetDefinition().m_sName, mission.GetMissionId()));
					mission.GetLocation().SetIsHostingMission(false);
					m_aActiveMissions.RemoveItemOrdered(mission);
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
		// Do not create new mission if there are no slots available
		if (GetActiveMissionCount() >= GetMaxActiveMissionSlots())
			return;
		
		// Do not create new mission if there are pending timed notifications
		if (m_aPendingTimedNotifications.Count() > 0)
			return;

		// Do not create new mission if the relevant delays have not elapsed
		WorldTimestamp now = GetGame().GetWorld().GetTimestamp();
		if (m_Config.m_fNewMissionDelayM > 0 || m_Config.m_fNewMissionNotificationDelayM > 0)
		{
			int delayMs = WR_Utils.MinutesToMilliseconds(m_Config.m_fNewMissionDelayM + m_Config.m_fNewMissionNotificationDelayM);			
			if (m_LastNotificationSendTime && now.DiffMilliseconds(m_LastNotificationSendTime) < delayMs)
				return;
		}
		
		// Do not create new mission if last displayed notification has not disappeared
		if (m_LastNotificationSendTime && now.DiffMilliseconds(m_LastNotificationSendTime) < m_iNotificationDelayMs)
			return;
		
		// Choose a random definition for the mission
		WR_MissionDefinition definition = GetRandomUniqueMissionDefinition();
		if (!definition)
		{
			logger.LogError("Unable to get unique mission definition! Cannot start new mission.");
			return;
		}
		
		// Choose a random vacant location for the mission
		WR_MissionLocationEntity location = GetRandomVacantMissionLocation(definition.m_eSize);
		if (!location)
		{
			logger.LogError("No vacant mission locations! Cannot start new mission.");
			return;
		}
		
		// Initialize new mission and increment mission count.
		// If mission fails to spawn, AdvanceMissionStatuses catches it next frame, so we don't need to handle it here.
		WR_Mission mission = new WR_Mission(location, definition);
		
		// Send preliminary notification if a mission start delay exists
		if (m_Config.m_fNewMissionNotificationDelayM > 0)
		{
			EnqueueTimedNotification(mission);
		}

		m_LastMissionDefinition = definition;
		location.SetIsHostingMission();
		m_aActiveMissions.Insert(mission);
		
		logger.LogNormal(string.Format("Mission queued: %1 (ID: %2, Location: %3)", mission.GetDefinition().m_sName, mission.GetMissionId(), mission.GetLocation().GetName()));
	}
	
	private void ChangeMissionLocation(WR_Mission mission) // Called when a mission fails to spawn at the original location
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
	
	private int GetMaxActiveMissionSlots()
	{
		if (m_Config.m_aPlayercountMissionThresholds.Count() == 0)
		{
			logger.LogError("No player count thresholds set in the mission system config! Stopping mission system...");
			Enable(false);
			return 0;
		}
		
		// Get number of players currently in the game
		int playerCount = GetGame().GetPlayerManager().GetPlayerCount();
		
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
		foreach (WR_Mission mission : m_aActiveMissions)
		{
			WR_MissionStatus status = mission.GetStatus();
			if (status == WR_MissionStatus.Pending || status == WR_MissionStatus.InProgress)
				count += 1;
		}
		
		return count;
	}
	
	// Return a mission definition that is different than the last one spawned. If only one definition exists, returns that one.
	private WR_MissionDefinition GetRandomUniqueMissionDefinition()
	{
		if (!m_aDefinitions || m_aDefinitions.Count() == 0)
		{
			logger.LogError("No mission definitions are set in the mission system config! Stopping mission system...");
			Enable(false);
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
			if (definition.m_eDifficulty == WR_MissionDifficulty.FREE && currentPlayers < m_Config.m_iFreeMissionThreshold)
				continue;
			if (definition.m_eDifficulty == WR_MissionDifficulty.HARD && currentPlayers < m_Config.m_iHardMissionThreshold)
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
		if (m_Config.m_iFreeMissionThreshold == 0 && m_Config.m_iHardMissionThreshold == 0)
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
	
	private void SendNotification(WR_Mission mission)
	{
		m_NotifComponent.SendNotification(mission);
		m_LastNotificationSendTime = GetGame().GetWorld().GetTimestamp();
	}
	
	private void EnqueueTimedNotification(WR_Mission mission)
	{
		m_aPendingTimedNotifications.Insert(mission);
	}
	
	private void HandleTimedNotificationQueue()
	{		
		if (m_aPendingTimedNotifications.Count() == 0)
			return;
		
		WorldTimestamp now = GetGame().GetWorld().GetTimestamp();
		
		int delayMs = WR_Constants.s_iMissionHintDisplayDurationMs + WR_Constants.s_iMissionHintPaddingDurationMs;
		if (!m_LastNotificationSendTime || now.DiffMilliseconds(m_LastNotificationSendTime) > delayMs)
		{
			WR_Mission mission = m_aPendingTimedNotifications[0];
			SendNotification(mission);
			m_aPendingTimedNotifications.Remove(0);
		}
	}
}