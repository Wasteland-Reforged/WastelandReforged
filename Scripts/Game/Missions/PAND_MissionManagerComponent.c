class PAND_MissionManagerComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class PAND_MissionManagerComponent : SCR_BaseGameModeComponent
{
    [Attribute("3", UIWidgets.Slider, "Max number of missions that can be active at once", "0 10 1")]
	protected int m_iMaxActiveMissions;
    
	protected SCR_BaseGameMode m_Parent;
	protected BaseRplComponent m_RplComponent;
	
    protected int m_iMissionCounter;

    //[RplProp(onRplName: "OnActiveMissionsChanged")]
    protected ref array<ref PAND_Mission> m_aActiveMissions = {};
	
	//[RplProp(onRplName: "OnActiveMissionsChanged")]
    //protected ref map<int, ref PAND_Mission> m_mActiveMissions = new map<int, ref PAND_Mission>();
    // Can maybe upgrade this by adding a PAND_Mission variable 'lastUpdatedMission'. Then we can just send incremental updates by updating that variable. Of course, we will also need to provide means for clients to get the full list of missions when they connect to the game.
	
	[RplProp(onRplName: "OnActiveMissionsChanged")]
	protected ref PAND_Mission m_lastUpdatedMission = new PAND_Mission();
	
//	[RplProp(onRplName: "OnActiveMissionsChanged")]
	protected int m_iTestInt = 0;
	
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
		
		Print("[WASTELAND] Initializing PAND_MissionManagerComponent...", LogLevel.NORMAL);
		Print("[WASTELAND] RplComponent role: " + m_RplComponent.Role(), LogLevel.NORMAL);
		
		//m_lastUpdatedMission = PAND_Mission.GetInitialMission();
	}
	
	override void OnGameModeStart()
	{
		if (m_RplComponent.Role() == RplRole.Authority)
		{
			GetGame().GetCallqueue().CallLater(StartNewMission, 16*1000, false);
		
			//StartNewMission(); // upgrade this to start multiple missions, track existing missions, etc.
		}
	}
	
    protected void StartNewMission()
    {
        // Ensure we are the authority here
        if (m_RplComponent.Role() != RplRole.Authority) return;

        if (m_iMaxActiveMissions == 0 || m_aActiveMissions.Count() >= m_iMaxActiveMissions) return;

		Print("[WASTELAND] PAND_MissionManagerComponent: (Authority) Starting new mission...");
		
        // Create mission object
		int newMissionId = GetNewMissionId();
        //PAND_Mission mission = PAND_Mission.CreateMission(newMissionId, GetRandomMissionType(), GetRandomMissionPosition());
		ref PAND_Mission mission = PAND_Mission.CreateMission(newMissionId, PAND_MissionType.PaulTest, "100 69 1");

        // Instantiate mission NPCs and rewards in the world (I don't think we need to handle replication for this. I think the game will do it automatically when players get close to the mission. Not sure though. Will require research/testing)
		// TODO: implement this
		
        // Add mission object to mission list (where it should hopefully get propagated by Rpl framework)
        m_aActiveMissions.Insert(mission);
		
//		bool success = m_mActiveMissions.Insert(mission.m_iMissionId, mission);
//		if (!success)
//		{
//			Print(
//				"[WASTELAND] WR_MissionManagerComponent: Unable to insert mission into active mission list! (ID: " + newMissionId + ") Aborting mission creation process."
//				, LogLevel.ERROR
//			); 
//			return;
//		}

		AudioSystem.PlaySound("{05A018350AAD3106}Sounds/task.wav");
		m_lastUpdatedMission = mission;
		Replication.BumpMe();
    }

	protected void OnActiveMissionsChanged()
    {
		GetGame().GetCallqueue().CallLater(HandleNewMissions, 2*1000, false);
	}
	
    protected void HandleNewMissions()
    {
        
		Print("[WASTELAND] PAND_MissionManagerComponent: Entered function OnActiveMissionsChanged", LogLevel.DEBUG);
		
		// Ensure we are a proxy here
        if (m_RplComponent.Role() != RplRole.Proxy) return;
		
		if (!m_aActiveMissions) return;
		
		
		Print("[WASTELAND] PAND_MissionManagerComponent: (Proxy) Received active missions change!!! TestInt = " + m_iTestInt);
		
		//if (m_aActiveMissions.Count() < 1) return;
		
		if (m_lastUpdatedMission.GetMissionId() == -1) // -1 means this mission object is a dummy object
			return;
		AudioSystem.PlaySound("{863213E1338E3773}Sounds/hint.wav");
		
		vector pos = m_lastUpdatedMission.GetPosition();
		
		
		SCR_HintUIInfo hintInfo = SCR_HintUIInfo.CreateInfo("Objective: " + m_lastUpdatedMission.GetName() + m_lastUpdatedMission.GetCreatedAtTime() + pos[0] + " " + pos[1] + " " + pos[2], "New Objective", 10.0, EHint.UNDEFINED, EFieldManualEntryId.NONE, true);
		//SCR_HintUIInfo hintInfo = SCR_HintUIInfo.CreateInfo("Objective: " + m_aActiveMissions[0].GetName(), "New Objective", 10.0, EHint.UNDEFINED, EFieldManualEntryId.NONE, true);
		
		SCR_HintManagerComponent.ShowHint(hintInfo);
		
        // Figure out if a mission was added or removed

        // Create map marker

        // Send hint and sound notification
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
		return new map<int, ref PAND_Mission>(); // Placeholder. It's certainly not as simple as just returning the list. Need to find out how we can transfer a specific variable on-demand from authority to proxy.
    }

    PAND_MissionType GetRandomMissionType()
    {
        array<PAND_MissionType> missionTypes =
        {
            PAND_MissionType.CaptureWeapons,
            PAND_MissionType.CaptureVehicle
        };

        return missionTypes.GetRandomElement();
    }

    vector GetRandomMissionPosition()
    {
        // TODO: make this choose from a list of mission locations
        return "0 0 0";
    }

    private 
}