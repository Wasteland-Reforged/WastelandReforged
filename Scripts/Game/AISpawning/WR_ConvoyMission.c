class WR_ConvoyMission : WR_Mission
{
	
	protected SCR_AIGroup group;
	protected int startingWaypointIndex, totalWaypoints;
	protected ref array<vector> waypointLocations;
	
	void WR_ConvoyMission(string name, vector pos)
	{
		waypointLocations = {};
		totalWaypoints = createWaypointArray();
		getStartingWaypoint();
		createMissionMarker(4, 15);
		
		spawnAIGroup();
		group.GetOnEmpty().Insert(OnEmpty);
		
		addWaypointsToGroup();
		
		GetGame().GetCallqueue().CallLater(updateMissionLocation, 3000, false);
	}
	
	protected int createWaypointArray()		//Returns total number of waypoints
	{
		int index = 1;
		string waypointName;
		while (true) {
			waypointName = ("AIWaypointConvoy" + index);
			IEntity newWaypoint = GetGame().GetWorld().FindEntityByName(waypointName);
			if (!newWaypoint) break;
			waypointLocations.Insert(newWaypoint.GetOrigin());
			index++;
		}
		return index-1;
	}
	
	protected void getStartingWaypoint()
	{
		startingWaypointIndex = Math.RandomIntInclusive(1, totalWaypoints);		//waypoint names start indexing at 1
		string waypointName = ("AIWaypointConvoy" + startingWaypointIndex);
		IEntity startingWaypoint = GetGame().GetWorld().FindEntityByName(waypointName);
		
		missionLocation = startingWaypoint.GetOrigin();
	}
	
	protected void addWaypointsToGroup()
	{
		int newIndex;
		vector newLocation;
		
		for (int i = 0; i < totalWaypoints; i++) {
			
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
			Print("[WR_ConvoyMission]: Ceasing updateMissionLocation()..  1");
			return;
		}
		
		if (!group.GetLeaderEntity()) {
			Print("[WR_ConvoyMission]: Ceasing updateMissionLocation()..  2");
			return;
		}
		
		missionLocation = group.GetLeaderEntity().GetOrigin();
		marker.SetWorldPos(missionLocation[0], missionLocation[2]);
		GetGame().GetCallqueue().CallLater(updateMissionLocation, 1000, false);

	}
	
	protected void spawnAIGroup()
	{
		//Generate Resource for spawn group
		Resource groupResource = Resource.Load("{295E3CC0BB41A71F}Prefabs/Groups/US/WR_AIConvoy.et");
		if (!groupResource || !groupResource.IsValid()) {
			Print("Unable to load resource for spawn group", LogLevel.ERROR);
			return;
		}
		
		//Generate the spawn parameters and spawn the spawnGroup
		group = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupResource, GetGame().GetWorld(), GenerateSpawnParameters(missionLocation)));	
		if (!group) {
			Print("Unable to spawn group!", LogLevel.ERROR);
			return;
		}
		
		//Create waypoint to make AI get in nearest vehicle
		Resource waypointResource = Resource.Load("{B049D4C74FBC0C4D}Prefabs/AI/Waypoints/AIWaypoint_GetInNearest.et");
		if (!waypointResource || !waypointResource.IsValid()) {
			Print("[WR_ConvoyMission] Unable to load resource for waypoint", LogLevel.ERROR);
			return;
		}	

		SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(waypointResource, GetGame().GetWorld(), GenerateSpawnParameters(missionLocation)));
		if (!waypoint) {
			Print("[WR_ConvoyMission] Unable to create waypoint!", LogLevel.ERROR);
			return;
		}

		group.AddWaypoint(waypoint);

	}
	
	void OnEmpty(SCR_AIGroup grp)
	{
		spawnEquipmentCrates("Weapon", 1, 6, 8);
		marker.SetColorEntry(5);
		createCompletionTrigger();
	}
	
	void ~WR_ConvoyMission()
	{
		delete waypointLocations;
		delete group;
	}
}