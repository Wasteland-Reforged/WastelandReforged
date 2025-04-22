[EntityEditorProps(category: "WastelandReforged", description: "Marks a location where objectives can spawn.")]
class WR_MissionLocationEntityClass : SCR_ScenarioFrameworkTriggerEntityClass
{
	
}

class WR_MissionLocationEntity : SCR_ScenarioFrameworkTriggerEntity
{
	// Categories
	private const string CATEGORY_MISSION_LOCATION = "Mission Location";
	
	ref WR_Logger<WR_MissionLocationEntity> logger = new WR_Logger<WR_MissionLocationEntity>(this);
	
	private static ref array<WR_MissionLocationEntity> MissionLocationEntities;
	
	protected BaseRplComponent m_RplComponent;
	
	[Attribute(defvalue: "true", desc: "Determines if missions should spawn here", category: CATEGORY_MISSION_LOCATION)]
	private bool m_bIsActive;
	
	private WR_Mission m_CurrentMission;
	private bool m_bIsHostingMission;

	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Size of the mission location. Determines which missions spawn here. Does NOT affect the radius of the location.", enums: ParamEnumArray.FromEnum(WR_MissionLocationSize), category: CATEGORY_MISSION_LOCATION)]
	private WR_MissionLocationSize m_eSize;
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!MissionLocationEntities)
		{
			MissionLocationEntities = {};
			
			logger.LogNormal("Initialized mission location list.");
		}
		
		m_RplComponent = BaseRplComponent.Cast(FindComponent(BaseRplComponent));
		if (!m_RplComponent)
		{
			logger.LogError("Parent entity must have RplComponent!");
			return;
		}
		
		SetActivationPresence(SCR_EScenarioFrameworkTriggerActivation.PLAYER);
		
		MissionLocationEntities.Insert(this);
	}
	
	void ~WR_MissionLocationEntity()
	{
		MissionLocationEntities = null;
	}
	
	static array<WR_MissionLocationEntity> GetAllLocations()
	{
		return MissionLocationEntities;
	}
	
	static array<WR_MissionLocationEntity> GetAllVacantLocations(WR_MissionLocationSize requiredSize)
	{
		array<WR_MissionLocationEntity> vacantLocations = {};
		
		foreach (auto location : MissionLocationEntities)
		{
			if (!location.m_bIsHostingMission && location.GetSize() == requiredSize && location.m_bIsActive)
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
	
	WR_Mission GetCurrentMission()
	{
		return m_CurrentMission;
	}
	
	WR_MissionLocationSize GetSize()
	{
		return m_eSize;
	}
	
	void SetCurrentMission(WR_Mission mission)
	{
		m_CurrentMission = mission;
		m_bIsHostingMission = mission != null;
	}

}