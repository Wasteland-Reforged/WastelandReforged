class WR_Mission
{
	const string RESOURCE_AI_WAYPOINT_DEFEND = "{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Defend.et";

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
	
	int m_iMarkerId;

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
		// Fail to spawn if any entities are within the mission radius
		vector safePos;
		if (!SCR_WorldTools.FindEmptyTerrainPosition(safePos, m_Location.GetOrigin(), m_Location.GetSphereRadius(), m_Location.GetSphereRadius())) {
			logger.LogError(string.Format("Could not find empty terrain position when starting mission: %1 (ID: %2)", m_Definition.m_sName, m_iMissionId));
			OnMissionMalformed();
			return false;
		}
		
		// Spawn each set of mission entities individually, while validating that they spawned correctly
		if (!SpawnProp()) {
			OnMissionMalformed();
			return false;
		}
		if (!SpawnRewards()) {
			OnMissionMalformed();
			return false;
		}
		if (!SpawnNpcGroups()) {
			OnMissionMalformed();
			return false;
		}

		SetMissionStatus(WR_MissionStatus.InProgress);
		m_Location.GetOnActivate().Insert(OnPlayerEnteredMissionLocation);
		logger.LogNormal(string.Format("Mission started successfully: %1 (ID: %2)", m_Definition.m_sName, m_iMissionId));
		
		return true;
	}
	
	void OnMissionMalformed()
	{
		//SetMissionStatus(WR_MissionStatus.Malformed);
		logger.LogWarning(string.Format("Mission failed to spawn: %1 (ID: %2)", m_Definition.m_sName, m_iMissionId));
	}
	
	void SetMissionStatus(WR_MissionStatus newStatus)
	{
		m_eStatus = newStatus;
		timeSinceLastStatusChange = GetGame().GetWorld().GetTimestamp();
	}
	
	protected bool SpawnProp()
	{
		if (!m_Definition.m_sPropPrefabChoices || m_Definition.m_sPropPrefabChoices.Count() == 0)
			return true;
		
		ResourceName propResource = m_Definition.m_sPropPrefabChoices.GetRandomElement();

		vector safepos;
		if (!SCR_WorldTools.FindEmptyTerrainPosition(safepos, m_Location.GetOrigin(), 2)) {
			logger.LogError(string.Format("Could not find empty terrain position for prop! (ID: %1)", m_iMissionId)); 
			return false;
		}
		
		m_PropEntity = WR_Utils.SpawnPrefabInWorld(propResource, safepos);
		if (!m_PropEntity)
		{
			logger.LogError(string.Format("Prop failed to spawn! (ID: %1)", m_iMissionId)); 
			return false;
		}
		
		logger.LogNormal(string.Format("Spawned prop at: %1)", m_PropEntity.GetOrigin())); 
		
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
			
			// Prevent rewards from entering garbage collection
			SCR_GarbageSystem GarbageSys = SCR_GarbageSystem.GetByEntityWorld(rewardEntity);
			if (GarbageSys)
				GarbageSys.UpdateBlacklist(rewardEntity, true);
			
			// Insert OnDestroy function for vehicles
			auto vehicleDamageManager = SCR_VehicleDamageManagerComponent.Cast(rewardEntity.FindComponent(SCR_VehicleDamageManagerComponent));
			if (vehicleDamageManager)
			{
				vehicleDamageManager.GetOnVehicleDestroyed().Insert(CheckRewardDestroyed);
			}

			// Check if we need to fill reward with loot
			if (m_Definition.m_eLootContext == WR_LootContext.NONE)
				continue;
			
			// Fill with Loot----------------------------------------------------------------------------------------------------------------------------------
			WR_LootSpawningComponent lootSpawningComponent = WR_LootSpawningComponent.GetInstance();
			array<ResourceName> items = lootSpawningComponent.GetRandomItemsByBudget(m_Definition.m_eLootContext, m_Definition.m_fLootBudget, m_Definition.m_fLootBoxAdditionalItemMultiplier);
			
			SCR_InventoryStorageManagerComponent inventoryStorageManager = SCR_InventoryStorageManagerComponent.Cast(rewardEntity.FindComponent(SCR_InventoryStorageManagerComponent));
			SCR_VehicleInventoryStorageManagerComponent vicStorageManager = SCR_VehicleInventoryStorageManagerComponent.Cast(rewardEntity.FindComponent(SCR_VehicleInventoryStorageManagerComponent));

			if (inventoryStorageManager)
			{
				Print("Found inventory storage manager");
				foreach (ResourceName item : items)
					inventoryStorageManager.TrySpawnPrefabToStorage(item);
			}
			else if (vicStorageManager)
			{
				Print("Found vehicle inventory storage manager");
				foreach (ResourceName item : items)
					vicStorageManager.TrySpawnPrefabToStorage(item);
			}
			else 
			{
				Print("Could not find inventory storage manager");
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
				logger.LogError(string.Format("Unable to find a safe spawn position for mission bots! (ID: %1)", m_iMissionId));
				return false;
			}	
			
			// Spawn the group prefab
			IEntity aiGroupEntity = WR_Utils.SpawnPrefabInWorld(aiGroupResource, spawnPos);
			SCR_AIGroup group = SCR_AIGroup.Cast(aiGroupEntity);
			if (!group)
			{
				logger.LogError(string.Format("Bot group was not spawned! (ID: %1)", m_iMissionId));
				return false;
			}	
			
			// Configure group
			group.SetDeleteWhenEmpty(true);
			group.SetFaction(GetGame().GetFactionManager().GetFactionByKey("CIV"));	

			// Set group waypoint to defend mission location
			IEntity waypointEntity = WR_Utils.SpawnPrefabInWorld(RESOURCE_AI_WAYPOINT_DEFEND, m_Location.GetOrigin());
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

		logger.LogNormal(string.Format("All bots neutralized at mission: %1 (ID: %2)", m_Definition.m_sName, m_iMissionId));
		
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
	
	protected void CheckRewardDestroyed(int playerId)
	{
		// We only care about this if the mission is still in progress
		if (m_eStatus != WR_MissionStatus.InProgress) {
			return;
		}
		
		// Loop through all mission rewards and see if any are vehicles that are now destroyed
		foreach (IEntity rewardEntity : m_aRewards)
		{
			SCR_VehicleDamageManagerComponent vehicleDamageManager = SCR_VehicleDamageManagerComponent.Cast(rewardEntity.FindComponent(SCR_VehicleDamageManagerComponent));
			if (vehicleDamageManager)
			{
				if (vehicleDamageManager.GetState() == EDamageState.DESTROYED)
				{
					logger.LogNormal(string.Format("Mission vehicle reward destroyed for mission: %1 (ID: %2)", m_Definition.m_sName, m_iMissionId));
					m_iCompletingPlayerId = playerId;
					m_eCompletionType = WR_MissionCompletionType.Destroyed;
					SetMissionStatus(WR_MissionStatus.Complete);
				}
			}
		}
		
		
	}
	
	bool AreAllRewardsStolen()
	{
		if (m_aRewards.Count() == 0)
			return false;
		
		foreach (IEntity ent : m_aRewards) 
		{
			if (m_Location.QueryEntityInside(ent))
				return false;
		}
		
		return true;
	}
	
	void DeleteMissionEntities(bool includeRewards)
	{
		logger.LogNormal(string.Format("Deleting mission entities for mission: %1 (ID: %2)", m_Definition.m_sName, m_iMissionId));
		
		// Delete prop and NPCs
		SCR_EntityHelper.DeleteEntityAndChildren(m_PropEntity);
		foreach (SCR_AIGroup group : m_aGroups)
			SCR_EntityHelper.DeleteEntityAndChildren(group);
		
		if (!includeRewards)
			return;
		
		// Delete rewards under certain conditions
		foreach (IEntity ent : m_aRewards) {
			
			if (!ent) continue;
			
			// Skip crates that are inside vehicles
			UniversalInventoryStorageComponent storageComp = UniversalInventoryStorageComponent.Cast(ent.FindComponent(UniversalInventoryStorageComponent));
			if (storageComp && storageComp.GetParentSlot())
				continue;
			
			// Skip vehicles that are occupied
			SCR_BaseCompartmentManagerComponent vehicleComp = SCR_BaseCompartmentManagerComponent.Cast(ent.FindComponent(SCR_BaseCompartmentManagerComponent));
			if (vehicleComp && vehicleComp.GetOccupantCount() > 0)
				continue;
			
			// Otherwise, If reward is within mission radius, delete it
			if (m_Location.QueryEntityInside(ent))
				SCR_EntityHelper.DeleteEntityAndChildren(ent);
		}

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

	int GetMarkerId()
	{
		return m_iMarkerId;
	}
	
	void SetMarkerId(int markerId)
	{
		m_iMarkerId = markerId;
	}
	
	WR_StaticMarkerParameters GetMarkerParameters()
	{
		ref WR_StaticMarkerParameters parameters = new WR_StaticMarkerParameters();
		
		parameters.m_eType = SCR_EMapMarkerType.PLACED_CUSTOM;
		parameters.m_sCustomText = GetDefinition().m_sName;
		parameters.m_iIconEntry = GetDefinition().m_eMissionIcon;
		parameters.m_vWorldPos = GetLocation().GetOrigin();
		
		switch (GetStatus())
		{
			case WR_MissionStatus.Pending:
			case WR_MissionStatus.Malformed:
			case WR_MissionStatus.InProgress:
			{
				parameters.m_iColorEntry = GetDefinition().m_eMissionColor;
				break;
			}
			case WR_MissionStatus.Complete:
			case WR_MissionStatus.AwaitingMarkerCleanup:
			case WR_MissionStatus.AwaitingCleanup:
			{
				parameters.m_iColorEntry = WR_MapMarkerConfigColorIndices.DarkGrey;
				break;
			}
			default:
			{
				parameters.m_iColorEntry = GetDefinition().m_eMissionColor;
				break;
			}
		}
	
		return parameters;
	}
	
	void ~WR_Mission()
	{
		delete m_aGroups;
		delete m_aRewards;
	}
}