class WR_Mission
{
	protected string missionName;
	protected vector missionLocation;
	protected bool isComplete = false;
	protected ref array<IEntity> missionEntityList = {};
	protected SCR_AIGroup group;
	
	protected SCR_MapMarkerManagerComponent markerManager;
	protected ref SCR_MapMarkerBase marker;
	
	string lootBoxType = "None"; //Weapon, HeavyWeapon, Medical, Misc (any other values results in no box being spawned)
	
	//--------------------------------------------------
	protected ref array<ResourceName> wreckList = {
		"{25B265F3E7CBDFC8}Prefabs/Props/Wrecks/BMP1_wreck.et",
		"{75FEF1CA582431F3}Prefabs/Props/Wrecks/BTR70_wreck.et",
		"{7783AE4B44D3CF29}Prefabs/Props/Wrecks/M923A1_wreck.et",
		"{2B24356C351A2538}Prefabs/Props/Wrecks/M113_wreck.et"
	};
	protected ref array<ResourceName> unarmedHeliList = {
		"{70BAEEFC2D3FEE64}Prefabs/Vehicles/Helicopters/UH1H/UH1H.et",
		"{DF5CCB7C0FF049F4}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport.et"
	};
	protected ref array<ResourceName> armedHeliList = {
		"{7BD282AF716ED639}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed.et",
		"{DDDD9B51F1234DF3}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed.et"
	};	
	protected ref array<ResourceName> captureBaseList = {
		"{760BD4105816358C}Prefabs/Compositions/Slotted/SlotFlatMedium/FieldHospital_M_FIA_01.et",
		"{CDD0712FD6215F03}Prefabs/Compositions/Slotted/SlotFlatMedium/FieldHospital_M_US_01.et",
		"{2ACE3A315F531CFD}Prefabs/Compositions/Slotted/SlotFlatMedium/FieldHospital_M_USSR_01.et"
	};
	//For CaptureVehicles, use WR_ResourceNamesWeightedOld.GetCaptureVehicles().GetRandomItem()
	
	//--------------------------------------------------
	
	void WR_Mission(string name, vector pos)
	{
		missionName = name;
		missionLocation = pos;
		
		parseMissionType();
		
		//AudioSystem.PlaySound("{06C02AFB2CA882EB}Sounds/UI/Samples/Menu/UI_Task_Created.wav");	
		SCR_HintManagerComponent.ShowCustomHint(("A new " + missionName + " mission has spawned!"), "New Mission Created", 3);
	}
	
	protected void parseMissionType()
	{
		switch (missionName) {
			case "geocache":
				createMissionMarker("Geocache", 2, 33);
				lootBoxType = "Misc";
				spawnEquipmentCrate();
				createCompletionTrigger();
				break;
			case "abandonedheli":
				createMissionMarker("Abandoned Heli", 2, 33);
				spawnMissionPrefab(unarmedHeliList.GetRandomElement(), false);
				createCompletionTrigger();
				break;
			case "capturebase":
				createMissionMarker("Capture Base", 4, 11);
				group = spawnMissionGroup("{6183140797E9A8EF}Prefabs/Groups/US/WR_AIGroupLarge.et");
				spawnMissionPrefab(captureBaseList.GetRandomElement(), true);
				lootBoxType = "Medical";
				group.GetOnEmpty().Insert(OnEmpty);
				break;
			case "captureheli":
				createMissionMarker("Capture Helicopter", 4, 11);
				group = spawnMissionGroup("{6183140797E9A8EF}Prefabs/Groups/US/WR_AIGroupLarge.et");
				spawnMissionPrefab(armedHeliList.GetRandomElement(), false);
				lootBoxType = "None";
				group.GetOnEmpty().Insert(OnEmpty);
				break;
			case "capturevehicle":
				createMissionMarker("Capture Vehicle", 4, 11);
				group = spawnMissionGroup("{6183140797E9A8EF}Prefabs/Groups/US/WR_AIGroupLarge.et");
				spawnMissionPrefab(WR_ResourceNamesWeightedOld.GetCaptureVehicles().GetRandomItem(), false);
				lootBoxType = "None";
				group.GetOnEmpty().Insert(OnEmpty);
				break;
			case "vehiclewreck":
				createMissionMarker("Vehicle Wreck", 2, 11);
				group = spawnMissionGroup("{D4A1576757665D02}Prefabs/Groups/US/WR_AIGroupSmall.et");
				spawnMissionPrefab(wreckList.GetRandomElement(), true);
				lootBoxType = "Weapon";
				group.GetOnEmpty().Insert(OnEmpty);
				break;
			case "medicalsupplies":
				createMissionMarker("Medical Supplies", 2, 11);
				group = spawnMissionGroup("{D4A1576757665D02}Prefabs/Groups/US/WR_AIGroupSmall.et");
				lootBoxType = "Medical";
				group.GetOnEmpty().Insert(OnEmpty);
				break;
			case "heavyweapons":
				createMissionMarker("Heavy Weapons Shipment", 2, 11);
				group = spawnMissionGroup("{D4A1576757665D02}Prefabs/Groups/US/WR_AIGroupSmall.et");
				lootBoxType = "HeavyWeapons";
				group.GetOnEmpty().Insert(OnEmpty);
				break;
			case "snipersquad":
				createMissionMarker("Sniper Squad", 2, 11);
				group = spawnMissionGroup("{D807C7047E818488}Prefabs/Groups/BLUFOR/WR_AISniperTeam.et");
				lootBoxType = "Misc";
				group.GetOnEmpty().Insert(OnEmpty);
				break;
			case "convoy":
				//Everything for this mission is handled in WR_ConvoyMission
				break;
			default:
				Print("[WASTELAND] WR_Mission: Unrecognized mission name!", LogLevel.ERROR);
		}
	}
	
	protected SCR_AIGroup spawnMissionGroup(ResourceName groupPrefabName)
	{
		Resource groupResource = Resource.Load(groupPrefabName);
		SCR_AIGroup newGroup = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupResource, GetGame().GetWorld(), GenerateSpawnParameters(missionLocation)));	

		Resource waypointResource = Resource.Load("{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Defend.et");
		SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(waypointResource, GetGame().GetWorld(), GenerateSpawnParameters(missionLocation)));

		newGroup.AddWaypoint(waypoint);
		return newGroup;
	}
	
	protected void spawnMissionPrefab(ResourceName prefabName, bool deleteLater)
	{
		vector spawnPos;
		bool foundSafePos = SCR_WorldTools.FindEmptyTerrainPosition(spawnPos, missionLocation, 25, 5, 5);
		if (!foundSafePos) {
			Print("[WASTELAND] WR_Mission: Unable to find safe position to spawn vehicle wreck!", LogLevel.ERROR);
			return;
		}
		
		//Load resource and generate spawn parameters
		Resource resource = Resource.Load(prefabName);
		EntitySpawnParams params = GenerateSpawnParameters(missionLocation);
		
		//Spawn wreck and give it a random rotation
		IEntity prefabToSpawn = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		prefabToSpawn.SetYawPitchRoll(WR_Utils.GetRandomHorizontalDirectionAngles());
		
		if (deleteLater)
			missionEntityList.Insert(prefabToSpawn);
	}

	protected void createMissionMarker(string markerText, int colorEntry, int iconEntry)
	{
		markerManager =  SCR_MapMarkerManagerComponent.GetInstance();
		marker = new SCR_MapMarkerBase();
		marker.SetType(SCR_EMapMarkerType.PLACED_CUSTOM);
		marker.SetWorldPos(missionLocation[0], missionLocation[2]);
		marker.SetColorEntry(colorEntry);
		marker.SetIconEntry(iconEntry);
		marker.SetCustomText(markerText);
		
		markerManager.InsertStaticMarker(marker, false, true);
	}
	
	protected EntitySpawnParams GenerateSpawnParameters(vector location)
	{
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = location;
		return params;
	}
	
	void spawnEquipmentCrate()
	{		
		int minItems = 5; int maxItems = 8;
		
		//Setup loot contexts depending on which box needs to be spawned (lootBoxType is set by parseMissionType())
		WR_LootSpawnContext lootContext;
		switch (lootBoxType) {
			case "Weapon":
				lootContext = WR_LootSpawnContextPresets.GetWeaponBoxContext();
				break;
			case "HeavyWeapon":
				lootContext = WR_LootSpawnContextPresets.GetHeavyWeaponBoxContext();
				break;
			case "Medical":
				lootContext = WR_LootSpawnContextPresets.GetMedicalBoxContext();
				minItems = 8; maxItems = 12;
				break;
			case "Misc":
				lootContext = WR_LootSpawnContextPresets.GetLootBoxContext();
				break;
			default:
				return;
		}
	
		//Find a safe place to spawn box
		vector spawnPos;
		bool foundSafePos = WR_Utils.TryGetSafePos(spawnPos, missionLocation, 5, 5);
		if (!foundSafePos) {
			Print("[WASTELAND] WR_Mission: Unable to find safe position to spawn mission reward!", LogLevel.ERROR);
			return;
		}
			
		Print("[WASTELAND] WR_Mission: Spawning box of type " + lootBoxType + " at: " + spawnPos);
			
		//Load resource and generate spawn parameters
		Resource resource = Resource.Load("{2E6EB383EEFDDC4F}Prefabs/Props/Military/Arsenal/ArsenalBoxes/US/WR_BigLootBox_US.et");
		EntitySpawnParams params = GenerateSpawnParameters(spawnPos);
			
		//Create box
		IEntity box = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		if (!box) {
			Print("[WASTELAND] WR_Mission: Mission reward was not spawned!", LogLevel.ERROR);
		}
			
		//Setup inventory and fill it with random items taken from the loot context we selected earlier
		auto inventoryStorage = SCR_UniversalInventoryStorageComponent.Cast(box.FindComponent(SCR_UniversalInventoryStorageComponent));
		auto inventoryStorageManager = SCR_InventoryStorageManagerComponent.Cast(box.FindComponent(SCR_InventoryStorageManagerComponent));
		array<ResourceName> items = lootContext.GetRandomItems(Math.RandomIntInclusive(minItems, maxItems));
		foreach (ResourceName item : items) {
			inventoryStorageManager.TrySpawnPrefabToStorage(item, inventoryStorage);
		}
			
		missionEntityList.Insert(box);
	}
	
	void OnEmpty(SCR_AIGroup grp)
	{
		marker.SetColorEntry(5);
		spawnEquipmentCrate();
		createCompletionTrigger();
	}
	
	protected void createCompletionTrigger()
	{
		Resource resource = Resource.Load("{DA58C3DF4455555E}Prefabs/ScenarioFramework/Triggers/ScenarioFrameworkTrigger.et");
		EntitySpawnParams params = GenerateSpawnParameters(missionLocation);
		SCR_ScenarioFrameworkTriggerEntity triggerEntity = SCR_ScenarioFrameworkTriggerEntity.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params));
		triggerEntity.SetActivationPresence(SCR_EScenarioFrameworkTriggerActivation.PLAYER);
		triggerEntity.SetSphereRadius(10);
		triggerEntity.GetOnActivate().Insert(setMissionComplete);
		missionEntityList.Insert(triggerEntity);
	}
	
	void setMissionComplete()
	{
		AudioSystem.PlaySound("{06C02AFB2CA882EB}Sounds/UI/Samples/Menu/UI_Task_Created.wav");
		SCR_HintManagerComponent.ShowCustomHint(("A " + missionName + " mission has been completed!"), "Mission Completed", 5);
		
		markerManager.RemoveStaticMarker(marker);
		
		int deleteTimer = 1000*60*3;	//3 Minutes
		GetGame().GetCallqueue().CallLater(clearMissionEntities, deleteTimer, false);
	}
	
	void clearMissionEntities()
	{
		Print("[WASTELAND] WR_Mission: Clearing mission entities...");
		foreach (IEntity itemToClear : missionEntityList) {
			SCR_EntityHelper.DeleteEntityAndChildren(itemToClear);
		}
		
		isComplete = true;
	}
	
	vector getMissionLocation()
	{
		return missionLocation;
	}
	
	bool isMissionComplete()
	{
		return isComplete;
	}
	
	void ~WR_Mission()
	{
		delete group;
	}
} 

