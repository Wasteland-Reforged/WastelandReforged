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
	
	[RplProp(onRplName: "OnNewMissionReceived")]
	protected ref PAND_Mission m_lastUpdatedMission = new PAND_Mission();
	
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

		Print("[WASTELAND] PAND_MissionManagerComponent: Initialized.", LogLevel.NORMAL);
	}
	
	override void OnGameModeStart()
	{
		if (m_RplComponent.Role() == RplRole.Authority)
		{
			// TODO: upgrade this to start multiple missions, track existing missions, give "mission about to start hint", etc.
			// GetGame().GetCallqueue().CallLater(StartRandomMission, 5*1000, false);
			
			float initialDelayMs = m_Config.m_fInitialMissionDelay * 60 * 1000;
			float delayBetweenMissionStartMs = 5000;
			
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

        // Create mission record
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
		
//		bool isMissionRecordCreated = HandleNewMissionRecord(m_lastUpdatedMission);
//		if (!areObjectsCreated)
//		{
//			Print("[WASTELAND] PAND_MissionControllerComponent: Failed to create mission record! (ID: " + missionId + ") Aborting mission creation.", LogLevel.ERROR);
//			DestroyMission(m_lastUpdatedMission);
//		}
		HandleNewMissionRecord(m_lastUpdatedMission);
		
		// Update clients with mission record
		Replication.BumpMe();
		
		Print("[WASTELAND] PAND_MissionManagerComponent: New mission started: " + GetMissionName(m_lastUpdatedMission) + " (ID: " + missionId + ")", LogLevel.NORMAL);
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
		HandleNewMissionRecord(m_lastUpdatedMission);
    }
	
	protected void HandleNewMissionRecord(PAND_Mission newMission)
	{
		// We've never received this mission ID, so initialize a new mission.
		ref PAND_Mission mission;
		bool missionExists = m_mActiveMissions.Find(newMission.GetMissionId(), mission);
		
		if (!missionExists)
		{
			mission = newMission;
			
//			if (m_RplComponent.Role() == RplRole.Authority)
//			{
//				// Spawn mission world objects on authority only
//				bool missionObjectsSpawnedSuccessfully = InstantiateMissionWorldObjects(mission);
//				if (!missionObjectsSpawnedSuccessfully)
//				{
//					Print("[WASTELAND] WR_MissionManagerComponent: Unable to spawn mission objects! (ID: " + mission.GetMissionId() + ") Aborting mission creation.", LogLevel.ERROR); 
//					return;
//				}
//			}
			
			m_mActiveMissions.Insert(mission.GetMissionId(), mission);

			CreateMarker(mission);
			ShowHintMissionStarted(mission);
			PlaySound();
		}
		else
		{
			// We have received a mission with this ID before, so update or delete existing mission record as required.
			// TODO: implement all this
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
	
	// TODO: maybe change this to be a create or update method?
	private void CreateMarker(PAND_Mission mission)
	{
		ref SCR_MapMarkerBase marker = new SCR_MapMarkerBase();
		
		marker.SetType(SCR_EMapMarkerType.PLACED_CUSTOM);
		marker.SetCustomText(GetMissionName(mission));
		marker.SetIconEntry(WR_MapMarkerConfigQuadIndices.TargetReferencePoint2);
		marker.SetColorEntry(WR_MapMarkerConfigColorIndices.Red);
		marker.SetWorldPos(mission.GetPosition()[0], mission.GetPosition()[2]);
		
		// Save marker record to mission object so we can update or delete it from the map later
		mission.SetMarker(marker);
		
		// Draw marker on map
		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
		if (!markerManager)
		{
			Print("[WASTELAND] WR_MissionManagerComponent: Marker manager is null.", LogLevel.ERROR);
			return;
		}

		markerManager.InsertStaticMarker(marker, isLocal: false, isServerMarker: true);
	}
	
	private void ShowHintMissionStarted(PAND_Mission mission)
	{
		ShowHint("New Objective: " + GetMissionName(mission), GetMissionDescription(mission));
	}
	
	private void ShowHintMissionCompleted(PAND_Mission mission)
	{
		ShowHint("Objective Complete: " + GetMissionName(mission), "The mission has been completed!");
	}
	
	private void ShowHint(string title, string description)
	{
		SCR_HintUIInfo hintInfo = SCR_HintUIInfo.CreateInfo(
			description,
			title,
			10, // TODO: read from config
			EHint.UNDEFINED,
			EFieldManualEntryId.NONE,
			true // TODO: read from config
		);
		
		SCR_HintManagerComponent.ShowHint(hintInfo, isSilent: true, ignoreShown: true);
	}
	
	private void PlaySound()
	{
		SCR_UISoundEntity uiSound = SCR_UISoundEntity.GetInstance();
		//uiSound.SetSignalValueStr("", 1.00); // TODO: figure out how to set volume properly
		uiSound.SoundEvent("SOUND_HUD_TASK_CREATED", force: true);
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

	string GetMissionName(PAND_Mission mission)
	{
		// TODO: maybe we can improve this at some point by storing mission defs in a map. Should be fine for now with how small the data set is, though
		foreach (PAND_MissionDefinition definition : m_Config.m_aMissionDefinitions)
			if (mission.GetType() == definition.m_eType) return definition.m_sName;
		
		Print("[WASTELAND] PAND_MissionControllerComponent: Unable to get mission name!", LogLevel.ERROR);
		return "";
	}
	
	string GetMissionDescription(PAND_Mission mission)
	{
		// TODO: maybe we can improve this at some point by storing mission defs in a map. Should be fine for now with how small the data set is, though
		foreach (PAND_MissionDefinition definition : m_Config.m_aMissionDefinitions)
			if (mission.GetType() == definition.m_eType) return definition.m_sDescription;
		
		Print("[WASTELAND] PAND_MissionControllerComponent: Unable to get mission description!", LogLevel.ERROR);
		return "";
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
		
		Print("[WASTELAND] PAND_MissionControllerComponent: Player entered mission zone.");
		
		if (!GetAreAllNpcsDead(mission)) return;
		
		CompleteMission(mission);
		location.SetCurrentMission(null);
	}
	
	bool GetAreAllNpcsDead(PAND_Mission mission)
	{
		array<SCR_AIGroup> aiGroups = mission.GetAiGroups();

		foreach (SCR_AIGroup group : aiGroups)
			if (group) return false; // AI groups become null when last man dies
		
		return true;
	}
	
	void CompleteMission(PAND_Mission mission)
	{
		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
		
		markerManager.RemoveStaticMarker(mission.GetMarker());
		ShowHintMissionCompleted(mission);
		
		m_mActiveMissions.Remove(mission.GetMissionId());
		
		// TODO: move this logic into a central place that will check the amount of active missions and schedule replacements
		GetGame().GetCallqueue().CallLater(StartRandomMission, m_Config.m_fNewMissionDelay * 60 * 1000, false);
	}
}