[EntityEditorProps(category: "SPHCU/Wasteland", description: "Marks a circular area players can spawn inside.")]
class WR_SpawnAreaEntityClass : BaseGameTriggerEntityClass
{
	
}

class WR_SpawnAreaEntity : BaseGameTriggerEntity
{
	// Categories
	private const string CATEGORY_SPAWNAREA = "Spawn Area";
	
	[Attribute(defvalue: "", desc: "Name of the spawn area", category: CATEGORY_SPAWNAREA)]
	protected string m_Name;
	
	[Attribute(defvalue: "true", desc: "Determines if players should spawn here", category: CATEGORY_SPAWNAREA)]
	protected bool m_IsActive;

	override void EOnInit(IEntity owner)
	{
		Print("[WASTELAND] Initializing " + GetSpawnAreaName() + " spawn area entity...");
	}
	
	string GetSpawnAreaName()
	{
		return m_Name;
	}
}