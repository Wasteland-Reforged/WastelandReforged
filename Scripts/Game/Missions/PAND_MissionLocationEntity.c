[EntityEditorProps(category: "WastelandReforged", description: "Marks a location where objectives can spawn.")]
class PAND_MissionLocationEntityClass : SCR_ScenarioFrameworkTriggerEntityClass
{
	
}

class PAND_MissionLocationEntity : SCR_ScenarioFrameworkTriggerEntity //BaseGameTriggerEntity
{
	// Categories
	private const string CATEGORY_MISSION_LOCATION = "Mission Location";
	
	private static ref array<PAND_MissionLocationEntity> MissionLocationEntities;
	
	protected BaseRplComponent m_RplComponent;
	
	[Attribute(defvalue: "true", desc: "Determines if missions should spawn here", category: CATEGORY_MISSION_LOCATION)]
	private bool m_bIsActive;
	
	private PAND_Mission m_CurrentMission;
	private bool m_bIsHostingMission;
	
//	void PAND_MissionLocationEntity(IEntitySource src, IEntity parent)
//	{
//		SetEventMask(EntityEvent.INIT | EntityEvent.FRAME);
//	}
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!MissionLocationEntities)
		{
			MissionLocationEntities = {};
			Print("[WASTELAND] PAND_MissionLocationEntity: Initialized mission location list.");
		}
		
		m_RplComponent = BaseRplComponent.Cast(FindComponent(BaseRplComponent));
		if (!m_RplComponent)
		{
			Print("[WASTELAND] PAND_MissionLocationEntity: Parent entity must have RplComponent!", LogLevel.ERROR);
			return;
		}
		
		SetActivationPresence(SCR_EScenarioFrameworkTriggerActivation.PLAYER);
		GetOnActivate().Insert(OnPlayerEnteredMissionLocation);
		
		MissionLocationEntities.Insert(this);
	}
	
	void ~PAND_MissionLocationEntity()
	{
		MissionLocationEntities = null;
	}
	
	static array<PAND_MissionLocationEntity> GetAllLocations()
	{
		return MissionLocationEntities;
	}
	
	static array<PAND_MissionLocationEntity> GetAllVacantLocations()
	{
		array<PAND_MissionLocationEntity> vacantLocations = {};
		
		foreach (auto location : MissionLocationEntities)
		{
			if (!location.m_bIsHostingMission)
				vacantLocations.Insert(location);
		}
		return vacantLocations;
	}
	
	bool GetIsActive()
	{
		return m_bIsActive;
	}
	
	bool GetIsHostingMission()
	{
		return m_bIsHostingMission;
	}
	
	void SetIsHostingMission(bool isHostingMission)
	{
		m_bIsHostingMission = isHostingMission;
	}
	
	PAND_Mission GetCurrentMission()
	{
		return m_CurrentMission;
	}
	
	void SetCurrentMission(PAND_Mission mission)
	{
		m_CurrentMission = mission;
		m_bIsHostingMission = mission != null;
	}
	
	// I have no idea if this will work properly on multiplayer
	void OnPlayerEnteredMissionLocation()
	{
		if (m_RplComponent.Role() != RplRole.Authority) return;
		if (!m_bIsHostingMission) return;
		
		PAND_MissionControllerComponent missionController = PAND_MissionControllerComponent.	Cast(GetGame().GetGameMode().FindComponent(PAND_MissionControllerComponent));
		
		missionController.OnPlayerEnteredMissionZone(m_CurrentMission, this);
	}
}