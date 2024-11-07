class WR_LootSpawnerClass : GenericEntityClass{}


class WR_LootSpawner : GenericEntity
{
    [Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Which loot context to use for this spawn", enums: ParamEnumArray.FromEnum(WR_LootContextType))]
	WR_LootContextType m_LootContextType;

	vector m_WorldTransform[4];
	
	protected override void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);
		
		if (!WR_LootSystem.m_aLootSpawners) 
		{
			WR_LootSystem.m_aLootSpawners = {};
			WR_Logger.LogDebug("Initialized loot spawner array in loot system.");
		}
		
		WR_LootSystem.m_aLootSpawners.Insert(this);
		WR_Logger.LogDebug("Initialized loot spawner: " + this);
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
		
		WR_LootSystem.m_iSuccessfulSpawns++;
		return true;
	}
}