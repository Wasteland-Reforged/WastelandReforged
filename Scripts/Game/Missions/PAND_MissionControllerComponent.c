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

        // Create mission object
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
		HandleNewMission(m_lastUpdatedMission);
		
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
		// Ensure we are a proxy here
        if (m_RplComponent.Role() != RplRole.Proxy) return;
		
		if (!m_mActiveMissions) return;
			
		if (m_lastUpdatedMission.IsEmptyMission()) return;
		
		//////// Try to wrap as much of this logic up into one function that can be called from both the server (for in-game hosting) and the client
		Print("[WASTELAND] PAND_MissionManagerComponent: New mission received from authority (ID: " + m_lastUpdatedMission.GetMissionId() + ")", LogLevel.NORMAL);
		HandleNewMission(m_lastUpdatedMission);
    }
	
	protected void HandleNewMission(PAND_Mission newMission)
	{
		// We've never received this mission ID, so initialize a new mission.
		ref PAND_Mission mission;
		bool missionExists = m_mActiveMissions.Find(newMission.GetMissionId(), mission);
		
		if (!missionExists)
		{
			mission = newMission;
			
			bool success = m_mActiveMissions.Insert(mission.GetMissionId(), mission);
			if (!success)
			{
				Print("[WASTELAND] WR_MissionManagerComponent: Unable to insert mission into active mission list! (ID: " + mission.GetMissionId() + ") Aborting mission creation.", LogLevel.ERROR); 
				return;
			}
			
			CreateMarker(mission);
			CreateHint(mission);
			PlaySound();
		}
		else
		{
			// We have received a mission with this ID before, so update or delete existing mission record as required.
		}
		
		
		// Ensure we are the authority here
        if (m_RplComponent.Role() != RplRole.Authority) return;
		
		
		// Authority stuff down here (spawning NPCs, spawning rewards, etc)
		// Instantiate mission NPCs and rewards in the world (I don't think we need to handle replication for this. I think the game will do it automatically when players get close to the mission. Not sure though. Will require research/testing)
		// TODO: implement this
		
	}
	
	// TODO: maybe change this to be a create or update method?
	private void CreateMarker(PAND_Mission mission)
	{
		SCR_MapMarkerBase marker = new SCR_MapMarkerBase();
		
		marker.SetType(SCR_EMapMarkerType.PLACED_CUSTOM);
		marker.SetCustomText(mission.GetName());
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
			mission.GetDescription(),
			mission.GetName(),
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

    PAND_MissionType GetRandomMissionType()
    {
		// TODO: read from config
        array<PAND_MissionType> missionTypes =
        {
            PAND_MissionType.CAPTURE_WEAPONS,
            PAND_MissionType.CAPTURE_VEHICLE,
			PAND_MissionType.CAPTURE_BASE
        };

        return missionTypes.GetRandomElement();
    }

    vector GetRandomMissionPosition()
    {
        // TODO: make this choose from a list of mission locations
        return "0 0 0";
    }
	
//	protected SCR_AIGroup pawnMissionGroup(PAND_Mission mission)
//	{
//		EntitySpawnParams spawnParams = new EntitySpawnParams();
//		spawnParams.TransformMode = ETransformMode.WORLD;
//		spawnParams.Transform[3] = mission.GetPosition();
//		
//		Resource groupResource = Resource.Load(groupPrefabName);
//		SCR_AIGroup newGroup = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupResource, GetGame().GetWorld(), spawnParams));	
//
//		Resource waypointResource = Resource.Load("{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Defend.et");
//		SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(waypointResource, GetGame().GetWorld(), GenerateSpawnParameters(missionLocation)));
//
//		newGroup.AddWaypoint(waypoint);
//		return newGroup;
//	}
//		
	protected vector GetRandomVacantMissionLocation()
	{	
		array<PAND_MissionLocationEntity> vacantLocations = PAND_MissionLocationEntity.GetAllVacantLocations();
		if (vacantLocations.Count() == 0)
		{
			Print("[WASTELAND] WR_MissionManagerComponent: No vacant mission locations!", LogLevel.ERROR);
			return Vector(0, 0, 0);
		}
		
		PAND_MissionLocationEntity randomLocation = vacantLocations.GetRandomElement();
		randomLocation.SetIsHostingMission(true); // This must be set to false once the mission finishes
		return randomLocation.GetOrigin();
	}
}