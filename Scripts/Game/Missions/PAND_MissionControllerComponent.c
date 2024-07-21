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
   	// TODO: Of course, we will also need to provide means for clients to get the full list of missions when they connect to the game.
	
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

		
		// Proxies should request a copy of the authority's map of active missions, then handle them locally
//		if (m_RplComponent.Role() == RplRole.Proxy)
//		{	
//			Rpc(RpcAsk_Authority_Method, true);
//		}

		
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
	
	// Called on the authority when an entity gets streamed
	override bool RplSave(ScriptBitWriter writer)
	{
		// Write active mission count as header so proxy knows how many values it's receiving
		writer.WriteInt(m_mActiveMissions.Count());

		foreach (PAND_Mission mission : m_mActiveMissions)
		{
			Print("[WASTELAND] RplSave: " + mission.GetMissionId());
			Print("[WASTELAND] RplSave: " + mission.GetType());
			Print("[WASTELAND] RplSave: " + mission.GetStatus());
			Print("[WASTELAND] RplSave: " + mission.GetPosition());
			Print("[WASTELAND] RplSave: " + mission.GetCreatedAtTime());
			Print("[WASTELAND] RplSave: " + mission.GetIsSuccessful());
			
			writer.WriteInt(mission.GetMissionId());
			writer.WriteInt(mission.GetType());
			writer.WriteInt(mission.GetStatus());
			writer.WriteVector(mission.GetPosition());
			writer.Write(mission.GetCreatedAtTime(), 64);
			writer.WriteBool(mission.GetIsSuccessful());
		}

		return true;
	}
	
	// Called on the streamed proxy
	override bool RplLoad(ScriptBitReader reader)
	{
		Print("[WASTELAND] PAND_MissionControllerComponent: RplLoad called on proxy.", LogLevel.WARNING);
		
		int activeMissionCount;
		if (!reader.ReadInt(activeMissionCount)) return false;
		
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
			
			WorldTimestamp createdAtTime;
			if (!reader.Read(createdAtTime, 64)) return false;
			
			bool isSuccessful;
			if (!reader.ReadBool(isSuccessful)) return false;

			Print("[WASTELAND] RplLoad: " + id);
			Print("[WASTELAND] RplLoad: " + type);
			Print("[WASTELAND] RplLoad: " + status);
			Print("[WASTELAND] RplLoad: " + pos);
			Print("[WASTELAND] RplLoad: " + createdAtTime);
			Print("[WASTELAND] RplLoad: " + isSuccessful);
						
			PAND_Mission mission = new PAND_Mission();

			mission.SetMissionId(id);
			mission.SetType(type);
			mission.SetStatus(status);
			mission.SetPosition(pos);
			mission.SetCreatedAtTime(createdAtTime);
			mission.SetIsSuccessful(isSuccessful);
			
			HandleUpdatedMissionRecord(mission, true);
		}
		
		foreach (PAND_Mission mission : m_mActiveMissions)
		{
			Print("[WASTELAND] RplLoad: ID = " + mission.GetMissionId() + " Name = " + mission.GetName());
		}
		
		return true;
	}
	
	override void OnGameModeStart()
	{
		if (m_RplComponent.Role() == RplRole.Authority)
		{
			// TODO: upgrade this to start multiple missions, track existing missions, give "mission about to start hint", etc.
			// GetGame().GetCallqueue().CallLater(StartRandomMission, 5*1000, false);
			
			float initialDelayMs = m_Config.m_fInitialMissionDelay * 60 * 1000;
			float delayBetweenMissionStartMs = 0 * 1000;
			
			for (int i = 0; i < m_Config.m_iMaxActiveMissions; i++)
			{
				//GetGame().GetCallqueue().CallLater(StartRandomMission, initialDelayMs + i * delayBetweenMissionStartMs, false);
				GetGame().GetCallqueue().CallLater(StartRandomMission, 0 + i * delayBetweenMissionStartMs, false);
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

	protected void OnNewMissionReceived()
    {
		ReceiveNewMissionOnProxy();
	}
	
    protected void ReceiveNewMissionOnProxy()
    {
        if (m_RplComponent.Role() != RplRole.Proxy) return;
		if (!m_mActiveMissions) return;
		if (m_lastUpdatedMission.IsEmptyMission()) return;
		
		Print("[WASTELAND] PAND_MissionManagerComponent: New mission received from authority (ID: " + m_lastUpdatedMission.GetMissionId() + ")", LogLevel.NORMAL);
		
		HandleUpdatedMissionRecord(m_lastUpdatedMission, false);
    }
	
	protected void HandleUpdatedMissionRecord(PAND_Mission newMission, bool isJipRplLoad)
	{
		ref PAND_Mission mission;
		bool missionExists = m_mActiveMissions.Find(newMission.GetMissionId(), mission);
		
		// Populate mission definition from local config file
		PAND_MissionDefinition definition = GetMissionDefinition(newMission.GetType());
		newMission.SetDefinition(definition); 
		
		if (!missionExists)
		{
			// We've never received this mission ID, so initialize a new mission.
			mission = newMission;
			
			m_mActiveMissions.Insert(mission.GetMissionId(), mission);

			PAND_MissionUiElementHelper.CreateMarker(mission);
			
			// Only active player should see notifications, not JIP clients who are receiving initial mission info
			if (!isJipRplLoad)
				PAND_MissionUiElementHelper.SendMissionNotificationByStatus(mission);
		}
		else
		{
			// We have received a mission with this ID before, so update or delete existing mission record as required
			PAND_MissionStatus currStatus = mission.GetStatus();
			switch	(currStatus)
			{
				case PAND_MissionStatus.Complete:
				case PAND_MissionStatus.Failed:
				{
					SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
					
					PAND_MissionUiElementHelper.DeleteMarker(mission);
					PAND_MissionUiElementHelper.SendMissionNotificationByStatus(mission);
		
					m_mActiveMissions.Remove(mission.GetMissionId());
					
					break;
				}
			}
		}
	}
	
	private bool InstantiateMissionWorldObjects(PAND_Mission mission)
	{
		// I don't think we need to handle replication for these entities. I think the game will do it automatically when players get close to the mission. Not sure though. Will require research/testing)
		
		// Spawn rewards
		array<IEntity> rewardEntities;
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

    protected map<int, ref PAND_Mission> GetActiveMissions()
    {
        // Used when a player connects JIP or reconnects after lagging/desyncing in order to make sure they have an up-to-date list of active missions.
        
		// Convert m_mActiveMissions to map<int, ref PAND_MissionDto>
		if (!m_mActiveMissions) return null;
			
		return m_mActiveMissions;
		//Placeholder. It's certainly not as simple as just returning the list. Need to find out how we can transfer a specific variable on-demand from authority to proxy.
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
	private bool SpawnReward(PAND_Mission mission, out array<IEntity> rewardEntities)
	{
		if (!mission.GetDefinition())
		{
			Print("[WASTELAND] PAND_MissionControllerComponent: Tried to spawn mission reward, but no mission definition was found!", LogLevel.ERROR);
			return false;
		}
		
		array<ResourceName> rewardPrefabs = mission.GetDefinition().m_sRewardPrefabs;
		
		if (!rewardPrefabs || rewardPrefabs.Count() == 0)
		{
			Print("[WASTELAND] PAND_MissionControllerComponent: Tried to spawn mission reward, but no rewards were defined!", LogLevel.ERROR);
			return false;
		}
		
		rewardEntities = {};
		for (int i = 0; i < rewardPrefabs.Count(); i++)
		{
			vector spawnPos = mission.GetPosition();
			
			// Spawn all subsequent rewards beyond the first one at random nearby positions
			if (i > 0)
			{
				
				bool safePosFound = WR_Utils.TryGetRandomSafePosWithinRadius(spawnPos, mission.GetPosition(), 15.0, 10.0, 10.0, 2.0); // TODO: read these floats from a config
				if (!safePosFound)
				{
					Print("[WASTELAND] PAND_MissionControllerComponent: Unable to find a safe spawn position for this mission's reward!", LogLevel.ERROR);
					return false;
				}	
			}
			
			// Spawn the reward prefab
			IEntity rewardEntity = WR_Utils.SpawnPrefabInWorld(rewardPrefabs[i], spawnPos);
			rewardEntity.SetYawPitchRoll(WR_Utils.GetRandomHorizontalDirectionAngles());
			
			// Fill reward boxes with loot
			// TODO: make this dynamic with loot contexts and configs
			int minItems = 6;
			int maxItems = 10;
			WR_LootSpawnContext lootContext = WR_LootSpawnContextPresets.GetHeavyWeaponBoxContext();
			
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
			
			rewardEntities.Insert(rewardEntity);
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
		
		// Handle update on server's proxy
		HandleUpdatedMissionRecord(mission, false);
		
		// Send updated mission to proxies
		Replication.BumpMe();
	}
	
//	void HandleCompleteMission()
//	{
//		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
//		
//		PAND_MissionUiElementHelper.DeleteMarker(mission);
//		PAND_MissionUiElementHelper.SendMissionNotificationByStatus(mission);
//		
//		m_mActiveMissions.Remove(mission.GetMissionId());
//	}
	
	static PAND_MissionControllerComponent GetInstance()
	{
		PAND_MissionControllerComponent missionController = PAND_MissionControllerComponent.Cast(GetGame().GetGameMode().FindComponent(PAND_MissionControllerComponent));
		return missionController;
	}
	
//	private string GetMissionName(PAND_Mission mission)
//	{
//		PAND_MissionDefinition definition = GetMissionDefinition(mission);
//		
//		if (!definition)
//		{
//			Print("[WASTELAND] PAND_MissionControllerComponent: Unable to get mission name!", LogLevel.ERROR);
//			return "";
//		}
//		
//		return definition.m_sName;
//	}
//	
//	private string GetMissionDescription(PAND_Mission mission)
//	{
//		PAND_MissionDefinition definition = GetMissionDefinition(mission);
//		
//		if (!definition)
//		{
//			Print("[WASTELAND] PAND_MissionControllerComponent: Unable to get mission description!", LogLevel.ERROR);
//			return "";
//		}
//		
//		return definition.m_sDescription;
//	}
}