class WR_LootSpawnerClass : GenericEntityClass{}


class WR_LootSpawner : GenericEntity
{
	
    [Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Which loot context to use for this spawn", enums: ParamEnumArray.FromEnum(WR_LootContextType))]
	WR_LootContextType m_LootContextType;

	ref static array<WR_LootSpawner> lootSpawners;
	vector m_WorldTransform[4];
	
    void ZEL_StaticLootSpawner(IEntitySource src, IEntity parent)
    {
		SetEventMask(EntityEvent.INIT);
    }
		
	protected override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!lootSpawners) 
		{
			lootSpawners = {};
			Print("[Wasteland]: Initialized Loot Spawner Array");
		}
		
		lootSpawners.Insert(this);
	}

	bool TrySpawnLoot()
	{
		if (!Replication.IsServer())
			return false;
		if(GetChildren())
			return false;
		
		//Get Item Resource to be spawned
		WR_LootSpawnContext lootContext = WR_LootSpawnContextPresets.GetLootContextByType(m_LootContextType);
		array<ResourceName> itemsToSpawn = lootContext.GetRandomItems(1);
		
		foreach (ResourceName item : itemsToSpawn)
		{	
			Resource m_Resource = Resource.Load(item);
		
			//Get position of where to spawn it
			GetWorldTransform(m_WorldTransform);
			EntitySpawnParams params();
			m_WorldTransform[3][1] = m_WorldTransform[3][1] + 0.200;
			params.Transform = m_WorldTransform;
		
			//Spawn the item
			IEntity newEnt = GetGame().SpawnEntityPrefab(m_Resource, GetGame().GetWorld(), params);
			if (!newEnt)
				return false;
		
			SCR_EntityHelper.SnapToGround(newEnt);
			AddChild(newEnt, -1, EAddChildFlags.NONE);
		}
		
		return true;
	}
	
	void ~WR_LootSpawner()
    {
    }
	
}



