class WR_MissionSystem : GameSystem
{
	ref WR_Logger<WR_MissionSystem> logger = new WR_Logger<WR_MissionSystem>(this);
	
	[Attribute("", UIWidgets.Object, "Mission system config")]
	protected ref WR_MissionSystemConfig m_Config;
	
	float m_fMissionCreationTimeElaspedS = 0;
	float m_fMissionCreationTickrateS = 10;

	int currentActiveMissions = 0;
	ref map<ref WR_Mission, SCR_MapMarkerBase> m_aMissions = new map<ref WR_Mission, SCR_MapMarkerBase>;
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
		
		// Check if mission needs creation
		if (m_fMissionCreationTimeElaspedS >= m_fMissionCreationTickrateS)
		{
			if (currentActiveMissions < GetMaxActiveMissionSlots())
				TryCreateNewMission();
			
			m_fMissionCreationTimeElaspedS = 0;
		}
		
		// Advance mission statuses
		if (!m_aMissions || m_aMissions.Count() == 0) return;
		foreach (WR_Mission mission, SCR_MapMarkerBase marker : m_aMissions)
		{
			if (mission) {
				CheckForMissionUpdate(mission);
				if (!mission)						//If a mission gets deleted (and removed from the array) mid update, this foreach loop goes out of bounds, hence we need a break
					break;
			}
		}
	}
	
	private override void OnCleanup()
	{
		logger.LogNormal("Mission system cleaned up.");
	}
	
	private void CheckForMissionUpdate(WR_Mission mission)
	{
		float delay;
		WorldTimestamp ts = GetGame().GetWorld().GetTimestamp();
		SCR_MapMarkerBase marker = m_aMissions.Get(mission);
		
		switch (mission.GetStatus())
		{
			case WR_MissionStatus.FailedToSpawn:
				WR_MissionUiElementHelper.DeleteMarker(marker);
				ConcludeMission(mission, true);
				break;
			
			case WR_MissionStatus.Pending: 
				delay = WR_Utils.minutesToMS(m_Config.m_fInitialMissionDelay);
				if (ts.DiffMilliseconds(mission.GetLastTimestamp()) > delay) {
					mission.StartMission();
					WR_MissionUiElementHelper.ShowMarker(marker);
					// Send Mission Start Notification
				}
				break;
			
			case WR_MissionStatus.InProgress: 
				delay = WR_Utils.minutesToMS(m_Config.m_fMissionTimeLimit);
				if (ts.DiffMilliseconds(mission.GetLastTimestamp()) > delay) {
					WR_MissionUiElementHelper.DeleteMarker(marker);
					// Send Timeout Notification
					ConcludeMission(mission, true);
					logger.LogNormal("Mission timed out.");
				}
				break;
			
			case WR_MissionStatus.Complete: 		//Needs its own status because we only change the marker and send the notification ONCE
				WR_MissionUiElementHelper.UpdateMarkerColor(marker, 5);
				// Send Completion Notif
				mission.ChangeMissionStatus(WR_MissionStatus.AwaitingMarkerCleanup);
				logger.LogNormal("Mission completed.");
				break;
			
			case WR_MissionStatus.AwaitingMarkerCleanup: 
				delay = WR_Utils.minutesToMS(m_Config.m_fMissionMapMarkerCleanupDelay);
				if (ts.DiffMilliseconds(mission.GetLastTimestamp()) > delay) {
					WR_MissionUiElementHelper.DeleteMarker(marker);
					mission.ChangeMissionStatus(WR_MissionStatus.AwaitingCleanup);
					logger.LogNormal("Cleaned map marker.");
				}
				break;
			
			case WR_MissionStatus.AwaitingCleanup: 
				delay = WR_Utils.minutesToMS(m_Config.m_fMissionCleanupDelay);
				if (ts.DiffMilliseconds(mission.GetLastTimestamp()) > delay) {
					ConcludeMission(mission, false);
					logger.LogNormal("Cleaned mission.");
				}
				break;
			
			default: 
		}
	}
	
	private void TryCreateNewMission()
	{
		// Choose a mission definition
		WR_MissionDefinition definition = GetRandomMissionDefinition();
		if (!definition) {
			logger.LogError("No mission definitions have been defined! Cannot start new mission.");
			return;
		}
		
		// Choose a random vacant location for the mission
		WR_MissionLocationEntity location = GetRandomVacantMissionLocation(definition.m_eSize);
		if (!location) {
			logger.LogError("No vacant mission locations! Cannot start new mission.");
			return;
		}
		
		//Initialize new mission and increment mission count. If mission fails to spawn, CheckForMissionUpdate catches it next frame anyways so we dont handle it here
		WR_Mission mission = new WR_Mission(location, definition);

		currentActiveMissions += 1;
		logger.LogNormal("Incremented active missions to: " + currentActiveMissions);
			
		SCR_MapMarkerBase marker = WR_MissionUiElementHelper.CreateMarker(mission);
		
		// If future mission notifications is disabled, start mission right away and show its marker
		if (!doPreNotif) {
			mission.StartMission();
			WR_MissionUiElementHelper.ShowMarker(marker);
		}
		
		m_aMissions.Insert(mission, marker);
	}
	
	private void ConcludeMission(WR_Mission mission, bool deleteRewards)
	{
		currentActiveMissions -= 1;
		logger.LogNormal("Decremented active missions to: " + currentActiveMissions);
		
		mission.DeleteMissionEntities(deleteRewards);
		m_aMissions.Remove(mission);

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