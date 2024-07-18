[EntityEditorProps(category: "WastelandReforged", description: "Marks a location where objectives can spawn.")]
class PAND_MissionLocationEntityClass : GenericEntityClass
{
	
}

class PAND_MissionLocationEntity : GenericEntity
{
	// Categories
	private const string CATEGORY_MISSIONLOCATION = "Mission Location";
	
	[Attribute(defvalue: "true", desc: "Determines if missions should spawn here", category: CATEGORY_MISSIONLOCATION)]
	protected bool m_IsActive;

	static ref array<PAND_MissionLocationEntity> MissionLocationEntities;
	
	void PAND_MissionLocationEntity(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
	
	override void EOnInit(IEntity owner)
	{
		if (!MissionLocationEntities)
		{
			MissionLocationEntities = {};
			Print("[WASTELAND] Initialized mission location list.");
		}
		
		MissionLocationEntities.Insert(this);
	}
}