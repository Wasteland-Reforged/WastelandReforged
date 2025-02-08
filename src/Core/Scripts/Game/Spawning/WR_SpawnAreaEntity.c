[EntityEditorProps(category: "WastelandReforged", description: "Marks a spherical area players can spawn inside.")]
class WR_SpawnAreaEntityClass : BaseGameTriggerEntityClass
{
	
}

class WR_SpawnAreaEntity : BaseGameTriggerEntity
{
	// Categories
	private const string CATEGORY_SPAWNAREA = "Spawn Area";
	
	ref WR_Logger<WR_SpawnAreaEntity> logger = new WR_Logger<WR_SpawnAreaEntity>(this);
	
	[Attribute(defvalue: "", desc: "Name of the spawn area", category: CATEGORY_SPAWNAREA)]
	protected string m_Name;
	
	[Attribute("", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(SpawnAreaCategory), category: CATEGORY_SPAWNAREA)]
	SpawnAreaCategory spawnAreaCategory;
	
	[Attribute(defvalue: "true", desc: "Determines if players should spawn here", category: CATEGORY_SPAWNAREA)]
	protected bool m_IsActive;

	override void EOnInit(IEntity owner)
	{
		logger.LogDebug("Initializing " + GetSpawnAreaName() + " spawn area entity...");
	}
	
	string GetSpawnAreaName()
	{
		return m_Name;
	}
	
	SpawnAreaCategory GetSpawnAreaCategory()
	{
		return spawnAreaCategory;
	}
}