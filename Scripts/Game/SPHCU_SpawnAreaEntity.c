[EntityEditorProps(category: "SPHCU/Wasteland", description: "Marks a circular area players can spawn inside.")]
class SPHCU_SpawnAreaEntityClass : BaseGameTriggerEntityClass
{
	
}

class SPHCU_SpawnAreaEntity : BaseGameTriggerEntity
{
	// Categories
	private const string CATEGORY_SPAWNAREA = "Spawn Area";
	
	[Attribute(defvalue: "", desc: "Name of the spawn area", category: CATEGORY_SPAWNAREA)]
	protected string m_Name;
	
	[Attribute(defvalue: "true", desc: "Determines if players should spawn here", category: CATEGORY_SPAWNAREA)]
	protected bool m_IsActive;
	
//	ref static array<SPHCU_SpawnAreaEntity> SpawnAreas;
//	
//	override void EOnInit(IEntity owner)
//	{
//		if (!m_IsActive) return;
//		
//		if (!SpawnAreas)
//		{
//			SpawnAreas = {};
//			Print("Initialized spawn area list.");
//		}
//		
//		SpawnAreas.Insert(this);
//		Print("Inserted " + this.GetSpawnAreaName() + " into the spawn area list");
//	}
//	
//	void ~SPHCU_SpawnAreaEntity()
//	{
//		SpawnAreas.RemoveItem(this)
//	}
//	
	string GetSpawnAreaName()
	{
		return m_Name;
	}
}