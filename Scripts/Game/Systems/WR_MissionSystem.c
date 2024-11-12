class WR_MissionSystem : GameSystem
{
	ref WR_Logger<WR_MissionSystem> logger = new WR_Logger<WR_MissionSystem>(this);
	
	[Attribute("", UIWidgets.Object, "Mission system config")]
	protected ref WR_MissionSystemConfig m_Config;
	
	float m_fMissionCreationTimeElaspedS = 0;
	float m_fMissionCreationTickrateS = 30;

	int currentActiveMissions = 0;
	ref array<WR_Mission> m_aMissions = {};
	bool doPreNotif = false;
	
	private override void OnStarted()
	{
		logger.LogNormal("Mission system started.");
	}
	
	private override void OnUpdate(ESystemPoint point)
	{
		// Advance time elapsed
		float timeSlice = GetWorld().GetTimeSlice();
		m_fMissionCreationTimeElaspedS += timeSlice;
		
		// Advance mission statuses
		foreach (WR_Mission mission : m_aMissions)
		{
			CheckForMissionUpdate(mission);
		}
		
		// Check if mission needs creation
		if (m_fMissionCreationTimeElaspedS >= m_fMissionCreationTickrateS)
		{
			if (currentActiveMissions < GetMaxActiveMissionSlots())
				TryCreateNewMission();
			
			m_fMissionCreationTimeElaspedS = 0;
		}
	}
	
	private override void OnCleanup()
	{
		logger.LogNormal("Mission system cleaned up.");
	}
	
	private float CheckForMissionUpdate(WR_Mission mission)
	{
		float delay;
		WorldTimestamp ts = GetGame().GetWorld().GetTimestamp();
		
		switch (mission.getStatus())
		{
			case WR_MissionStatus.FailedToSpawn:
				ConcludeMission(mission, true);
				break;
			
			case WR_MissionStatus.Pending: 
				delay = m_Config.m_fInitialMissionDelay;
				if (ts.DiffMilliseconds(mission.getLastTimestamp()) > delay) {
					mission.StartMission();
					// Send Mission Start Notification
					// Create Marker
				}
				break;
			
			case WR_MissionStatus.InProgress: 
				delay = m_Config.m_fMissionTimeLimit;
				if (ts.DiffMilliseconds(mission.getLastTimestamp()) > delay) {
					// Send Timeout Notification
					// Delete Marker
					ConcludeMission(mission, true);
				}
				break;
			
			case WR_MissionStatus.Complete: 
				// Grey Out Marker
				// Send Completion Notif
				mission.ChangeMissionStatus(WR_MissionStatus.AwaitingMarkerDeletion);
				break;
			
			case WR_MissionStatus.RewardDestroyed:
				// Grey Out Marker
				// Send Reward Destroyed Notif
				mission.ChangeMissionStatus(WR_MissionStatus.AwaitingMarkerDeletion);
				break;
			
			case WR_MissionStatus.AwaitingMarkerDeletion:
				delay = m_Config.m_fMissionMapMarkerCleanupDelay;
				if (ts.DiffMilliseconds(mission.getLastTimestamp()) > delay) {
					// Delete Marker
					currentActiveMissions -= 1;
					mission.ChangeMissionStatus(WR_MissionStatus.AwaitingCleanup);
				}
				break;
			
			case WR_MissionStatus.AwaitingCleanup: 
				delay = m_Config.m_fMissionCleanupDelay;
				if (ts.DiffMilliseconds(mission.getLastTimestamp()) > delay) {
					ConcludeMission(mission, false);
				}
				break;
			
			default: 
		}
		return 0;
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
		
		WR_Mission mission = new WR_Mission(location, definition);
		m_aMissions.Insert(mission);
		currentActiveMissions += 1;
		
		if (!doPreNotif) mission.StartMission();
		
		logger.LogNormal("Mission started successfully: " + mission.m_Definition.m_sName);
	}
	
	private void ConcludeMission(WR_Mission mission, bool deleteRewards)
	{
		mission.DeleteMissionEntities(true);
		m_aMissions.RemoveItem(mission);
		delete mission;
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
	
	private WR_MissionDefinition GetRandomMissionDefinition()
	{
		// TODO: let's make this use a weighted array
		return m_Config.m_aMissionDefinitions.GetRandomElement();
	}
	
	private WR_MissionLocationEntity GetRandomVacantMissionLocation(WR_MissionLocationSize requiredSize)
	{			
		array<WR_MissionLocationEntity> vacantLocations = WR_MissionLocationEntity.GetAllVacantLocations(requiredSize);
		if (vacantLocations.Count() == 0) return null;

		return vacantLocations.GetRandomElement();
	}
}