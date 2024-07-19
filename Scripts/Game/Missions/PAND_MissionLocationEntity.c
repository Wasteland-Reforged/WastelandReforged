[EntityEditorProps(category: "WastelandReforged", description: "Marks a location where objectives can spawn.")]
class PAND_MissionLocationEntityClass : BaseGameTriggerEntityClass
{
	
}

class PAND_MissionLocationEntity : BaseGameTriggerEntity
{
	// Categories
	private const string CATEGORY_MISSIONLOCATION = "Mission Location";
	
	[Attribute(defvalue: "true", desc: "Determines if missions should spawn here", category: CATEGORY_MISSIONLOCATION)]
	private bool m_bIsActive;
	
	private bool m_bIsHostingMission;

	private static ref array<PAND_MissionLocationEntity> MissionLocationEntities;
	
	void PAND_MissionLocationEntity(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
	
	override void EOnInit(IEntity owner)
	{
		if (!MissionLocationEntities)
		{
			MissionLocationEntities = {};
			Print("[WASTELAND] PAND_MissionLocationEntity: Initialized mission location list.");
		}
		
		float triggerRadius = 15.0; // used to detect when players are nearby to trigger mission completion
		SetSphereRadius(triggerRadius);
		
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
}