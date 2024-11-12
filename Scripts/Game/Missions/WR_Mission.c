class WR_Mission
{
	WR_MissionLocationEntity m_Location;
	WR_MissionDefinition m_Definition;
	WR_MissionStatus m_eStatus;
	WorldTimestamp timeSinceLastStatusChange;
	
	int m_iCompletingPlayerId;
	int m_iDestroyingPlayerId;

	IEntity m_PropEntity;
	ref array<IEntity> m_aRewards = {};
	ref array<SCR_AIGroup> m_aGroups = {};

	void WR_Mission(notnull WR_MissionLocationEntity location, notnull WR_MissionDefinition definition)
	{
		m_Location = location;
		m_Definition = definition;
		ChangeMissionStatus(WR_MissionStatus.Pending);
	}
	
	void StartMission()
	{
		if (!SpawnProp() || !SpawnRewards() || !SpawnAIGroups())
			ChangeMissionStatus(WR_MissionStatus.FailedToSpawn);
		else {
			ChangeMissionStatus(WR_MissionStatus.InProgress);
			m_Location.GetOnActivate().Insert(OnPlayerEnteredMissionLocation);
		}
	}
	
	void ChangeMissionStatus(WR_MissionStatus newStatus)
	{
		m_eStatus = newStatus;
		timeSinceLastStatusChange = GetGame().GetWorld().GetTimestamp();
	}
	
	protected bool SpawnProp()
	{
		ResourceName propResource = m_Definition.m_sPropPrefab;
		if (!propResource) return true;
		
		m_PropEntity = WR_Utils.SpawnPrefabInWorld(propResource, m_Location.GetOrigin());
		if (!m_PropEntity) {
			Print("[WASTELAND]: WR_Mission: Mission prop failed to spawn!", LogLevel.ERROR);
			return false;
		}
		
		return true;
	}
	
	protected bool SpawnRewards()
	{
		for (int i = 0; i < m_Definition.m_iNumberOfRewards; i++)
		{
			// Get Random Reward Resource
			ResourceName rewardPrefab = m_Definition.m_sRewardPrefabChoices.GetRandomElement();
			if (!rewardPrefab) {
				Print("[WASTELAND]: WR_Mission: No reward prefabs defined for mission that requires rewards", LogLevel.ERROR);
				return false;
			}
			
			// Get Random Safe Position
			vector spawnPos;
			bool safePosFound = WR_Utils.TryGetRandomSafePosWithinRadius(spawnPos, m_Location.GetOrigin(), 1.0, 10.0, 10.0, 2.0);
			if (!safePosFound) {
				Print("[WASTELAND] WR_Mission: Unable to find a safe spawn position for this mission's reward!", LogLevel.ERROR);
				return false;
			}
			
			// Attempt to Spawn Reward
			IEntity rewardEntity = WR_Utils.SpawnPrefabInWorld(rewardPrefab, spawnPos);
			if (!rewardEntity) {
				Print("[WASTELAND] WR_Mission: Mission reward failed to spawn!", LogLevel.ERROR);
				return false;
			}
			
			WR_Utils.RandomlyRotateAndOrientEntity(rewardEntity);
			m_aRewards.Insert(rewardEntity);
			
			// Check if we need to fill reward with loot
			if (m_Definition.m_eLootContext == WR_LootContextType.NONE) continue;
			
			// Fill with Loot----------------------------------------------------------------------------------------------------------------------------------
			WR_LootSpawnContext lootContext = WR_LootSpawnContextPresets.GetLootContextByType(m_Definition.m_eLootContext);
			int minItems = m_Definition.m_iMinItemsInBox;
			int maxItems = m_Definition.m_iMaxItemsInBox;
			
			// Find the prefabs InventoryStorageManager, which may be a regular one, or a vehicle one
			auto inventoryStorageManager = SCR_InventoryStorageManagerComponent.Cast(rewardEntity.FindComponent(SCR_InventoryStorageManagerComponent));
			if (!inventoryStorageManager) 
				inventoryStorageManager = SCR_InventoryStorageManagerComponent.Cast(rewardEntity.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
			
			// Insert Randomly Selected Items
			if (inventoryStorageManager)
			{
				auto inventoryStorage = SCR_UniversalInventoryStorageComponent.Cast(rewardEntity.FindComponent(SCR_UniversalInventoryStorageComponent));
				
				array<ResourceName> items = lootContext.GetRandomItems(Math.RandomIntInclusive(minItems, maxItems), minExtraMags: 5,  maxExtraMags: 12);
				foreach (ResourceName item : items) {
					inventoryStorageManager.TrySpawnPrefabToStorage(item, inventoryStorage);
				}
			}
			
			//Insert OnDestroy function for vehicles
			auto vehicleDamageManager = SCR_VehicleDamageManagerComponent.Cast(rewardEntity.FindComponent(SCR_VehicleDamageManagerComponent));
			if (vehicleDamageManager)
				vehicleDamageManager.GetOnVehicleDestroyed().Insert(OnRewardDestroyed);
		}
		
		return true;
	}
	
	protected bool SpawnAIGroups()
	{
		foreach (ResourceName aiGroupResource : m_Definition.m_aAIGroupPrefabs)
		{
			// Find a safe spot for the NPCs to spawn
			vector spawnPos;
			bool safePosFound = WR_Utils.TryGetRandomSafePosWithinRadius(spawnPos, m_Location.GetOrigin(), 1.0, 10.0, 1.0, 1.0);
			if (!safePosFound) {
				Print("[WASTELAND] WR_MissionControllerComponent: Unable to find a safe spawn position for a mission AI Group!", LogLevel.ERROR);
				return false;
			}	
			
			// Spawn the group prefab
			IEntity aiGroupEntity = WR_Utils.SpawnPrefabInWorld(aiGroupResource, spawnPos);
			SCR_AIGroup aiGroup = SCR_AIGroup.Cast(aiGroupEntity);
			if (!aiGroup) {
				Print("[WASTELAND] WR_MissionControllerComponent: AI Group was not spawned!", LogLevel.ERROR);
				return false;
			}	
			
			// Configure Group
			aiGroup.SetDeleteWhenEmpty(true);
			aiGroup.SetFaction(GetGame().GetFactionManager().GetFactionByKey("CIV"));	

			// Set group waypoint to defend mission location
			ResourceName waypointResource = "{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Defend.et";
			IEntity waypointEntity = WR_Utils.SpawnPrefabInWorld(waypointResource, m_Location.GetOrigin());
			SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(waypointEntity);
			aiGroup.AddWaypoint(waypoint);
			
			m_aGroups.Insert(aiGroup);
		}
		return true;
	}
	
	protected bool AreAllNPCsDead()
	{
		foreach (SCR_AIGroup group : m_aGroups)
			if (group) return false;
		
		return true;
	}
	
	protected void OnPlayerEnteredMissionLocation()
	{
		if (!AreAllNPCsDead()) 
			return;
		
		//m_iCompletingPlayerId = ?
		ChangeMissionStatus(WR_MissionStatus.Complete);
	}
	
	protected void OnRewardDestroyed(int playerID)
	{
		// We only care about this if the mission is still in progress
		if (m_eStatus != WR_MissionStatus.InProgress) return;
		
		m_iDestroyingPlayerId = playerID;
		ChangeMissionStatus(WR_MissionStatus.RewardDestroyed);
	}
	
	void DeleteMissionEntities(bool includeRewards)
	{
		// Delete Prop and AI Groups
		SCR_EntityHelper.DeleteEntityAndChildren(m_PropEntity);
		foreach (SCR_AIGroup group : m_aGroups)
			SCR_EntityHelper.DeleteEntityAndChildren(group);
		
		// Conditionally delete rewards		Missions fail on: Failed to Spawn, Timeout, Reward Destroyed
		if (!includeRewards) return;
		foreach (IEntity ent : m_aRewards)
			SCR_EntityHelper.DeleteEntityAndChildren(ent);

	}
	
	WR_MissionLocationEntity getLocation()
	{
		return m_Location;
	}
	
	WR_MissionDefinition getDefinition()
	{
		return m_Definition;
	}
	
	WR_MissionStatus getStatus()
	{
		return m_eStatus;
	}
	
	WorldTimestamp getLastTimestamp()
	{
		return timeSinceLastStatusChange;
	}
	
	void ~WR_Mission()
	{
		delete m_aGroups;
		delete m_aRewards;
	}

}