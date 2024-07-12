class WR_ConvoyMission : WR_Mission
{
	protected int totalWaypoints = 0;
	protected int startingWaypointIndex;
	protected ref array<vector> waypointLocations = {};
	
	void WR_ConvoyMission(string name, vector pos)
	{	
		initializeWaypoints();
		
		group = spawnAIGroup();
		addWaypointsToGroup();
		
		lootBoxType = "Weapon";
		group.GetOnEmpty().Insert(OnEmpty);
		
		createMissionMarker("Convoy", 4, 15);
		
		GetGame().GetCallqueue().CallLater(updateMissionLocation, 3000, false);
	}
	
	protected void initializeWaypoints()
	{
		while (true) {
			string waypointName = ("AIWaypointConvoy" + (totalWaypoints+1));
			IEntity newWaypoint = GetGame().GetWorld().FindEntityByName(waypointName);
			if (!newWaypoint) break;
			waypointLocations.Insert(newWaypoint.GetOrigin());
			totalWaypoints++;
		}

		startingWaypointIndex = Math.RandomIntInclusive(1, totalWaypoints);
		string waypointName = ("AIWaypointConvoy" + startingWaypointIndex);
		IEntity startingWaypoint = GetGame().GetWorld().FindEntityByName(waypointName);
		missionLocation = startingWaypoint.GetOrigin();
	}

	protected SCR_AIGroup spawnAIGroup()
	{
		//Generate Resource for spawn group
		Resource groupResource = Resource.Load("{295E3CC0BB41A71F}Prefabs/Groups/US/WR_AIConvoy.et");
		SCR_AIGroup newGroup = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupResource, GetGame().GetWorld(), GenerateSpawnParameters(missionLocation)));	
		
		//Create waypoint to make AI get in nearest vehicle
		Resource waypointResource = Resource.Load("{B049D4C74FBC0C4D}Prefabs/AI/Waypoints/AIWaypoint_GetInNearest.et");
		SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(waypointResource, GetGame().GetWorld(), GenerateSpawnParameters(missionLocation)));

		newGroup.AddWaypoint(waypoint);
		return newGroup;
	}
	
	protected void addWaypointsToGroup()
	{
		int newIndex;
		int maxWaypoints = 50;
		vector newLocation;
		
		for (int i = 0; i < maxWaypoints; i++) {
			
			newIndex = (startingWaypointIndex + i - 1) % totalWaypoints;
			newLocation = waypointLocations[newIndex];
			
			//Generate Resource for waypoint
			Resource waypointResource = Resource.Load("{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et");
			if (!waypointResource || !waypointResource.IsValid()) {
				Print("[WR_ConvoyMission] Unable to load resource for waypoint", LogLevel.ERROR);
				return;
			}
			
			//Generate spawn parameters and create the AIWaypoint
			SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(waypointResource, GetGame().GetWorld(), GenerateSpawnParameters(newLocation)));
			if (!waypoint) {
				Print("[WR_ConvoyMission] Unable to create waypoint!", LogLevel.ERROR);
				return;
			}
			
			//Assign the waypoint to the group and return the group
			group.AddWaypoint(waypoint);
		}
	}
	
	protected void updateMissionLocation()
	{
		if (isComplete || !group) {
			return;
		}	
		if (!group.GetLeaderEntity()) {
			return;
		}
		
		missionLocation = group.GetLeaderEntity().GetOrigin();
		marker.SetWorldPos(missionLocation[0], missionLocation[2]);
		GetGame().GetCallqueue().CallLater(updateMissionLocation, 1000, false);
	}
	
	void ~WR_ConvoyMission()
	{
		delete waypointLocations;
	}
}