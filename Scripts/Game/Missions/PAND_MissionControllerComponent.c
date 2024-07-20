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

		
		Print("[WASTELAND] PAND_MissionManagerComponent: Initialized", LogLevel.NORMAL);
	}
	
	override void OnGameModeStart()
	{
		if (m_RplComponent.Role() == RplRole.Authority)
		{
			// TODO: upgrade this to start multiple missions, track existing missions, give "mission about to start hint", etc.
			// GetGame().GetCallqueue().CallLater(StartRandomMission, 5*1000, false);
			for (int i = 0; i < m_Config.m_iMaxActiveMissions; i++) {
				GetGame().GetCallqueue().CallLater(StartRandomMission, i*3000, false);
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
		vector missionLocation = GetRandomVacantMissionLocation();
        
		if (missionLocation == vector.Zero)
		{
			Print("[WASTELAND] PAND_MissionManagerComponent: Unable to get location for new mission! Aborting mission creation.", LogLevel.ERROR);
			return;
		}
		
		m_lastUpdatedMission = PAND_Mission.CreateMission(missionId, missionType, missionLocation);
		
		// Run mission on server (for in-game hosted servers)
		InstantiateMissionWorldObjects(m_lastUpdatedMission);
		HandleNewMissionRecord(m_lastUpdatedMission);
		
		// Update clients
		Replication.BumpMe();
		
		Print("[WASTELAND] PAND_MissionManagerComponent: New mission started (ID: " + missionId + ")", LogLevel.NORMAL);
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
			
			PAND_MissionDefinition missionDefinition = FindMissionDefinitionByMissionType(mission.GetType());
			if (!missionDefinition)
			{
				// TODO: improve error logging
				Print("No definition found for provided type!", LogLevel.ERROR);
			}
			
			mission.SetDefinition(missionDefinition);
			
			if (m_RplComponent.Role() == RplRole.Authority)
			{
				// Spawn mission world objects on authority only
				bool missionObjectsSpawnedSuccessfully = InstantiateMissionWorldObjects(mission);
				if (!missionObjectsSpawnedSuccessfully)
				{
					Print("[WASTELAND] WR_MissionManagerComponent: Unable to spawn mission objects! (ID: " + mission.GetMissionId() + ") Aborting mission creation.", LogLevel.ERROR); 
					return;
				}
			}
			
			m_mActiveMissions.Insert(mission.GetMissionId(), mission);

			CreateMarker(mission);
			CreateHint(mission);
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
			// TODO: improve error logging
			Print("Failed to spawn all mission objects in the world!");
			DestroyMission(mission);
			return false;
		}

		return true;
	}
	
	// TODO: maybe change this to be a create or update method?
	private void CreateMarker(PAND_Mission mission)
	{
		SCR_MapMarkerBase marker = new SCR_MapMarkerBase();
		
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
	
	private void CreateHint(PAND_Mission mission)
	{
		SCR_HintUIInfo hintInfo = SCR_HintUIInfo.CreateInfo(
			GetMissionDescription(mission),
			GetMissionName(mission),
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
		
		// TODO: improve error logging
		Print("Unable to get mission name!", LogLevel.ERROR);
		return "";
	}
	
	string GetMissionDescription(PAND_Mission mission)
	{
		// TODO: maybe we can improve this at some point by storing mission defs in a map. Should be fine for now with how small the data set is, though
		foreach (PAND_MissionDefinition definition : m_Config.m_aMissionDefinitions)
			if (mission.GetType() == definition.m_eType) return definition.m_sDescription;
		
		// TODO: improve error logging
		Print("Unable to get mission description!", LogLevel.ERROR);
		return "";
	}
	
    PAND_MissionType GetRandomMissionType()
    {
		PAND_MissionDefinition definition = m_Config.m_aMissionDefinitions.GetRandomElement();
        return definition.m_eType;
    }
			
	protected vector GetRandomVacantMissionLocation()
	{	
		array<PAND_MissionLocationEntity> vacantLocations = PAND_MissionLocationEntity.GetAllVacantLocations();
		if (vacantLocations.Count() == 0)
		{
			Print("[WASTELAND] WR_MissionManagerComponent: No vacant mission locations!", LogLevel.ERROR);
			return Vector(0, 0, 0);
		}
		
		PAND_MissionLocationEntity randomLocation = vacantLocations.GetRandomElement();
		// TODO: check if there are any entities inside/nearby this mission location's trigger zone. We don't want to spawn missions on people's heads
		randomLocation.SetIsHostingMission(true); // This must be set to false once the mission finishes
		return randomLocation.GetOrigin();
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
			// TODO: improve error logging
			Print("Tried to spawn mission prop, but no mission definition was found!", LogLevel.ERROR);
			return null;
		}
		
		ResourceName propResource = mission.GetDefinition().m_sPropPrefab;
		if (!propResource) return true; // Props are not required, so exit successfully

		propEntity = WR_Utils.SpawnPrefabInWorld(propResource, mission.GetPosition());
		propEntity.SetYawPitchRoll(WR_Utils.GetRandomHorizontalDirectionAngles());
		
		return propEntity;
	}
	
	private bool SpawnAiGroups(PAND_Mission mission, out array<SCR_AIGroup> aiGroups)
	{
		if (!mission.GetDefinition())
		{
			// TODO: improve error logging
			Print("Tried to spawn mission AI groups, but no mission definition was found!", LogLevel.ERROR);
			return null;
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
//				// TODO: improve error logging
//				Print("A safe spawn position was unable to be found for this mission's AI!", LogLevel.ERROR);
//				return false;
//			}
			
			// Spawn the group prefab
			IEntity aiGroupEntity = WR_Utils.SpawnPrefabInWorld(aiGroupResource, spawnPos);
			SCR_AIGroup aiGroupInstance = SCR_AIGroup.Cast(aiGroupEntity);	
	
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
			// TODO: improve error logging
			Print("Tried to spawn mission reward, but no mission definition was found!", LogLevel.ERROR);
			return null;
		}
		
		array<ResourceName> rewardPrefabs = mission.GetDefinition().m_sRewardPrefabs;
		
		if (!rewardPrefabs || rewardPrefabs.Count() == 0)
		{
			// TODO: improve error logging
			Print("Tried to spawn mission reward, but no rewards were defined!", LogLevel.ERROR);
			return null;
		}
		
		rewardEntities = {};
		
		// Spawn first reward prefab at exact mission position
		IEntity initialRewardEntity = WR_Utils.SpawnPrefabInWorld(rewardPrefabs[0], mission.GetPosition());
		rewardEntities.Insert(initialRewardEntity);
		
		// Spawn the rest at random nearby positions
		for (int i = 1; i < rewardPrefabs.Count(); i++)
		{
			// Find a safe spot
			vector spawnPos;
			bool safePosFound = WR_Utils.TryGetRandomSafePosWithinRadius(spawnPos, mission.GetPosition(), 15.0, 10.0, 10.0, 2.0); // TODO: read these floats from a config
			if (!safePosFound)
			{
				// TODO: improve error logging
				Print("Unable to find a safe spawn position for this mission's reward!", LogLevel.ERROR);
				return false;
			}
			
			// Spawn the reward prefab
			IEntity rewardEntity = WR_Utils.SpawnPrefabInWorld(rewardPrefabs[i], spawnPos);
			rewardEntities.Insert(rewardEntity);
		}
		
		return true;
	}
	
	private void DestroyMission(PAND_Mission mission)
	{
		// This method cleans up a mission record and ALL items related to it, including its markers and rewards!
	}
}