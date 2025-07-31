class WR_AIPatrolSystem : GameSystem
{
	ref WR_Logger<WR_AIPatrolSystem> logger = new WR_Logger<WR_AIPatrolSystem>(this);
	
	[Attribute("", UIWidgets.Object, "AI Patrol system configuration.")];
	ref WR_AIPatrolConfig patrolConfig;
	
	float m_fTimeElaspedS = 0;
	float m_fTotalTimeS = 0;
	float m_tickRateS = 10;
	bool m_enoughWaypoints = true;
	
	static ref array<WR_AIPatrolWaypoint> m_aPatrolWaypoints = {};
	ref array<ref WR_AIPatrol> m_aPatrolArray = {};

	protected override void OnStarted()
	{	
		// Patrol config must be defined and configured properly
		if (!patrolConfig 
		|| patrolConfig.m_iMaxPatrols < 1 
		|| patrolConfig.m_aAIGroupChoices.Count() == 0 
		|| patrolConfig.m_aPatrolVehicleChoices.Count() == 0
		|| !isEnoughWaypoints())
		{
			logger.LogError("No AI Patrol config found OR config not configured properly. Disabling AI Patrol system..");
			this.Enable(false);
		}
		
		SortWaypointArray();
		logger.LogNormal("AI Patrol system started.");

	}
	
	protected override void OnUpdate(ESystemPoint point)
	{
		float timeSlice = GetWorld().GetTimeSlice();
		m_fTimeElaspedS += timeSlice;
		m_fTotalTimeS += timeSlice;
		
		if (m_fTimeElaspedS >= m_tickRateS)
		{
			BeginUpdate();
			
			CheckPatrols();
			
			Update();
			EndUpdate();
			
			m_fTimeElaspedS = 0.0;
		}
	}
	
	protected override void OnCleanup()
	{
		m_aPatrolWaypoints = null;
		m_aPatrolArray = null;
		logger.LogNormal("AI Patrol system stopped.");
	}
	
	protected bool isEnoughWaypoints()
	{
		// Must be at least 2 waypoints in map
		if (!m_aPatrolWaypoints || m_aPatrolWaypoints.Count() < 2)
		{
			logger.LogWarning("Not enough WR_AIPatrolWaypoints found in world for AI Patrol System");
			return false;
		}
		return true;
	}
	
	protected void CheckPatrols()
	{
		logger.LogSpam("Checking Patrols.");
		
		// Remove all dead patrols from array
		for (int i = 0; i < m_aPatrolArray.Count(); i++)
		{
			WR_AIPatrol patrol = m_aPatrolArray.Get(i);
			if (!patrol || patrol.IsGroupDead())
			{
				m_aPatrolArray.RemoveItemOrdered(patrol);
				i--;
			}
		}
		
		// Spawn new patrols until we are at max; Each patrol gets a unique starting waypoint
		array<WR_AIPatrolWaypoint> usedWaypoints = {};

		while (m_aPatrolArray.Count() < patrolConfig.m_iMaxPatrols)
		{
			WR_AIPatrolWaypoint newWaypoint = m_aPatrolWaypoints.GetRandomElement();
			
			// If newly generated waypoint has already been used this pass of CheckPatrols, try again
			if (usedWaypoints.Contains(newWaypoint)) continue;
			
			// Spawn the new patrol. If succeeded in spawning, add it to the patrol array and consider this waypoint "used" for now
			WR_AIPatrol newPatrol = new WR_AIPatrol(patrolConfig, m_aPatrolWaypoints, newWaypoint);
			
			if (newPatrol) 
			{
				m_aPatrolArray.Insert(newPatrol);
				usedWaypoints.Insert(newWaypoint);
			}
		}
	}
	
	// Make sure waypoint array is sorted in the order of their index attributes (which is not necessarily the same order as they have added themselves via InsertNewWaypoint)
	protected void SortWaypointArray()
	{
		array<WR_AIPatrolWaypoint> sortedArray = {};
		int MAX_ATTEMPTS = 100;
		
		for (int i = 0; sortedArray.Count() < m_aPatrolWaypoints.Count() && i < MAX_ATTEMPTS; i++)
		{
			foreach (WR_AIPatrolWaypoint waypoint : m_aPatrolWaypoints)
			{
				if (!waypoint) continue;
				
				if (waypoint.GetWaypointOrderIndex() == i)
				{
					sortedArray.Insert(waypoint);
					logger.LogWarning("Ordering waypoint: " + i);
				}
			}
		}
		
		if (sortedArray.Count() < m_aPatrolWaypoints.Count())
		{
			logger.LogWarning("Incorrect Waypoint Array length after sorting; Some waypoints may have negative indexes or indexes above " + MAX_ATTEMPTS);
		}
		
		m_aPatrolWaypoints = sortedArray;
	}
	
	// Called by WR_AIPatrolWaypoint entities that are placed within the world upon initializing
	static void InsertNewWaypoint(WR_AIPatrolWaypoint waypoint)
	{
		if (!m_aPatrolWaypoints)
		{
			m_aPatrolWaypoints = {};
		}
		
		if (!waypoint) return;
		
		m_aPatrolWaypoints.Insert(waypoint);
		Print("[Wasteland]: Inserted waypoint: " + waypoint.GetWaypointOrderIndex())
	}
	
	static array<WR_AIPatrolWaypoint> GetWaypoints()
	{
		return m_aPatrolWaypoints;
	}

}