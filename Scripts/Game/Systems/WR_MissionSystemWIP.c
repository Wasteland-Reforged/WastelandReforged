class WR_MissionSystemWIP : GameSystem
{
	ref WR_Logger<WR_MissionSystem> logger = new WR_Logger<WR_MissionSystem>(this);
	
	[Attribute("", UIWidgets.Object, "Mission system config")]
	protected ref WR_MissionSystemConfig m_Config;
	
	float m_fTimeElaspedS = 0;
	float m_fMissionCreationTickrateS = 30;

	ref array<WR_Mission> m_aMissions = {};
	
	private override void OnStarted()
	{
		logger.LogNormal("Mission system started.");
	}
	
	private override void OnUpdate(ESystemPoint point)
	{
		// Advance time elapsed
		float timeSlice = GetWorld().GetTimeSlice();
		m_fTimeElaspedS += timeSlice;
		
		// Check if mission needs creation
		if (m_fTimeElaspedS >= m_fMissionCreationTickrateS)
		{
			if (m_aMissions.Count() < GetMaxActiveMissionSlots())
				TryCreateNewMission();
			
			m_fTimeElaspedS = 0;
		}
	}
	
	private override void OnCleanup()
	{
		logger.LogNormal("Mission system cleaned up.");
	}
	
	private void TryCreateNewMission()
	{
		// Choose a mission definition
		WR_MissionDefinition definition = GetRandomMissionDefinition();
		
		// Choose a random vacant location for the mission
		WR_MissionLocationEntity location = GetRandomVacantMissionLocation(definition.m_eSize);
		if (!location) {
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
}