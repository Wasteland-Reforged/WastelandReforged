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

	vector m_WorldTransform[4];
	
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
			
			//Guns and Mags should be lifted slightly and on their sides. Mags should spawn a short distance away from their gun
			if (newEnt.FindComponent(WeaponComponent))
			{
				newEnt.SetOrigin(newEnt.GetOrigin() + {0.0, 0.02, 0.0});
				newEnt.SetYawPitchRoll(m_vItemRotation + GetYawPitchRoll());
			}
			else if (newEnt.FindComponent(MagazineComponent))
			{
				newEnt.SetOrigin(newEnt.GetOrigin() + getLootRandomOffset() + {0.0, 0.02, 0.0});
				newEnt.SetYawPitchRoll(m_vItemRotation + GetYawPitchRoll());
			}
		
			AddChild(newEnt, -1, EAddChildFlags.NONE);

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