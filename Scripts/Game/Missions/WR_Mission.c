class WR_Mission
{
	ref WR_Logger<WR_Mission> logger = new WR_Logger<WR_Mission>(this);
	private static int s_iMissionCounter;
	
	int m_iMissionId;
	WR_MissionLocationEntity m_Location;
	WR_MissionDefinition m_Definition;
	WR_MissionStatus m_eStatus;
	WorldTimestamp timeSinceLastStatusChange;
	
	WR_MissionCompletionType m_eCompletionType = null;
	int m_iCompletingPlayerId;

	IEntity m_PropEntity;
	ref array<IEntity> m_aRewards = {};
	ref array<SCR_AIGroup> m_aGroups = {};
	
	ref SCR_MapMarkerBase m_Marker;

	void WR_Mission(notnull WR_MissionLocationEntity location, notnull WR_MissionDefinition definition)
	{
		s_iMissionCounter += 1;
		
		m_iMissionId = s_iMissionCounter;
		m_Location = location;
		m_Definition = definition;
		
		SetMissionStatus(WR_MissionStatus.Pending);
	}
	
	bool StartMission()
	{
		bool entitiesDidSpawn = SpawnProp() && SpawnRewards() && SpawnNpcGroups();
		
		if (entitiesDidSpawn)
		{
			SetMissionStatus(WR_MissionStatus.InProgress);
			m_Location.GetOnActivate().Insert(OnPlayerEnteredMissionLocation);
			logger.LogNormal(string.Format("Mission started successfully: %1 (ID: %2)", m_Definition.m_sName, m_iMissionId));
		}
		else
		{
			SetMissionStatus(WR_MissionStatus.Malformed);
			logger.LogWarning(string.Format("Mission failed to spawn: %1 (ID: %2)", m_Definition.m_sName, m_iMissionId));
		}
		
		return entitiesDidSpawn;
	}
	
	void SetMissionStatus(WR_MissionStatus newStatus)
	{
		m_eStatus = newStatus;
		timeSinceLastStatusChange = GetGame().GetWorld().GetTimestamp();
	}
	
	protected bool SpawnProp()
	{
		ResourceName propResource = m_Definition.m_sPropPrefab;
		if (!propResource) return true;
		
		m_PropEntity = WR_Utils.SpawnPrefabInWorld(propResource, m_Location.GetOrigin());
		if (!m_PropEntity)
		{
			logger.LogError(string.Format("Prop failed to spawn! (ID: %1)", m_iMissionId)); 
			return false;
		}
		
		return true;
	}
	
	protected bool SpawnRewards()
	{
		if (m_Definition.m_sRewardPrefabChoices.Count() == 0) return true;
		
		for (int i = 0; i < m_Definition.m_iNumberOfRewards; i++)
		{
			// Get Random Reward Resource
			ResourceName rewardPrefab = m_Definition.m_sRewardPrefabChoices.GetRandomElement();
			if (!rewardPrefab)
			{
				logger.LogError(string.Format("No reward prefabs defined for mission that requires rewards (ID: %1)", m_iMissionId));
				return false;
			}
			
			// Get random safe position
			vector spawnPos;
			bool safePosFound = WR_Utils.TryGetRandomSafePosWithinRadius(spawnPos, m_Location.GetOrigin(), 1.0, 10.0, 1.0, 1.0);
			if (!safePosFound)
			{
				logger.LogError(string.Format("Unable to find a safe spawn position for this mission's reward! (ID: %1)", m_iMissionId));
				return false;
			}
			
			// Attempt to spawn reward
			IEntity rewardEntity = WR_Utils.SpawnPrefabInWorld(rewardPrefab, spawnPos);
			if (!rewardEntity)
			{
				logger.LogError(string.Format("Mission reward failed to spawn! (ID: %1)", m_iMissionId));
				return false;
			}
			
			WR_Utils.RandomlyRotateAndOrientEntity(rewardEntity);
			m_aRewards.Insert(rewardEntity);
			
			// Insert OnDestroy function for vehicles
			auto vehicleDamageManager = SCR_VehicleDamageManagerComponent.Cast(rewardEntity.FindComponent(SCR_VehicleDamageManagerComponent));
			if (vehicleDamageManager)
			{
				vehicleDamageManager.GetOnVehicleDestroyed().Insert(OnRewardDestroyed);
			}
			
			// Check if we need to fill reward with loot
			if (m_Definition.m_eLootContext == WR_LootContext.NONE)
				continue;
			
			// Fill with Loot----------------------------------------------------------------------------------------------------------------------------------
			// Find the prefabs InventoryStorageManager, which may be a regular one, or a vehicle one
			auto inventoryStorageManager = SCR_InventoryStorageManagerComponent.Cast(rewardEntity.FindComponent(SCR_InventoryStorageManagerComponent));
			if (!inventoryStorageManager) 
				inventoryStorageManager = SCR_InventoryStorageManagerComponent.Cast(rewardEntity.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
			
			// Insert Randomly Selected Items
			if (inventoryStorageManager)
			{
				auto inventoryStorage = SCR_UniversalInventoryStorageComponent.Cast(rewardEntity.FindComponent(SCR_UniversalInventoryStorageComponent));
				
				WR_LootSpawningComponent lootSpawningComponent = WR_LootSpawningComponent.GetInstance();
				array<ResourceName> items = lootSpawningComponent.GetRandomItemsByBudget(m_Definition.m_eLootContext, 0.75, 1.5); // TODO: parameterize these floats. maybe make a lootbox reward config
				foreach (ResourceName item : items)
					inventoryStorageManager.TrySpawnPrefabToStorage(item, inventoryStorage);
			}
		
		}
		
		return true;
	}
	
	protected bool SpawnNpcGroups()
	{
		foreach (ResourceName aiGroupResource : m_Definition.m_aAIGroupPrefabs)
		{
			// Find a safe spot for the NPCs to spawn
			vector spawnPos;
			bool safePosFound = WR_Utils.TryGetRandomSafePosWithinRadius(spawnPos, m_Location.GetOrigin(), 1.0, 10.0, 1.0, 1.0);
			if (!safePosFound)
			{
				logger.LogError(string.Format("Unable to find a safe spawn position for a mission AI Group! (ID: %1)", m_iMissionId));
				return false;
			}	
			
			// Spawn the group prefab
			IEntity aiGroupEntity = WR_Utils.SpawnPrefabInWorld(aiGroupResource, spawnPos);
			SCR_AIGroup group = SCR_AIGroup.Cast(aiGroupEntity);
			if (!group)
			{
				logger.LogError(string.Format("AI Group was not spawned! (ID: %1)", m_iMissionId));
				return false;
			}	
			
			// Configure group
			group.SetDeleteWhenEmpty(true);
			group.SetFaction(GetGame().GetFactionManager().GetFactionByKey("CIV"));	

			// Set group waypoint to defend mission location
			ResourceName waypointResource = "{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Defend.et";
			IEntity waypointEntity = WR_Utils.SpawnPrefabInWorld(waypointResource, m_Location.GetOrigin());
			SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(waypointEntity);
			group.AddWaypoint(waypoint);
			
			m_aGroups.Insert(group);
		}
		return true;
	}
	
	protected bool AreAllNpcsDead()
	{
		foreach (SCR_AIGroup group : m_aGroups)
			if (group) return false;
		
		logger.LogNormal("All AI are dead at mission: " + m_Definition.m_sName);
		
		return true;
	}
	
	protected void OnPlayerEnteredMissionLocation()
	{
		if (m_eStatus != WR_MissionStatus.InProgress)
			return;
		
		if (!AreAllNpcsDead()) 
			return;
		
		array<IEntity> playersInTrigger = {};
		m_Location.GetPlayersByFactionInsideTrigger(playersInTrigger);
		if (playersInTrigger.Count() > 0)
		{
			PlayerManager playerManager = GetGame().GetPlayerManager();
			m_iCompletingPlayerId = playerManager.GetPlayerIdFromControlledEntity(playersInTrigger[0]);
		}
		
		m_eCompletionType = WR_MissionCompletionType.Success;
		SetMissionStatus(WR_MissionStatus.Complete);
	}
	
	protected void OnRewardDestroyed(int playerId)
	{
		logger.LogNormal(string.Format("Reward destroyed at mission: %1 (ID: %2)", m_Definition.m_sName, m_iMissionId));
		
		// We only care about this if the mission is still in progress
		if (m_eStatus != WR_MissionStatus.InProgress) return;
		
		m_iCompletingPlayerId = playerId;
		m_eCompletionType = WR_MissionCompletionType.Destroyed;
		SetMissionStatus(WR_MissionStatus.Complete);
	}
	
	void DeleteMissionEntities(bool includeRewards)
	{
		logger.LogNormal(string.Format("Deleting mission entities for mission: %1 (ID: %2)", m_Definition.m_sName, m_iMissionId));
		
		// Delete prop and NPCs
		SCR_EntityHelper.DeleteEntityAndChildren(m_PropEntity);
		foreach (SCR_AIGroup group : m_aGroups)
			SCR_EntityHelper.DeleteEntityAndChildren(group);
		
		// Conditionally delete rewards
		if (!includeRewards)
			return;
		
		foreach (IEntity ent : m_aRewards)
			SCR_EntityHelper.DeleteEntityAndChildren(ent);
	}
	
	WR_MissionLocationEntity GetLocation()
	{
		return m_Location;
	}
	
	WR_MissionDefinition GetDefinition()
	{
		return m_Definition;
	}
	
	WR_MissionType GetType()
	{
		return m_Definition.m_eType;
	}
	
	WR_MissionStatus GetStatus()
	{
		return m_eStatus;
	}
	
	WR_MissionCompletionType GetCompletionType()
	{
		return m_eCompletionType;
	}
	
	WorldTimestamp GetLastTimestamp()
	{
		return timeSinceLastStatusChange;
	}
	
	SCR_MapMarkerBase GetMarker()
	{
		return m_Marker;
	}
	
	void SetMarker(SCR_MapMarkerBase m)
	{
		m_Marker = m;
	}
	
	int GetCompletingPlayerId()
	{
		return m_iCompletingPlayerId;
	}
	
	int GetMissionId()
	{
		return m_iMissionId;
	}
	
	void SetCompletionType(WR_MissionCompletionType type)
	{
		m_eCompletionType = type;
	}
	
	void ~WR_Mission()
	{
		delete m_aGroups;
		delete m_aRewards;
	}

}