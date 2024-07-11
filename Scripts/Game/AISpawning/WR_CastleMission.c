class WR_CastleMission : WR_Mission
{
	protected int remainingGroups;

	void WR_CastleMission(string name, vector pos)
	{
		remainingGroups = 3;
		
		missionLocation = GetGame().GetWorld().FindEntityByName("CastleDefenderWaypoint1").GetOrigin();
		spawnCastleAI();
		missionLocation = GetGame().GetWorld().FindEntityByName("CastleDefenderWaypoint2").GetOrigin();
		spawnCastleAI();
		missionLocation = GetGame().GetWorld().FindEntityByName("CastleDefenderWaypoint3").GetOrigin();
		spawnCastleAI();
		
		createMissionMarker(4,11);
	}
	
	protected void spawnCastleAI()
	{
		vector posToSpawn;
		bool foundSafePos;
		string groupPrefabName = "{D4A1576757665D02}Prefabs/Groups/US/WR_AIGroupSmall.et";

		//Generate Resource for spawn group
		Resource groupResource = Resource.Load(groupPrefabName);
		if (!groupResource || !groupResource.IsValid()) {
			Print(("[WR_AIMission] Unable to load resource for spawn group: " + groupPrefabName), LogLevel.ERROR);
			return;
		}
		
		//Generate the spawn parameters and spawn the spawnGroup
		SCR_AIGroup newGroup = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupResource, GetGame().GetWorld(), GenerateSpawnParameters(missionLocation)));	
		if (!newGroup) {
			Print("[WR_AIMission] Unable to spawn group!", LogLevel.ERROR);
			return;
		}

		//Generate Resource for waypoint
		Resource waypointResource = Resource.Load("{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Defend.et");
		if (!waypointResource || !waypointResource.IsValid()) {
			Print("[WR_AIMission] Unable to load resource for waypoint", LogLevel.ERROR);
			return;
		}
		
		//Generate spawn parameters and create the AIWaypoint
		SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(waypointResource, GetGame().GetWorld(), GenerateSpawnParameters(missionLocation)));
		if (!waypoint) {
			Print("[WR_AIMission] Unable to create waypoint!", LogLevel.ERROR);
			return;
		}
		
		//Assign the waypoint to the group and return the group
		newGroup.AddWaypoint(waypoint);
		newGroup.GetOnEmpty().Insert(onGroupKilled);

	}
	
	protected void onGroupKilled(SCR_AIGroup grp)
	{
		remainingGroups--;
		if (remainingGroups <= 0) {
			missionLocation = grp.GetOrigin();
			setMissionComplete();
			spawnEquipmentCrates("Regular", 1, 6, 8);
			spawnEquipmentCrates("HeavyWeapon", 1, 4, 6);
			spawnEquipmentCrates("Medical", 1, 6, 8);
		}
	}

}