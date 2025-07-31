class WR_AIPatrol
{
	ref WR_Logger<WR_AIPatrol> logger = new WR_Logger<WR_AIPatrol>(this);
	SCR_AIGroup m_patrolGroup;
	
	const string GETINVEHICLE_WAYPOINT = "{B049D4C74FBC0C4D}Prefabs/AI/Waypoints/AIWaypoint_GetInNearest.et";
	const string MOVE_WAYPOINT = "{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et";
	const string CYCLE_WAYPOINT = "{35BD6541CBB8AC08}Prefabs/AI/Waypoints/AIWaypoint_Cycle.et";
	
	array<WR_AIPatrolWaypoint> m_aWaypointArray;
	int m_iCurrentWaypointIndex;
	bool recentlyCheckedForVehicle = false;
	
	void WR_AIPatrol(WR_AIPatrolConfig patrolConfig, array<WR_AIPatrolWaypoint> waypointArr, WR_AIPatrolWaypoint startingWaypoint)
	{
		logger.LogNormal("AI Patrol constructor.");
		
		m_aWaypointArray = waypointArr;
		m_iCurrentWaypointIndex = m_aWaypointArray.Find(startingWaypoint);
		
		ResourceName groupResource = patrolConfig.m_aAIGroupChoices.GetRandomElement();
		ResourceName groupVehicleResource = patrolConfig.m_aPatrolVehicleChoices.GetRandomElement();
		
		if (!SpawnGroup(groupResource, groupVehicleResource))
		{
			logger.LogError("Failed to spawn group. Deleting this patrol");
			delete this;
		}
		else 
		{
			logger.LogNormal("Spawned new patrol at waypoint: " + m_iCurrentWaypointIndex);
		}
	}
	
	bool SpawnGroup(ResourceName groupResource, ResourceName groupVehicleResource)
	{
		// Find a safe spot for the NPCs to spawn
		vector spawnPos;
		bool safePosFound = WR_Utils.TryGetRandomSafePosWithinRadius(spawnPos, m_aWaypointArray[m_iCurrentWaypointIndex].GetOrigin(), 1.0, 10.0, 1.0, 1.0);
		if (!safePosFound)
		{
			logger.LogError("Unable to find safe location to spawn AI Patrol");
			return false;
		}
	
		// Spawn the vehicle prefab
		IEntity groupVehicle = WR_Utils.SpawnPrefabInWorld(groupVehicleResource, spawnPos);
		if (!groupVehicle)
		{
			logger.LogError("Vehicle did not spawn!");
			return false;
		}
		
		// Spawn the group prefab
		IEntity aiGroupEntity = WR_Utils.SpawnPrefabInWorld(groupResource, spawnPos);
		m_patrolGroup = SCR_AIGroup.Cast(aiGroupEntity);
		if (!m_patrolGroup)
		{
			logger.LogError("AI Patrol bots did not spawn!");
			return false;
		}	
		
		// Configure group
		m_patrolGroup.SetDeleteWhenEmpty(true);
		m_patrolGroup.SetFaction(GetGame().GetFactionManager().GetFactionByKey("CIV"));
	
		// Add the first waypoint at the AI starting position to make them get in their vehicle
		IEntity waypointEntity = WR_Utils.SpawnPrefabInWorld(GETINVEHICLE_WAYPOINT, m_aWaypointArray[m_iCurrentWaypointIndex].GetOrigin());
		SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(waypointEntity);
		m_patrolGroup.AddWaypoint(waypoint);
		
		
		//Handle Subsequent waypoint creation
		m_patrolGroup.GetOnWaypointCompleted().Insert(UpdateGroupWaypoint);
		
		m_patrolGroup.ActivateAllMembers();

		return true;

	}
	
	void UpdateGroupWaypoint()
	{
		// Check if bots have lost their vehicle
		ChimeraCharacter leader = ChimeraCharacter.Cast(m_patrolGroup.GetLeaderAgent().GetControlledEntity());
		if (!leader.IsInVehicle() && !recentlyCheckedForVehicle)
		{
			logger.LogNormal("Group leader not in vehicle at waypoint: " + m_iCurrentWaypointIndex);
			
			IEntity waypointEntity = WR_Utils.SpawnPrefabInWorld(GETINVEHICLE_WAYPOINT, m_aWaypointArray[m_iCurrentWaypointIndex].GetOrigin());
			SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(waypointEntity);
			m_patrolGroup.AddWaypoint(waypoint);

			recentlyCheckedForVehicle = true;
			return;
		}
		
		recentlyCheckedForVehicle = false;
		
		IncrementWaypointIndex();
		logger.LogNormal("Group has completed a waypoint. Updating to move to waypoint: " + m_iCurrentWaypointIndex);
		
		// Add new MOVE waypoint for the next waypoint in the array
		IEntity waypointEntity = WR_Utils.SpawnPrefabInWorld(MOVE_WAYPOINT, m_aWaypointArray[m_iCurrentWaypointIndex].GetOrigin());
		SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(waypointEntity);
		m_patrolGroup.AddWaypoint(waypoint);
	}
	
	void IncrementWaypointIndex()
	{
		m_iCurrentWaypointIndex++;
		if (m_iCurrentWaypointIndex >= m_aWaypointArray.Count())
			m_iCurrentWaypointIndex = 0;
	}
	
	bool IsGroupDead()
	{
		if (m_patrolGroup) 
			return false;
		
		logger.LogNormal("AI Patrol detected as dead.");
		
		return true;
	}
	
	void ~WR_AIPatrol()
	{
		m_aWaypointArray = null;
	}
}