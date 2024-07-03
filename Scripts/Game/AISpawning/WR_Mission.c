class WR_Mission
{
	protected string missionName;
	protected vector missionLocation;
	protected bool isComplete;
	protected SCR_MapMarkerManagerComponent markerManager;
	protected ref SCR_MapMarkerBase marker;
	protected ref array<IEntity> missionEntityList;
	
	void WR_Mission(string name, vector pos)
	{
		missionName = name;
		missionLocation = pos;
		isComplete = false;
		missionEntityList = {};
		
		//Send out notification to server
		SCR_NotificationsComponent.SendToEveryone(ENotification.WR_MISSION_CREATED);
		AudioSystem.PlaySound("{06C02AFB2CA882EB}Sounds/UI/Samples/Menu/UI_Task_Created.wav");
	}
	
	protected void createMissionMarker(int colorEntry, int iconEntry)
	{
		markerManager =  SCR_MapMarkerManagerComponent.GetInstance();
		marker = new SCR_MapMarkerBase();
		marker.SetType(SCR_EMapMarkerType.PLACED_CUSTOM);
		marker.SetWorldPos(missionLocation[0], missionLocation[2]);
		marker.SetColorEntry(colorEntry);
		marker.SetIconEntry(iconEntry);
		marker.SetCustomText(missionName);
		
		markerManager.InsertStaticMarker(marker, false, true);
	}
	
	protected EntitySpawnParams GenerateSpawnParameters(vector location)
	{
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = location;
		return params;
	}
	
	void spawnEquipmentCrates(string lootType, int numBoxes, int minLootBoxItems, int maxLootBoxItems)
	{		
		//Setup loot contexts depending on which box needs to be spawned
		WR_LootSpawnContext lootContext;
		switch (lootType) {
			case "Weapon":
				lootContext = WR_LootSpawnContextPresets.GetWeaponBoxContext();
				break;
			case "HeavyWeapon":
				lootContext = WR_LootSpawnContextPresets.GetHeavyWeaponBoxContext();
				break;
			case "Medical":
				lootContext = WR_LootSpawnContextPresets.GetMedicalBoxContext();
				break;
			default:
				lootContext = WR_LootSpawnContextPresets.GetLootBoxContext();
		}
		
		//Loop through each box that is being spawned
		for (int i = 0; i < numBoxes; i++) {		
			//Find a safe place to spawn them
			vector spawnPos;
			vector randomPoint = WR_Utils.GetRandomPointWithinCircle(missionLocation, 5);
			bool foundSafePos = SCR_WorldTools.FindEmptyTerrainPosition(spawnPos, randomPoint, 35, 4, 4);
			if (!foundSafePos) {
				Print("[WR_AIMission]: Could not find safe place to spawn item");
				return;
			}
			
			Print("[WR_Mission]: Reward box of type " + lootType + " being spawned at: " + spawnPos);
			
			//Load resource and generate spawn parameters
			Resource resource = Resource.Load("{2E6EB383EEFDDC4F}Prefabs/Props/Military/Arsenal/ArsenalBoxes/US/WR_BigLootBox_US.et");
			EntitySpawnParams params = GenerateSpawnParameters(spawnPos);
			
			//Create box
			IEntity box = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
			if (!box) {
				Print("Box was not spawned!");
			}
			
			//Setup inventory and fill it with random items taken from the loot context we selected earlier
			auto inventoryStorage = SCR_UniversalInventoryStorageComponent.Cast(box.FindComponent(SCR_UniversalInventoryStorageComponent));
			auto inventoryStorageManager = SCR_InventoryStorageManagerComponent.Cast(box.FindComponent(SCR_InventoryStorageManagerComponent));
			array<ResourceName> items = lootContext.GetRandomItems(Math.RandomIntInclusive(minLootBoxItems, maxLootBoxItems));
			foreach (ResourceName item : items) {
				inventoryStorageManager.TrySpawnPrefabToStorage(item, inventoryStorage);
			}
			
			missionEntityList.Insert(box);
		}
	}
	
	bool isMissionComplete()
	{
		return isComplete;
	}
	
	void setMissionComplete(ENotification notif = ENotification.WR_MISSION_COMPLETE)
	{
		SCR_NotificationsComponent.SendToEveryone(notif);
		AudioSystem.PlaySound("{06C02AFB2CA882EB}Sounds/UI/Samples/Menu/UI_Task_Created.wav");
		Print(missionName + " has been marked as Completed");
		
		markerManager.RemoveStaticMarker(marker);
		
		int deleteTimer = 1000*60*3;	//3 Minutes
		GetGame().GetCallqueue().CallLater(clearMissionEntities, deleteTimer, false);
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
	
	void clearMissionEntities()
	{
		Print("Clearing mission entities..");
		foreach (IEntity itemToClear : missionEntityList) {
			SCR_EntityHelper.DeleteEntityAndChildren(itemToClear);
		}
		
		isComplete = true;
	}
	
	vector getMissionLocation()
	{
		return missionLocation;
	}
	
	void ~WR_Mission()
	{
			
	}
} 

