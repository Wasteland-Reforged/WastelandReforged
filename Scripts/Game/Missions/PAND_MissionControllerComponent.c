class PAND_MissionControllerComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class PAND_MissionControllerComponent : SCR_BaseGameModeComponent
{
	[Attribute("", UIWidgets.Object, "Mission controller config")]
	protected ref PAND_MissionControllerConfig m_Config;
    
	protected SCR_BaseGameMode m_Parent;
	protected BaseRplComponent m_RplComponent;
	
    protected int m_iMissionCounter;
    
	protected ref map<int, ref PAND_Mission> m_mActiveMissions = new map<int, ref PAND_Mission>();
	
	protected ref map<PAND_MissionType, PAND_MissionDefinition> m_mMissionDefinitions = new map<PAND_MissionType, PAND_MissionDefinition>();
	
	[RplProp(onRplName: "OnNewMissionReceived")]
	protected ref PAND_Mission m_lastUpdatedMission = new PAND_Mission();
	
	protected ref PAND_MissionUiElementHelper uiHelper;

	private bool m_bIsInitialRplLoadDone = false;
	
	override void OnPostInit(IEntity owner)
	{
		m_Parent = SCR_BaseGameMode.Cast(owner);
		if (!m_Parent)
		{
			Print("[WASTELAND] PAND_MissionManagerComponent Parent entity must be SCR_BaseGameMode!", LogLevel.ERROR);
			return;
		}
		
		m_RplComponent = BaseRplComponent.Cast(m_Parent.FindComponent(BaseRplComponent));
		if (!m_RplComponent)
		{
			Print("[WASTELAND] PAND_MissionManagerComponent: Parent entity must have RplComponent!", LogLevel.ERROR);
			return;
		}
		
		if (!m_Config)
		{
			Print("[WASTELAND] PAND_MissionManagerComponent: Mission controller must have a valid config!", LogLevel.ERROR);
			return;
		}
		
		foreach (PAND_MissionDefinition definition : m_Config.m_aMissionDefinitions)
			m_mMissionDefinitions.Insert(definition.m_eType, definition);

		Print("[WASTELAND] PAND_MissionControllerComponent: Mission controller initialized.", LogLevel.NORMAL);
	}
	
	PAND_MissionDefinition GetMissionDefinition(PAND_MissionType type)
	{
		PAND_MissionDefinition definition;
		if (m_mMissionDefinitions.Find(type, definition))
			return definition;
		
		Print("[WASTELAND] PAND_MissionControllerComponent: Mission definition not found for mission type " + type + "!", LogLevel.ERROR);
		return null;
	}
	
	override void OnGameModeStart()
	{
		if (m_RplComponent.Role() == RplRole.Authority)
		{
			float initialDelayMs = m_Config.m_fInitialMissionDelay * 60 * 1000;
			float delayBetweenMissionStartMs = 20 * 1000;
			
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
			Print("[WASTELAND] PAND_MissionManagerComponent: Not enough capacity to start a new mission! Aborting mission creation.", LogLevel.ERROR);
			return;
		}

		int missionId = GetNewMissionId();
		PAND_MissionType missionType = GetRandomMissionType();
		PAND_MissionLocationEntity missionLocation = GetRandomVacantMissionLocation();
        
		if (!missionLocation)
		{
			Print("[WASTELAND] PAND_MissionManagerComponent: Unable to get location for new mission! (ID: " + missionId + ") Aborting mission creation.", LogLevel.ERROR);
			return;
		}
		
		// Create mission record
		m_lastUpdatedMission = PAND_Mission.CreateMission(missionId, missionType, missionLocation.GetOrigin());
		missionLocation.SetCurrentMission(m_lastUpdatedMission);
		
		// Find mission definition corresponding to selected mission type
		PAND_MissionDefinition missionDefinition = FindMissionDefinitionByMissionType(m_lastUpdatedMission.GetType());
		if (!missionDefinition)
		{
			Print("[WASTELAND] PAND_MissionControllerComponent: No definition found for provided type! (ID: " + missionId + ") Aborting mission creation.", LogLevel.ERROR);
		}
		m_lastUpdatedMission.SetDefinition(missionDefinition);
		
		// Run mission on server (for in-game hosted servers)
		bool areObjectsCreated = InstantiateMissionWorldObjects(m_lastUpdatedMission);
		if (!areObjectsCreated)
		{
			Print("[WASTELAND] PAND_MissionControllerComponent: Failed to instantiate mission world objects! (ID: " + missionId + ") Aborting mission creation.", LogLevel.ERROR);
			DestroyMission(m_lastUpdatedMission);
		}
		
		HandleUpdatedMissionRecord(m_lastUpdatedMission, false);
		
		// Send updated mission record to proxies
		Replication.BumpMe();
		
		Print("[WASTELAND] PAND_MissionManagerComponent: New mission started: " + m_lastUpdatedMission.GetName() + " (ID: " + missionId + ")", LogLevel.NORMAL);
    }
	
    protected void ReceiveNewMissionOnProxy()
    {
        if (m_RplComponent.Role() != RplRole.Proxy) return;
		if (!m_mActiveMissions) return;
		if (m_lastUpdatedMission.IsEmptyMission()) return;
		
		Print("[WASTELAND] PAND_MissionManagerComponent: New mission received from authority (ID: " + m_lastUpdatedMission.GetMissionId() + ")", LogLevel.NORMAL);
		
		HandleUpdatedMissionRecord(m_lastUpdatedMission, false);
    }
	
	protected void HandleUpdatedMissionRecord(PAND_Mission mission, bool isJipRplLoad)
	{
		PAND_Mission newMission = mission;
		//bool missionExists = m_mActiveMissions.Find(newMission.GetMissionId(), mission);
		bool missionExists = m_mActiveMissions.Contains(newMission.GetMissionId());
		
		// Populate mission definition from local config file
		PAND_MissionDefinition definition = GetMissionDefinition(newMission.GetType());
		newMission.SetDefinition(definition); 
		
		//mission = newMission;
		if (!missionExists)
		{
			// We've never received this mission ID, so add it to the list of active missions.
			m_mActiveMissions.Set(newMission.GetMissionId(), newMission);

			// Markers should only be created on the authority, since Reforger already includes infrastructure to replicate them
			if (m_RplComponent.Role() == RplRole.Authority)
			{
				PAND_MissionUiElementHelper.CreateMarker(newMission);	
			}
			
			// Only active player should see notifications, not JIP clients who are receiving initial mission info
			if (!isJipRplLoad)
				PAND_MissionUiElementHelper.SendMissionNotificationByStatus(newMission);
		}
		else
		{
			// We have received a mission with this ID before, so update or delete existing mission record as required
			PAND_MissionStatus currStatus = newMission.GetStatus();
			switch	(currStatus)
			{
				case PAND_MissionStatus.Complete:
				case PAND_MissionStatus.Failed:
				{
					SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
					
					if (m_RplComponent.Role() == RplRole.Authority)
					{
						PAND_MissionUiElementHelper.DeleteMarker(newMission);
					}
					
					// Only active players should see notifications, not JIP clients who are receiving initial mission info
					if (!isJipRplLoad)
						PAND_MissionUiElementHelper.SendMissionNotificationByStatus(newMission);
		
					m_mActiveMissions.Take(newMission.GetMissionId(), newMission);
					
					break;
				}
			}
		}
		
		Print("[WASTELAND] PAND_MissionManagerComponent: ID = " + newMission.GetMissionId() + " Status = " + newMission.GetStatus());
	}
	
	private bool InstantiateMissionWorldObjects(PAND_Mission mission)
	{
		// I don't think we need to handle replication for these entities. I think the game will do it automatically when players get close to the mission. Not sure though. Will require research/testing)
		
		// Spawn rewards
		IEntity rewardEntities;
		bool isRewardSpawned = SpawnReward(mission, rewardEntities);
		
		// Spawn props
		IEntity propEntity;
		bool isPropSpawned = SpawnProp(mission, propEntity);

		// Spawn NPCs
		ref array<SCR_AIGroup> aiGroups;
		bool areAiGroupsSpawned = SpawnAiGroups(mission, aiGroups);

		if (!isRewardSpawned || !isPropSpawned || !areAiGroupsSpawned)
		{
			Print("[WASTELAND] PAND_MissionControllerComponent: Failed to spawn mission objects in the world! (mission ID: " + mission.GetMissionId() + ")");
			return false;
		}
		
		mission.SetMissionEntities(rewardEntities, propEntity, aiGroups);

		return true;
	}

    private int GetNewMissionId()
    {
        m_iMissionCounter += 1;
        return m_iMissionCounter;
    }
	
    PAND_MissionType GetRandomMissionType()
    {
		PAND_MissionDefinition definition = m_Config.m_aMissionDefinitions.GetRandomElement();
        return definition.m_eType;
    }
			
	protected PAND_MissionLocationEntity GetRandomVacantMissionLocation()
	{	
		array<PAND_MissionLocationEntity> vacantLocations = PAND_MissionLocationEntity.GetAllVacantLocations();
		if (vacantLocations.Count() == 0)
		{
			Print("[WASTELAND] WR_MissionManagerComponent: No vacant mission locations!", LogLevel.ERROR);
			return null;
		}
		
		PAND_MissionLocationEntity randomLocation = vacantLocations.GetRandomElement();
		// TODO: check if there are any entities inside/nearby this mission location's trigger zone. We don't want to spawn missions on people's heads

		return randomLocation;
	}
	
	private PAND_MissionDefinition FindMissionDefinitionByMissionType(PAND_MissionType type)
	{
		if (!m_Config) return null; // TODO: add error logging
		if (!m_Config.m_aMissionDefinitions) return null; // TODO: add error logging
		
		foreach (PAND_MissionDefinition definition : m_Config.m_aMissionDefinitions)
			if (definition.m_eType == type) return definition;
		
		return null;
	}
	
	private bool SpawnProp(PAND_Mission mission, out IEntity propEntity)
	{
		if (!mission.GetDefinition())
		{
			Print("[WASTELAND] PAND_MissionControllerComponent: Tried to spawn mission prop, but no mission definition was found!", LogLevel.ERROR);
			return false;
		}
		
		ResourceName propResource = mission.GetDefinition().m_sPropPrefab;
		if (!propResource) return true; // Props are not required, so exit successfully

		propEntity = WR_Utils.SpawnPrefabInWorld(propResource, mission.GetPosition());
		propEntity.SetYawPitchRoll(WR_Utils.GetRandomHorizontalDirectionAngles());
		
		return true;
	}
	
	private bool SpawnAiGroups(PAND_Mission mission, out array<SCR_AIGroup> aiGroups)
	{
		if (!mission.GetDefinition())
		{
			Print("[WASTELAND] PAND_MissionControllerComponent: Tried to spawn mission AI groups, but no mission definition was found!", LogLevel.ERROR);
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
			
			// The return value of TryGetRandomSafePosWithinRadius is kinda broken, so disabling the safePosFound check for now. Fix that method!!!
			//bool safePosFound =
//			if (!safePosFound)
//			{
//				Print("[WASTELAND] PAND_MissionControllerComponent: A safe spawn position was unable to be found for this mission's AI!", LogLevel.ERROR);
//				return false;
//			}
			
			// Spawn the group prefab
			IEntity aiGroupEntity = WR_Utils.SpawnPrefabInWorld(aiGroupResource, spawnPos);
			SCR_AIGroup aiGroupInstance = SCR_AIGroup.Cast(aiGroupEntity);
			
			// All NPC fighters should be on civilian faction
			aiGroupInstance.SetFaction(GetGame().GetFactionManager().GetFactionByKey("CIV"));	
	
			// Command the group to defend the mission location
			ResourceName waypointResource = "{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Defend.et";
			IEntity waypointEntity = WR_Utils.SpawnPrefabInWorld(waypointResource, mission.GetPosition());
			SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(waypointEntity);
			aiGroupInstance.AddWaypoint(waypoint);
			
			aiGroups.Insert(aiGroupInstance);
		}
		
		return true;
	}
	
	// TODO: consider making it so this method ensures the reward's spawn position is safe
	// If it's not safe, make it find a position within a certain radius. (and update the mission record with the new pos so markers are accurate)
	// If no safe pos can be found, the function returns false
	private bool SpawnReward(PAND_Mission mission, out IEntity rewardEntity)
	{
		if (!mission.GetDefinition())
		{
			Print("[WASTELAND] PAND_MissionControllerComponent: Tried to spawn mission reward, but no mission definition was found!", LogLevel.ERROR);
			return false;
		}
		
		array<ResourceName> rewardPrefabs = mission.GetDefinition().m_sRewardPrefabChoices;	
		if (!rewardPrefabs || rewardPrefabs.Count() == 0)
		{
			Print("[WASTELAND] PAND_MissionControllerComponent: Tried to spawn mission reward, but no rewards were defined!", LogLevel.ERROR);
			return false;
		}

		//For each reward to be spawned, a random one is selected from the list of reward prefabs
		for (int i = 0; i < mission.GetDefinition().m_iNumberOfRewards; i++) {
			
			ResourceName rewardPrefab = rewardPrefabs.GetRandomElement();
	
			//Find safe position within 15m
			vector spawnPos = mission.GetPosition();
			bool safePosFound = WR_Utils.TryGetRandomSafePosWithinRadius(spawnPos, mission.GetPosition(), 15.0, 10.0, 10.0, 2.0); // TODO: read these floats from a config
			if (!safePosFound)
			{
				Print("[WASTELAND] PAND_MissionControllerComponent: Unable to find a safe spawn position for this mission's reward!", LogLevel.ERROR);
				return false;
			}	
			
			// Spawn the reward prefab
			rewardEntity = WR_Utils.SpawnPrefabInWorld(rewardPrefab, spawnPos);
			rewardEntity.SetYawPitchRoll(WR_Utils.GetRandomHorizontalDirectionAngles());
			
			// If loot context is set to NONE, do not fill inventory with loot
			if (mission.GetDefinition().m_eLootContext == PAND_MissionLootContext.NONE)
			{
				return true;
			}
			
			// Fill reward boxes with loot
			int minItems = mission.GetDefinition().m_iMinItemsInBox;
			int maxItems = mission.GetDefinition().m_iMaxItemsInBox;
			
			WR_LootSpawnContext lootContext;
			switch(mission.GetDefinition().m_eLootContext)
			{
				case PAND_MissionLootContext.WEAPONS:
					lootContext = WR_LootSpawnContextPresets.GetWeaponBoxContext();
					break;
				case PAND_MissionLootContext.HEAVYWEAPONS:
					lootContext = WR_LootSpawnContextPresets.GetHeavyWeaponBoxContext();
					break;
				case PAND_MissionLootContext.MEDICAL:
					lootContext = WR_LootSpawnContextPresets.GetMedicalBoxContext();
					break;
				default:
					lootContext = WR_LootSpawnContextPresets.GetLootBoxContext();
			}
			
			auto inventoryStorageManager = SCR_InventoryStorageManagerComponent.Cast(rewardEntity.FindComponent(SCR_InventoryStorageManagerComponent));
			if (inventoryStorageManager)
			{
				auto inventoryStorage = SCR_UniversalInventoryStorageComponent.Cast(rewardEntity.FindComponent(SCR_UniversalInventoryStorageComponent));
				
				array<ResourceName> items = lootContext.GetRandomItems(Math.RandomIntInclusive(minItems, maxItems), maxExtraMagsToIncludeForWeapons: 10);
				foreach (ResourceName item : items)
				{
					inventoryStorageManager.TrySpawnPrefabToStorage(item, inventoryStorage);
				}
			}
			
		}
		
		return true;
	}
	
	private void DestroyMission(PAND_Mission mission)
	{
		// This method cleans up a mission record and ALL items related to it, including its markers and rewards!
		
		// TODO: make sure to remove the reference to this mission on the mission location entity
	}
	
	void OnPlayerEnteredMissionZone(PAND_Mission mission, PAND_MissionLocationEntity location)
	{
		if (m_RplComponent.Role() != RplRole.Authority) return;
		
		if (!GetAreAllNpcsDead(mission)) return;
		
		EndMission(mission, isCompleted: true);
		location.SetCurrentMission(null);
	}
	
	bool GetAreAllNpcsDead(PAND_Mission mission)
	{
		array<SCR_AIGroup> aiGroups = mission.GetAiGroups();

		foreach (SCR_AIGroup group : aiGroups)
			if (group) return false; // AI groups become null when last man dies
		
		return true;
	}
	
	void EndMission(PAND_Mission mission, bool isCompleted)
	{
		PAND_MissionStatus status;
		if (isCompleted)
		{
			status = PAND_MissionStatus.Complete;
			Print("[WASTELAND] PAND_MissionControllerComponent: Mission complete: " + mission.GetName() + " (ID: " + mission.GetMissionId() + ")");
		}
		else
		{
			status = PAND_MissionStatus.Failed;
			Print("[WASTELAND] PAND_MissionControllerComponent: Mission failed: " + mission.GetName() + " (ID: " + mission.GetMissionId() + ")");
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
	
	static PAND_MissionControllerComponent GetInstance()
	{
		PAND_MissionControllerComponent missionController = PAND_MissionControllerComponent.Cast(GetGame().GetGameMode().FindComponent(PAND_MissionControllerComponent));
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

		foreach (PAND_Mission mission : m_mActiveMissions)
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
		Print("[WASTELAND] PAND_MissionControllerComponent: RplLoad called on proxy.", LogLevel.WARNING);
		
		int activeMissionCount;
		if (!reader.Read(activeMissionCount, 8)) return false;
		
		for (int i = 0; i < activeMissionCount; i++)
		{
			int id;
			if (!reader.ReadInt(id)) return false;
			
			PAND_MissionType type;
			if (!reader.ReadInt(type)) return false;
			
			PAND_MissionStatus status;
			if (!reader.ReadInt(status)) return false;
			
			vector pos;
			if (!reader.ReadVector(pos)) return false;
	
			Print("[WASTELAND] RplLoad: Mission ID     " + id, LogLevel.DEBUG);
			Print("[WASTELAND] RplLoad: Type           " + type, LogLevel.DEBUG);
			Print("[WASTELAND] RplLoad: Status         " + status, LogLevel.DEBUG);
			Print("[WASTELAND] RplLoad: Position       " + pos, LogLevel.DEBUG);
			Print("\n", LogLevel.DEBUG);
						
			PAND_Mission mission = new PAND_Mission();

			mission.SetMissionId(id);
			mission.SetType(type);
			mission.SetStatus(status);
			mission.SetPosition(pos);
			
			HandleUpdatedMissionRecord(mission, true);
		}
		
		foreach (PAND_Mission mission : m_mActiveMissions)
		{
			Print("[WASTELAND] RplLoad: ID = " + mission.GetMissionId() + " Name = " + mission.GetName());
		}
		
		return true;
	}
}