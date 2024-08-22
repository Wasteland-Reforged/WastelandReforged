class WR_MissionControllerComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class WR_MissionControllerComponent : SCR_BaseGameModeComponent
{
	[Attribute("", UIWidgets.Object, "Mission controller config")]
	protected ref WR_MissionControllerConfig m_Config;
    
	protected SCR_BaseGameMode m_Parent;
	protected BaseRplComponent m_RplComponent;
	
    protected int m_iMissionCounter;
    
	protected ref map<int, ref WR_Mission> m_mActiveMissions = new map<int, ref WR_Mission>();
	
	protected ref map<WR_MissionType, WR_MissionDefinition> m_mMissionDefinitions = new map<WR_MissionType, WR_MissionDefinition>();
	
	[RplProp(onRplName: "OnNewMissionReceived")]
	protected ref WR_Mission m_lastUpdatedMission = new WR_Mission();
	
	protected ref WR_MissionUiElementHelper uiHelper;

	private bool m_bIsInitialRplLoadDone = false;
	
	override void OnPostInit(IEntity owner)
	{
		m_Parent = SCR_BaseGameMode.Cast(owner);
		if (!m_Parent)
		{
			Print("[WASTELAND] WR_MissionManagerComponent Parent entity must be SCR_BaseGameMode!", LogLevel.ERROR);
			return;
		}
		
		m_RplComponent = BaseRplComponent.Cast(m_Parent.FindComponent(BaseRplComponent));
		if (!m_RplComponent)
		{
			Print("[WASTELAND] WR_MissionManagerComponent: Parent entity must have RplComponent!", LogLevel.ERROR);
			return;
		}
		
		if (!m_Config)
		{
			Print("[WASTELAND] WR_MissionManagerComponent: Mission controller must have a valid config!", LogLevel.ERROR);
			return;
		}
		
		foreach (WR_MissionDefinition definition : m_Config.m_aMissionDefinitions)
			m_mMissionDefinitions.Insert(definition.m_eType, definition);

		Print("[WASTELAND] WR_MissionControllerComponent: Mission controller initialized.", LogLevel.NORMAL);
	}
	
	WR_MissionDefinition GetMissionDefinition(WR_MissionType type)
	{
		WR_MissionDefinition definition;
		if (m_mMissionDefinitions.Find(type, definition))
			return definition;
		
		Print("[WASTELAND] WR_MissionControllerComponent: Mission definition not found for mission type " + type + "!", LogLevel.ERROR);
		return null;
	}
	
	override void OnGameModeStart()
	{
		if (m_RplComponent.Role() == RplRole.Authority)
		{
			float initialDelayMs = m_Config.m_fInitialMissionDelay * 60 * 1000;
			float delayBetweenMissionStartMs = m_Config.m_fNewMissionDelay * 60 * 1000;
			
			for (int i = 0; i < m_Config.m_iMaxActiveMissions; i++)
			{
				// TODO: upgrade this to start multiple missions, track existing missions, give "mission about to start hint", etc.
				GetGame().GetCallqueue().CallLater(StartRandomMission, initialDelayMs + i * delayBetweenMissionStartMs, false);
			}
		}
	}
	
    protected void StartRandomMission()
    {		
		// Ensure we are the authority here
        if (m_RplComponent.Role() != RplRole.Authority) return;

        if (m_Config.m_iMaxActiveMissions == 0 || m_mActiveMissions.Count() >= m_Config.m_iMaxActiveMissions)
		{
			Print("[WASTELAND] WR_MissionManagerComponent: Not enough capacity to start a new mission! Aborting mission creation.", LogLevel.ERROR);
			return;
		}

		int missionId = GetNewMissionId();
		WR_MissionType missionType = GetRandomMissionType();
		
		// Create mission record
		m_lastUpdatedMission = WR_Mission.CreateMission(missionId, missionType);
		
		// Find mission definition corresponding to selected mission type
		WR_MissionDefinition missionDefinition = FindMissionDefinitionByMissionType(m_lastUpdatedMission.GetType());
		if (!missionDefinition)
		{
			Print("[WASTELAND] WR_MissionControllerComponent: No definition found for provided type! (ID: " + missionId + ") Aborting mission creation.", LogLevel.ERROR);
		}
		m_lastUpdatedMission.SetDefinition(missionDefinition);
		
		Print("[WASTELAND] WR_MissionManagerComponent: Starting new mission: " + missionDefinition.m_sName + " (ID: " + missionId + ")", LogLevel.NORMAL);
		
		// Find a location for this mission
		WR_MissionLocationEntity missionLocation = GetRandomVacantMissionLocation(missionDefinition.m_eSize);

		if (!missionLocation)
		{
			Print("[WASTELAND] WR_MissionManagerComponent: Unable to get location for new mission! (ID: " + missionId + ") Aborting mission creation.", LogLevel.ERROR);
			
			// Destroy this mission and queue another one to be started.
			DestroyMission(m_lastUpdatedMission);
			GetGame().GetCallqueue().CallLater(StartRandomMission, m_Config.m_fNewMissionDelay * 60 * 1000, false); // TODO: make the delay pull from a member var
			
			return;
		}

		m_lastUpdatedMission.SetMissionLocation(missionLocation); // TODO: make CreateMission just take the mission location itself instead of its origin
		missionLocation.SetCurrentMission(m_lastUpdatedMission);		
		
		// Run mission on server (for in-game hosted servers)
		bool areObjectsCreated = InstantiateMissionWorldObjects(m_lastUpdatedMission);
		if (!areObjectsCreated)
		{
			Print("[WASTELAND] WR_MissionControllerComponent: Failed to instantiate mission world objects! (ID: " + missionId + ") Aborting mission creation.", LogLevel.ERROR);
			
			// Destroy this mission and queue another one to be started.
			DestroyMission(m_lastUpdatedMission);
			GetGame().GetCallqueue().CallLater(StartRandomMission, m_Config.m_fNewMissionDelay * 60 * 1000, false); // TODO: make the delay pull from a member var
			
			return;
		}
		
		HandleUpdatedMissionRecord(m_lastUpdatedMission, false);
		
		// Send updated mission record to proxies
		Replication.BumpMe();
		
		Print("[WASTELAND] WR_MissionManagerComponent: Mission started successfully: " + m_lastUpdatedMission.GetName() + " (ID: " + missionId + ")", LogLevel.NORMAL);
    }
	
    protected void ReceiveNewMissionOnProxy()
    {
        if (m_RplComponent.Role() != RplRole.Proxy) return;
		if (!m_mActiveMissions) return;
		if (m_lastUpdatedMission.IsEmptyMission()) return;
		
		Print("[WASTELAND] WR_MissionManagerComponent: New mission received from authority (ID: " + m_lastUpdatedMission.GetMissionId() + ")", LogLevel.NORMAL);
		
		HandleUpdatedMissionRecord(m_lastUpdatedMission, false);
    }
	
	protected void HandleUpdatedMissionRecord(WR_Mission mission, bool isJipRplLoad)
	{
		WR_Mission newMission = mission;
		//bool missionExists = m_mActiveMissions.Find(newMission.GetMissionId(), mission);
		bool missionExists = m_mActiveMissions.Contains(newMission.GetMissionId());
		
		// Populate mission definition from local config file
		WR_MissionDefinition definition = GetMissionDefinition(newMission.GetType());
		newMission.SetDefinition(definition); 
		
		//mission = newMission;
		if (!missionExists)
		{
			// We've never received this mission ID, so add it to the list of active missions.
			m_mActiveMissions.Set(newMission.GetMissionId(), newMission);

			// Markers should only be created on the authority, since Reforger already includes infrastructure to replicate them
			if (m_RplComponent.Role() == RplRole.Authority)
			{
				WR_MissionUiElementHelper.CreateMarker(newMission);	
			}
			
			// Only active player should see notifications, not JIP clients who are receiving initial mission info
			if (!isJipRplLoad)
				WR_MissionUiElementHelper.SendMissionNotificationByStatus(newMission);
		}
		else
		{
			// We have received a mission with this ID before, so update or delete existing mission record as required
			WR_MissionStatus currStatus = newMission.GetStatus();
			switch	(currStatus)
			{
				case WR_MissionStatus.Complete:
				case WR_MissionStatus.Failed:
				{
					SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
					
					if (m_RplComponent.Role() == RplRole.Authority)
					{
						WR_MissionUiElementHelper.DeleteMarker(newMission);
					}
					
					// Only active players should see notifications, not JIP clients who are receiving initial mission info
					if (!isJipRplLoad)
						WR_MissionUiElementHelper.SendMissionNotificationByStatus(newMission);
		
					m_mActiveMissions.Take(newMission.GetMissionId(), newMission);
					
					break;
				}
			}
		}
	}
	
	private bool InstantiateMissionWorldObjects(WR_Mission mission)
	{
		// Spawn rewards
		IEntity rewardEntities;
		bool isRewardSpawned = SpawnReward(mission, rewardEntities);
		
		// Spawn props
		IEntity propEntity;
		bool isPropSpawned = SpawnProp(mission, propEntity);

		// Spawn NPCs
		ref array<SCR_AIGroup> aiGroups;
		bool areAiGroupsSpawned = SpawnAiGroups(mission, aiGroups);

		mission.SetMissionEntities(rewardEntities, propEntity, aiGroups);
		
		if (!(isRewardSpawned && isPropSpawned && areAiGroupsSpawned))
		{
			Print("[WASTELAND] WR_MissionControllerComponent: Failed to spawn mission objects in the world! (mission ID: " + mission.GetMissionId() + ")");
			return false;
		}

		return true;
	}

    private int GetNewMissionId()
    {
        m_iMissionCounter += 1;
        return m_iMissionCounter;
    }
	
    WR_MissionType GetRandomMissionType()
    {
		WR_MissionDefinition definition = m_Config.m_aMissionDefinitions.GetRandomElement();
        return definition.m_eType;
    }
			
	protected WR_MissionLocationEntity GetRandomVacantMissionLocation(WR_MissionLocationSize requiredSize)
	{			
		array<WR_MissionLocationEntity> vacantLocations = WR_MissionLocationEntity.GetAllVacantLocations();
		if (vacantLocations.Count() == 0)
		{
			Print("[WASTELAND] WR_MissionManagerComponent: No vacant mission locations!", LogLevel.ERROR);
			return null;
		}
		
		WR_MissionLocationEntity randomLocation;
		
		// TODO: this is a lame, unoptimized way of doing it, but it works for now.
		int attempts = 1;
		while (!randomLocation || requiredSize != randomLocation.GetSize())
		{
			if (attempts == 10) return null;
			
			randomLocation = vacantLocations.GetRandomElement();
			attempts++;
		}
		
		// TODO: check if there are any entities inside/nearby this mission location's trigger zone. We don't want to spawn missions on people's heads

		return randomLocation;
	}
	
	private WR_MissionDefinition FindMissionDefinitionByMissionType(WR_MissionType type)
	{
		if (!m_Config) return null; // TODO: add error logging
		if (!m_Config.m_aMissionDefinitions) return null; // TODO: add error logging
		
		foreach (WR_MissionDefinition definition : m_Config.m_aMissionDefinitions)
			if (definition.m_eType == type) return definition;
		
		return null;
	}
	
	private bool SpawnProp(WR_Mission mission, out IEntity propEntity)
	{
		if (!mission.GetDefinition())
		{
			Print("[WASTELAND] WR_MissionControllerComponent: Tried to spawn mission prop, but no mission definition was found!", LogLevel.ERROR);
			return false;
		}
		
		ResourceName propResource = mission.GetDefinition().m_sPropPrefab;
		if (!propResource) return true; // Props are not required, so exit successfully if none are defined

		// Get safe pos
		vector safePos;
		WR_Utils.TryGetRandomSafePosWithinRadius(safePos, mission.GetPosition(), mission.GetMissionLocation().GetSphereRadius(), 10.0, 10.0, 2.0);
		propEntity = WR_Utils.SpawnPrefabInWorld(propResource, safePos);
		
		// Set random heading
		vector randomDir = WR_Utils.GetRandomHorizontalDirectionAngles();
		propEntity.SetYawPitchRoll(randomDir);
		
		// Orient the object to terrain normal
		vector transform[4];
		propEntity.GetTransform(transform);
		SCR_TerrainHelper.OrientToTerrain(transform);
		propEntity.SetTransform(transform);
		
		return true;
	}
	
	private bool SpawnAiGroups(WR_Mission mission, out array<SCR_AIGroup> aiGroups)
	{
		if (!mission.GetDefinition())
		{
			Print("[WASTELAND] WR_MissionControllerComponent: Tried to spawn mission AI groups, but no mission definition was found!", LogLevel.ERROR);
			return false;
		}
		
		array<ResourceName> aiGroupResources = mission.GetDefinition().m_aAIGroupPrefabs;
		if (!aiGroupResources) return true; // NPCs are not required, so exit successfully
		// TODO: need to make sure missions without AI can still be completed by just walking up to them 
		
		aiGroups = {};
		foreach (ResourceName aiGroupResource : aiGroupResources)
		{
			// Find a safe spot for the NPCs to spawn
			vector spawnPos;
			WR_Utils.TryGetRandomSafePosWithinRadius(spawnPos, mission.GetPosition(), 15.0, 10.0, 10.0, 2.0); // TODO: read these floats from a config
			
			// TODO: The return value of TryGetRandomSafePosWithinRadius is kinda broken, so disabling the safePosFound check for now. Fix that method!!!
			//bool safePosFound =
//			if (!safePosFound)
//			{
//				Print("[WASTELAND] WR_MissionControllerComponent: A safe spawn position was unable to be found for this mission's AI!", LogLevel.ERROR);
//				return false;
//			}
			
			// Spawn the group prefab
			IEntity aiGroupEntity = WR_Utils.SpawnPrefabInWorld(aiGroupResource, spawnPos);
			SCR_AIGroup aiGroup = SCR_AIGroup.Cast(aiGroupEntity);
			
			// Guarantee this group becomes null when last member dies
			aiGroup.SetDeleteWhenEmpty(true);
			
			// All NPC fighters should be on civilian faction
			aiGroup.SetFaction(GetGame().GetFactionManager().GetFactionByKey("CIV"));	
	
			// Command the group to defend the mission location
			ResourceName waypointResource = "{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Defend.et";
			IEntity waypointEntity = WR_Utils.SpawnPrefabInWorld(waypointResource, mission.GetPosition());
			SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(waypointEntity);
			aiGroup.AddWaypoint(waypoint);
			
			aiGroups.Insert(aiGroup);
		}
		
		return true;
	}
	
	// TODO: consider making it so this method ensures the reward's spawn position is safe
	// If it's not safe, make it find a position within a certain radius. (and update the mission record with the new pos so markers are accurate)
	// If no safe pos can be found, the function returns false
	private bool SpawnReward(WR_Mission mission, out IEntity rewardEntity)
	{
		if (!mission.GetDefinition())
		{
			Print("[WASTELAND] WR_MissionControllerComponent: Tried to spawn mission reward, but no mission definition was found!", LogLevel.ERROR);
			return false;
		}
		
		array<ResourceName> rewardPrefabs = mission.GetDefinition().m_sRewardPrefabChoices;	
		
		if (!rewardPrefabs || rewardPrefabs.Count() == 0)
		{
			// No mission rewards defined, so we can exit successfully
			return true;
		}
		
		// For each reward to be spawned, a random one is selected from the list of reward prefabs
		for (int i = 0; i < mission.GetDefinition().m_iNumberOfRewards; i++)
		{	
			ResourceName rewardPrefab = rewardPrefabs.GetRandomElement();
	
			// Find safe position within 15 m
			vector spawnPos = mission.GetPosition();
			bool safePosFound = WR_Utils.TryGetRandomSafePosWithinRadius(spawnPos, mission.GetPosition(), 1.0, 10.0, 10.0, 2.0); // TODO: read these floats from a config
			if (!safePosFound)
			{
				Print("[WASTELAND] WR_MissionControllerComponent: Unable to find a safe spawn position for this mission's reward!", LogLevel.ERROR);
				return false;
			}	
			
			// Spawn the reward prefab
			rewardEntity = WR_Utils.SpawnPrefabInWorld(rewardPrefab, spawnPos);
			
			// Set random heading
			vector randomDir = WR_Utils.GetRandomHorizontalDirectionAngles();
			rewardEntity.SetYawPitchRoll(randomDir);
			
			// Orient the object to terrain normal
			vector transform[4];
			rewardEntity.GetTransform(transform);
			SCR_TerrainHelper.OrientToTerrain(transform);
			rewardEntity.SetTransform(transform);
	
			// TODO: disabled until we find out how to make sure armed reward vehicles spawn with ammo
//			// Remove initial Items
//			if (!WR_Utils.RemoveAllItemsFromVehicle(rewardEntity))
//			{
//				Print("[WASTELAND] WR_SpawnAreaVehicleSpawnHandlerComponent: Could not remove initial items from vehicle");
//			}
			
			// If loot context is set to NONE, do not fill inventory with loot
			if (mission.GetDefinition().m_eLootContext == WR_LootContextType.NONE)
			{
				return true;
			}
			
			// Fill reward boxes with loot
			int minItems = mission.GetDefinition().m_iMinItemsInBox;
			int maxItems = mission.GetDefinition().m_iMaxItemsInBox;
			
			WR_LootSpawnContext lootContext = WR_LootSpawnContextPresets.GetLootContextByType(mission.GetDefinition().m_eLootContext);
			
			auto inventoryStorageManager = SCR_InventoryStorageManagerComponent.Cast(rewardEntity.FindComponent(SCR_InventoryStorageManagerComponent));
			if (inventoryStorageManager)
			{
				auto inventoryStorage = SCR_UniversalInventoryStorageComponent.Cast(rewardEntity.FindComponent(SCR_UniversalInventoryStorageComponent));
				
				array<ResourceName> items = lootContext.GetRandomItems(Math.RandomIntInclusive(minItems, maxItems), minExtraMags: 5,  maxExtraMags: 12);
				foreach (ResourceName item : items)
				{
					inventoryStorageManager.TrySpawnPrefabToStorage(item, inventoryStorage);
				}
			}
			
		}
		
		return true;
	}
	
	private void DestroyMission(WR_Mission mission)
	{
		// This method cleans up a mission record and ALL items related to it, including its markers and rewards! USE SPARINGLY.
		
		// Delete all entities tied to this mission record.
		array<IEntity> missionEntities = mission.GetMissionEntities();
		foreach (IEntity missionEntity : missionEntities)
			SCR_EntityHelper.DeleteEntityAndChildren(missionEntity);
		
		// Mark the mission location entity as available again
		WR_MissionLocationEntity location = mission.GetMissionLocation();
		if (location)
			location.SetCurrentMission(null);
		
		// Remove mission record from map of active missions.
		m_mActiveMissions.Remove(mission.GetMissionId());
		
		Replication.BumpMe();
	}
	
	void OnPlayerEnteredMissionLocation(WR_Mission mission, WR_MissionLocationEntity location)
	{
		if (m_RplComponent.Role() != RplRole.Authority) return;
		
		if (!GetAreAllNpcsDead(mission)) return;
		
		EndMission(mission, isCompleted: true);
		location.SetCurrentMission(null);
	}
	
	bool GetAreAllNpcsDead(WR_Mission mission)
	{
		array<SCR_AIGroup> aiGroups = mission.GetAiGroups();

		// If array of groups doesn't exist, there are no AI
		if (!aiGroups || aiGroups.Count() == 0) return true;
		
		foreach (SCR_AIGroup group : aiGroups)
			if (group) return false; // AI groups become null when last man dies
		
		
		return true;
	}
	
	void EndMission(WR_Mission mission, bool isCompleted)
	{
		WR_MissionStatus status;
		if (isCompleted)
		{
			status = WR_MissionStatus.Complete;
			Print("[WASTELAND] WR_MissionControllerComponent: Mission complete: " + mission.GetName() + " (ID: " + mission.GetMissionId() + ")");
		}
		else
		{
			status = WR_MissionStatus.Failed;
			Print("[WASTELAND] WR_MissionControllerComponent: Mission failed: " + mission.GetName() + " (ID: " + mission.GetMissionId() + ")");
		}
		
		mission.SetStatus(status);
		
		// TODO: move this logic into a central place that will check the amount of active missions and schedule replacements
		GetGame().GetCallqueue().CallLater(StartRandomMission, m_Config.m_fNewMissionDelay * 60 * 1000, false);
		
		// Send updated mission to proxies
		m_lastUpdatedMission = mission;
		Print("[WASTELAND] m_lastUpdatedMission Status just before bumping: " + mission.GetStatus() + " (ID: " + mission.GetMissionId() + ")");
		Replication.BumpMe();
		
		// Handle update on server's proxy
		HandleUpdatedMissionRecord(mission, false);
		// TODO: HandleUpdatedMissionRecord sets 'mission' parameter to null when the mission is removed from the active mission map
		// I'm not sure why this is happening. Inside the function, the variable still has a value. Only when the function exits back
		// here does mission become null. Need to fix this down the line sometime.
	}
	
	static WR_MissionControllerComponent GetInstance()
	{
		WR_MissionControllerComponent missionController = WR_MissionControllerComponent.Cast(GetGame().GetGameMode().FindComponent(WR_MissionControllerComponent));
		return missionController;
	}
	
	/////////////////////////////
	/* Replication logic below */
	/////////////////////////////
	
	protected void OnNewMissionReceived()
    {
		ReceiveNewMissionOnProxy();
	}
	
	// Called on the authority when an entity gets streamed (connects JIP)
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	override bool RplSave(ScriptBitWriter writer)
	{
		// Write active mission count as header so proxy knows how many values it's receiving
		writer.Write(m_mActiveMissions.Count(), 8);

		foreach (WR_Mission mission : m_mActiveMissions)
		{
			Print("[WASTELAND] RplSave: Mission ID     " + mission.GetMissionId(), LogLevel.DEBUG);
			Print("[WASTELAND] RplSave: Type           " + mission.GetType(), LogLevel.DEBUG);
			Print("[WASTELAND] RplSave: Status         " + mission.GetStatus(), LogLevel.DEBUG);
			Print("[WASTELAND] RplSave: Position       " + mission.GetPosition(), LogLevel.DEBUG);
			Print("\n", LogLevel.DEBUG);
			
			writer.WriteInt(mission.GetMissionId());
			writer.WriteInt(mission.GetType());
			writer.WriteInt(mission.GetStatus());
			writer.WriteVector(mission.GetPosition());
		}

		return true;
	}
	
	// Called on the streamed proxy
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	override bool RplLoad(ScriptBitReader reader)
	{
		Print("[WASTELAND] WR_MissionControllerComponent: RplLoad called on proxy.", LogLevel.WARNING);
		
		int activeMissionCount;
		if (!reader.Read(activeMissionCount, 8)) return false;
		
		for (int i = 0; i < activeMissionCount; i++)
		{
			int id;
			if (!reader.ReadInt(id)) return false;
			
			WR_MissionType type;
			if (!reader.ReadInt(type)) return false;
			
			WR_MissionStatus status;
			if (!reader.ReadInt(status)) return false;
			
			vector pos;
			if (!reader.ReadVector(pos)) return false;
	
			Print("[WASTELAND] RplLoad: Mission ID     " + id, LogLevel.DEBUG);
			Print("[WASTELAND] RplLoad: Type           " + type, LogLevel.DEBUG);
			Print("[WASTELAND] RplLoad: Status         " + status, LogLevel.DEBUG);
			Print("[WASTELAND] RplLoad: Position       " + pos, LogLevel.DEBUG);
			Print("\n", LogLevel.DEBUG);
						
			WR_Mission mission = new WR_Mission();

			mission.SetMissionId(id);
			mission.SetType(type);
			mission.SetStatus(status);
			mission.SetPosition(pos);
			
			HandleUpdatedMissionRecord(mission, true);
		}
		
		foreach (WR_Mission mission : m_mActiveMissions)
		{
			Print("[WASTELAND] RplLoad: ID = " + mission.GetMissionId() + " Name = " + mission.GetName());
		}
		
		return true;
	}
}