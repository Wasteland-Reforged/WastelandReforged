class WR_LootSpawnerClass : GenericEntityClass
{
	
}

class WR_LootSpawner : GenericEntity
{
    ref WR_Logger<WR_LootSpawner> logger = new WR_Logger<WR_LootSpawner>(this);
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Which loot context to use for this spawn", enums: ParamEnumArray.FromEnum(WR_LootContext))]
	WR_LootContext m_LootContextType;
	
	[Attribute("0 0 90", UIWidgets.EditBox, desc: "Rotation of item when spawned", category: "Loot")]
	vector m_vItemRotation;
	
	[Attribute("30", UIWidgets.EditBox, desc: "Minutes until a loot item is forced to be respawned", category: "Loot")]
	float m_fItemTimeoutM;

	float lastUpdateTimeS = 0;
	vector m_WorldTransform[4];
	ref map<IEntity, vector> m_aItemsAtThisSpawner = new map<IEntity, vector>();	//Key is the IEntity for the item, Value is the vector location that it spawned at
	
	ref RandomGenerator rnd = new RandomGenerator();
	
	protected override void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);
		
		if (!WR_LootSystem.m_aLootSpawners) 
		{
			WR_LootSystem.m_aLootSpawners = {};
			logger.LogDebug("Initialized loot spawner array in loot system.");
		}
		
		WR_LootSystem.m_aLootSpawners.Insert(this);
		logger.LogDebug("Initialized loot spawner: " + this);
	}
	
	private void ClearItems()
	{
		for (int i = 0; i < m_aItemsAtThisSpawner.Count(); i++) 
		{
			IEntity item = m_aItemsAtThisSpawner.GetKey(i);
			vector startingLocation = m_aItemsAtThisSpawner.GetElement(i);
			
			// Only delete item if it is still where it was originally spawned
			if (item.GetOrigin() != startingLocation)
				continue;
			
			SCR_EntityHelper.DeleteEntityAndChildren(item);
		}
		m_aItemsAtThisSpawner = new map<IEntity, vector>();
	}
	
	// Respawns loot ONLY if the spawner is empty, OR if the item has not been picked up in time
	bool TrySpawnLootTimeout(float currentTimeS, float spawnChance, float TimedOutLootRespawnChance)
	{
		float targetRespawnTimeS = lastUpdateTimeS + (m_fItemTimeoutM * 60);
		
		// Loot has timed out
		if (currentTimeS > targetRespawnTimeS)
		{
			lastUpdateTimeS = currentTimeS;
			return TrySpawnLoot(TimedOutLootRespawnChance);
		}
		
		// Loot Spawner is empty
		if (!GetChildren())
		{
			lastUpdateTimeS = currentTimeS;
			return TrySpawnLoot(spawnChance);
		}
		
		return false;
	}

	// Respawns loot regardless of if the spawner has items already or not
	bool TrySpawnLoot(float spawnChance)
	{
		if (!Replication.IsServer())
			return false;
		
		if (Math.RandomFloat01() > spawnChance)
			return false;
		
		// Clear out any items currently at this loot spawner
		ClearItems();
		
		// Get item resource to be spawned
		auto lootSpawningComponent = WR_LootSpawningComponent.GetInstance();
		array<ResourceName> itemsToSpawn = lootSpawningComponent.GetRandomItemsByCount(m_LootContextType, 1, 1);

		bool firstItem = true;
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
			if (!newEnt) {
				logger.LogError("Failed to spawn item: " + item);
				return false;
			}

			//All Items: Rotated randomly horizontally
			//Guns/Mags: Placed sideways
			//Additional Items: Spread
			
			newEnt.SetYawPitchRoll(GetYawPitchRoll() + WR_Utils.GetRandomHorizontalDirectionAngles());
			
			if (!firstItem)
			{
				newEnt.SetOrigin(newEnt.GetOrigin() + getLootRandomOffset());
			}
			
			SCR_EntityHelper.SnapToGround(newEnt);
			
			if (newEnt.FindComponent(WeaponComponent) || newEnt.FindComponent(MagazineComponent))
			{	
				newEnt.SetYawPitchRoll(GetYawPitchRoll() + m_vItemRotation);
			}
      
			newEnt.SetOrigin(newEnt.GetOrigin() + {0.0, 0.015, 0.0});
		
			m_aItemsAtThisSpawner.Insert(newEnt, newEnt.GetOrigin());
			firstItem = false;
		}
		
		WR_LootSystem.CountSuccessfulSpawn();
		return true;
	}
	
	protected vector getLootRandomOffset()
	{
		float offsetDiameter = 0.5;
		
		float xVal = (Math.RandomFloat01()*offsetDiameter)-(offsetDiameter/2);
		float zVal = (Math.RandomFloat01()*offsetDiameter)-(offsetDiameter/2);
		return {xVal, 0.0, zVal};
	}
}