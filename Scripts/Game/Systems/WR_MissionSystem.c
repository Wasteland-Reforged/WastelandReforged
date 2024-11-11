class WR_MissionSystem : GameSystem
{
	ref WR_Logger<WR_MissionSystem> logger = new WR_Logger<WR_MissionSystem>(this);
	
	[Attribute("", UIWidgets.Object, "Mission system config")]
	protected ref WR_MissionSystemConfig m_Config;
	
	float m_fMissionCreationTickrateTimeElaspedS = 0;
	float m_fMissionCreationTickrateS = 30;
	//const float m_fTimeBetweenConsecutiveMissionCreationMs = 15000; // Time to wait to start missions in quick succession to prevent overlap of notification hints
	
	WorldTimestamp m_LatestMissionCreatedAtTime;
	//ref array<WR_MissionSlot> m_aMissionSlots = {};
	ref array<WR_Mission> m_aMissions = {};
	
	private override void OnStarted()
	{
		// Initialize m_LatestMissionCreatedAtTime to the delay between consecutive missions so we don't have a delay on gamemode start.
		// TODO: maybe there is a smarter way to handle all this and I am just dumb and tired.
		//m_LatestMissionCreatedAtTime = m_LatestMissionCreatedAtTime.PlusMilliseconds(-m_fTimeBetweenConsecutiveMissionCreationMs);
		
		logger.LogNormal("Mission system started.");
	}
	
	private override void OnCleanup()
	{
		// TODO: should we force end all missions?
		// Not sure if this is needed since this system should only be cleaned up on server close
		
		logger.LogNormal("Mission system cleaned up.");
	}
	
//	private override void OnUpdate(ESystemPoint point)
//	{
//		// 1) Try to create a new mission
//		if (m_fMissionCreationTickrateTimeElaspedS >= m_fMissionCreationTickrateS)
//		{
//			if (GetNumberActiveMissionSlots() < GetMaxActiveMissionSlots())
//				TryStartNewMission();
//			
//			m_fMissionCreationTickrateTimeElaspedS = 0;
//		}
//		
//		// 2) Update existing missions
//		//    a) Start 'pending' missions
//		//    b) Update markers of 'completed' missions
//		//    c) Clean up 'completed' missions if they were completed long enough ago
//		
//		foreach (WR_MissionSlot slot : m_aMissionSlots)
//		{
//			WorldTimestamp now = GetGame().GetWorld().GetTimestamp();
//			WR_MissionStatus lifecycleStatus = slot.GetMission().GetStatus();
//			
//			switch (lifecycleStatus)
//			{
//				case lifecycleStatus == WR_MissionStatus.Pending:
//				{
//					float timeDiff = now.DiffMilliseconds(slot.GetCreatedAtTime());
//					
//					if (timeDiff >= m_Config.m_fNewMissionNotificationDelay)
//					{
//						slot.GetMission();//.StartMission();
//						// Send hint
//					}
//					break;
//				}
//				case lifecycleStatus == WR_MissionStatus.Complete:
//				{
//					// Grey out markers of missions that were just completed
//				
//					// Mark 
//					slot.GetMission().SetStatus(WR_MissionStatus.AwaitingCleanup);
//					
//					break;
//				}
//				case lifecycleStatus == WR_MissionStatus.AwaitingCleanup:
//				{
//					float timeDiff = now.DiffMilliseconds(slot.GetCompletedAtTime());
//					
//					if (slot.GetMission().GetMarker())
//					{
//						// Delete markers
//						if (timeDiff >= m_Config.m_fMissionMapMarkerCleanupDelay)
//						{
//							slot.GetMission();//.DeleteMarker();
//						}
//					}
//					
//					if (timeDiff >= m_Config.m_fMissionCleanupDelay)
//					{
//						slot.GetMission();//.Cleanup();
//					}
//					
//					break;
//				}
//			}
//		}
//
//		// Update timers
//		float timeSlice = GetWorld().GetTimeSlice();
//		m_fMissionCreationTickrateTimeElaspedS += timeSlice;
//	}
	
	private float GetDelayByStatus(WR_MissionStatus status)
	{
		switch (status)
		{
			case WR_MissionStatus.Pending: return m_Config.m_fInitialMissionDelay;
			case WR_MissionStatus.InProgress: return m_Config.m_fMissionTimeLimit;
			case WR_MissionStatus.Complete: return m_Config.m_fMissionMapMarkerCleanupDelay;
			case WR_MissionStatus.AwaitingCleanup: return m_Config.m_fMissionCleanupDelay;
			default: 
		}
		return 0;
	}
	
	private override void OnUpdate(ESystemPoint point)
	{
		// 1) Try creating a new mission
		if (m_fMissionCreationTickrateTimeElaspedS >= m_fMissionCreationTickrateS)
		{
			if (GetNumberActiveMissionSlots() < GetMaxActiveMissionSlots())
				TryCreateNewMission();
			
			m_fMissionCreationTickrateTimeElaspedS = 0;
		}
		
		// 2) Update existing missions 
		WorldTimestamp now = GetGame().GetWorld().GetTimestamp();
		foreach (WR_Mission mission : m_aMissions)
		{
			float lifecycleStatus = mission.GetStatus();
			float delay = GetDelayByStatus(lifecycleStatus);
			float timeDiff = now.DiffMilliseconds(mission.GetLastStepCompletedTime());
			
			// Update occurs if a mission has been waiting in its current status for long enough
			if (timeDiff >= delay)
			{
				switch (lifecycleStatus)
				{
					case lifecycleStatus == WR_MissionStatus.Pending:
					{
						//mission.StartMission();
						// Send hint
						break;
					}
					case lifecycleStatus == WR_MissionStatus.Complete:
					{
						// Grey out markers of missions that were just completed
					
						// Mark 
						mission.SetStatus(WR_MissionStatus.AwaitingCleanup);
						
						break;
					}
					case lifecycleStatus == WR_MissionStatus.AwaitingCleanup:
					{
						float timeDiff = now.DiffMilliseconds(slot.GetCompletedAtTime());
						
						if (slot.GetMission().GetMarker())
						{
							// Delete markers
							if (timeDiff >= m_Config.m_fMissionMapMarkerCleanupDelay)
							{
								slot.GetMission();//.DeleteMarker();
							}
						}
						
						if (timeDiff >= m_Config.m_fMissionCleanupDelay)
						{
							slot.GetMission();//.Cleanup();
						}
						
						break;
					}
				}
			}
		}
	}
	
	private void TryCreateNewMission()
	{
		// Choose a mission definition
		WR_MissionDefinition definition = GetRandomMissionDefinition();
		
		// Choose a location for the mission
		// TODO: change this to use a getter for the size on the mission
		WR_MissionLocationEntity location = GetRandomVacantMissionLocation(definition.m_eSize);
		if (!location)
		{
			logger.LogError("No vacant mission locations! Cannot start new mission.");
			return;
		}
		
		WR_Mission mission = new WR_Mission(); //new WR_Mission(definition, location);
		mission.SetDefinition(definition);
		// TODO: make sure missions are created with the PENDING status, or with the INPROGRESS status if no pre-notif is enabled
		RegisterMissionInSlot(mission);
		
		logger.LogNormal("Mission started successfully: " + mission.GetName() + " (ID: " + mission.GetMissionId() + ")");
	}
	
	private int GetMaxActiveMissionSlots()
	{
		// Get number of players currently in the game
		int playerCount = GetGame().GetPlayerManager().GetAllPlayerCount();
		
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
	
	private int GetNumberActiveMissionSlots()
	{
		return m_aMissionSlots.Count();
	}
	
	private WR_MissionDefinition GetRandomMissionDefinition()
	{
		// TODO: let's make this use a weighted array
		return m_Config.m_aMissionDefinitions.GetRandomElement();
	}
	
	private WR_MissionLocationEntity GetRandomVacantMissionLocation(WR_MissionLocationSize requiredSize)
	{			
		array<WR_MissionLocationEntity> vacantLocations = WR_MissionLocationEntity.GetAllVacantLocations();
		if (vacantLocations.Count() == 0)
		{
			return null;
		}
		
		WR_MissionLocationEntity randomLocation;
		
		// TODO: this is a lame, unoptimized way of doing it, but it works for now.
		int attempts = 1;
		while (!randomLocation || requiredSize != randomLocation.GetSize())
		{
			if (attempts == 10)
				return null;
			
			randomLocation = vacantLocations.GetRandomElement();
			attempts++;
		}
		
		// TODO: check if there are any entities inside/nearby this mission location's trigger zone. We don't want to spawn missions on people's heads

		return randomLocation;
	}
	
	private void RegisterMissionInSlot(WR_Mission mission)
	{
		WorldTimestamp createdAtTime = GetGame().GetWorld().GetTimestamp();
		
		WR_MissionSlot slot = new WR_MissionSlot(mission);
		slot.SetCreatedAtTime(createdAtTime);
		
		m_aMissionSlots.Insert(slot);
		
		WR_MissionUiElementHelper.ShowHintWithSound("New mission", mission.GetName());
		m_LatestMissionCreatedAtTime = createdAtTime;
	}
}