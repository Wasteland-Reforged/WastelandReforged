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
	
	ref static array<SPHCU_SpawnAreaEntity> SpawnAreas;
	
	override void EOnInit(IEntity owner)
	{
		if (!SpawnAreas)
		{
			SpawnAreas = {};
		}
		
		SpawnAreas.Insert(this);
		
		Print(SpawnAreas);
		Print(SpawnAreas.Count());
	}
	
	void ~SPHCU_SpawnAreaEntity()
	{
		SpawnAreas.RemoveItem(this)
	}
	
	string GetSpawnAreaName()
	{
		return m_Name;
	}
}