class WR_AIMission : WR_Mission
{
	protected SCR_AIGroup group;
	protected string groupPrefabName;
	
	void WR_AIMission(string name, vector pos)
	{
		parseMissionType();
		group = spawnAIGroup();
		group.GetOnEmpty().Insert(OnEmpty);
		
		createMissionMarker(4, 11);
	}
	
	protected void parseMissionType()
	{
		//Determine which AI group to spawn, depending on the mission type (name)
		switch(missionName)
		{
			case "CaptureVehicle":
				groupPrefabName = "{D4A1576757665D02}Prefabs/Groups/US/Group_US_Random.et";
				spawnRandomCaptureVehicle();
				break;
			case "HeavyWeapons":
				groupPrefabName = "{D4A1576757665D02}Prefabs/Groups/US/Group_US_Random.et";
				break;
			case "VehicleWreck":
				groupPrefabName = "{D4A1576757665D02}Prefabs/Groups/US/Group_US_Random.et";
				spawnWreckedVehicle();
				break;
			case "MedicalSupplies":
				groupPrefabName = "{D4A1576757665D02}Prefabs/Groups/US/Group_US_Random.et";
				break;
			case "CaptureBase":
				groupPrefabName = "{D4A1576757665D02}Prefabs/Groups/US/Group_US_Random.et";
				spawnCaptureBase();
				break;
			case "SniperSquad":
				groupPrefabName = "{D807C7047E818488}Prefabs/Groups/BLUFOR/Group_US_SniperTeam.et";
				break;
			default:
				Print("[WR_AIMission] Unexpected mission type!", LogLevel.ERROR);
		}
	}
	
	protected SCR_AIGroup spawnAIGroup()
	{
		//Generate Resource for spawn group
		Resource groupResource = Resource.Load(groupPrefabName);
		if (!groupResource || !groupResource.IsValid()) {
			Print(("[WR_AIMission] Unable to load resource for spawn group" + groupPrefabName), LogLevel.ERROR);
			return null;
		}
		
		//Generate the spawn parameters and spawn the spawnGroup
		SCR_AIGroup newGroup = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupResource, GetGame().GetWorld(), GenerateSpawnParameters(missionLocation)));	
		if (!newGroup) {
			Print("[WR_AIMission] Unable to spawn group!", LogLevel.ERROR);
			return null;
		}

		//Generate Resource for waypoint
		Resource waypointResource = Resource.Load("{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Defend.et");
		if (!waypointResource || !waypointResource.IsValid()) {
			Print("[WR_AIMission] Unable to load resource for waypoint", LogLevel.ERROR);
			return null;
		}
		
		//Generate spawn parameters and create the AIWaypoint
		SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(waypointResource, GetGame().GetWorld(), GenerateSpawnParameters(missionLocation)));
		if (!waypoint) {
			Print("[WR_AIMission] Unable to create waypoint!", LogLevel.ERROR);
			return null;
		}
		
		//Assign the waypoint to the group and return the group
		newGroup.AddWaypoint(waypoint);
		return newGroup;
	}
	
	void spawnRandomCaptureVehicle()
	{
		//Get list of vehicles that are allowed to spawn
		WR_WeightedItemArray<ResourceName> vehicleResourceNames = WR_ResourceNamesWeightedOld.GetCaptureVehicles();
		if (!vehicleResourceNames || vehicleResourceNames.Count() == 0) {
			Print("[WR_AIMission] No capture vehicle resource names were supplied! Please provide at least one resource name.");
			return;
		}
		
		//Find a safe place for the vehicle to spawn
		vector spawnPos;
		bool foundSafePos = SCR_WorldTools.FindEmptyTerrainPosition(spawnPos, missionLocation, 25, 5, 5);
		if (!foundSafePos) {
			Print("[WR_AIMission]: Could not find safe place to spawn capture vehicle");
			return;
		}
		
		//Load resources and generate spawn parameters
		Resource resource = Resource.Load(vehicleResourceNames.GetRandomItem());
		EntitySpawnParams params = GenerateSpawnParameters(missionLocation);
		
		//Spawn the vehicle and give it a random rotation
		IEntity vehicle = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		vehicle.SetYawPitchRoll(WR_Utils.GetRandomHorizontalDirectionAngles());
	}
	
	void spawnWreckedVehicle()
	{
		//Get list of prefabs of the possible vehicle wrecks
		array<ResourceName> wreckList = {
			"{25B265F3E7CBDFC8}Prefabs/Props/Wrecks/BMP1_wreck.et",
			"{75FEF1CA582431F3}Prefabs/Props/Wrecks/BTR70_wreck.et",
			"{7783AE4B44D3CF29}Prefabs/Props/Wrecks/M923A1_wreck.et",
			"{2B24356C351A2538}Prefabs/Props/Wrecks/M113_wreck.et"
		};
		
		//Find sage position to spawn wreck
		vector spawnPos;
		bool foundSafePos = SCR_WorldTools.FindEmptyTerrainPosition(spawnPos, missionLocation, 25, 5, 5);
		if (!foundSafePos) {
			Print("[WR_AIMission]: Could not find safe place to spawn wreck");
			return;
		}
		
		//Load resource and generate spawn parameters
		Resource resource = Resource.Load(wreckList.GetRandomElement());
		EntitySpawnParams params = GenerateSpawnParameters(missionLocation);
		
		//Spawn wreck and give it a random rotation
		IEntity wreck = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		wreck.SetYawPitchRoll(WR_Utils.GetRandomHorizontalDirectionAngles());
		missionEntityList.Insert(wreck);
	}
	
	void spawnCaptureBase()
	{
		array<ResourceName> baseList = {
			"{760BD4105816358C}Prefabs/Compositions/Slotted/SlotFlatMedium/FieldHospital_M_FIA_01.et",
			"{CDD0712FD6215F03}Prefabs/Compositions/Slotted/SlotFlatMedium/FieldHospital_M_US_01.et",
			"{2ACE3A315F531CFD}Prefabs/Compositions/Slotted/SlotFlatMedium/FieldHospital_M_USSR_01.et"
		};
		
		//Find sage position to spawn base
		vector spawnPos;
		bool foundSafePos = SCR_WorldTools.FindEmptyTerrainPosition(spawnPos, missionLocation, 50, 15, 15);
		if (!foundSafePos) {
			Print("[WR_AIMission]: Could not find safe place to spawn base");
			return;
		}
		
		//Load resource and generate spawn parameters
		Resource resource = Resource.Load(baseList.GetRandomElement());
		EntitySpawnParams params = GenerateSpawnParameters(missionLocation);
		
		//Spawn base
		IEntity base = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		
		missionEntityList.Insert(base);
	}
	
	//Triggers as soon as the last AI in a group dies
	void OnEmpty(SCR_AIGroup grp)
	{
		//Some missions spawn rewards after they are completed, depending on mission type
		switch(missionName) {
			case "HeavyWeapons":
				spawnEquipmentCrates("HeavyWeapon", 1, 4, 6);
				break;
			case "VehicleWreck":
				spawnEquipmentCrates("Weapon", 1, 4, 6);
				break;
			case "MedicalSupplies":
				spawnEquipmentCrates("Medical", 2, 10, 15);
			default:
		}
		marker.SetColorEntry(5);
		createCompletionTrigger();
	}
	
	void ~WR_AIMission()
	{
		delete group;
	}
	
}