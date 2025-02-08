[ComponentEditorProps(category: "Tutorial/Component", description: "Warn then teleport humans that are too close to the entity")]
class WR_SpawnAreaPlayerSpawnHandlerComponentClass : ScriptComponentClass
{
	
}

class WR_SpawnAreaPlayerSpawnHandlerComponent : ScriptComponent
{
	ref WR_Logger<WR_SpawnAreaPlayerSpawnHandlerComponent> logger = new WR_Logger<WR_SpawnAreaPlayerSpawnHandlerComponent>(this);
	
	private WR_SpawnAreaEntity _parent;
	
	ref static array<WR_SpawnAreaPlayerSpawnHandlerComponent> s_aPlayerSpawnHandlerComponents;
	
	override void OnPostInit(IEntity owner)
	{
		WR_SpawnAreaEntity _parent = WR_SpawnAreaEntity.Cast(owner);
		if (!_parent)
		{
			logger.LogError("Parent entity of WR_SpawnAreaPlayerSpawnHandlerComponent must be a WR_SpawnAreaEntity!");
			return;
		}
		
		if (!s_aPlayerSpawnHandlerComponents)
		{
			s_aPlayerSpawnHandlerComponents = {};
			logger.LogNormal("Initialized player spawn area handler component list.");
		}
		
		s_aPlayerSpawnHandlerComponents.Insert(this);
		logger.LogDebug("Inserted " + GetSpawnAreaName() + " into the player spawn handler component list.");
	}

	static SCR_FreeSpawnData GetSpawnData(string factionKey, SpawnAreaCategory spawnCategory)
	{
		ResourceName characterPrefabName;
		switch (factionKey)
		{
			case "US":
				characterPrefabName = "{B39F86B378284BFC}Prefabs/Characters/Factions/BLUFOR/US_Army/WR_Character_US_Base.et";
				break;
			case "USSR":
				characterPrefabName = "{8449DECFA1B5831F}Prefabs/Characters/Factions/OPFOR/USSR_Army/WR_Character_USSR_Base.et";
				break;
			case "FIA":
				characterPrefabName = "{CEE7531F4FBAEB38}Prefabs/Characters/Factions/CIV/Dockworker/WR_Character_CIV_Base.et";
				break;
			default:
				Print("[WASTELAND] WR_SpawnAreaPlayerSpawnHandlerComponent: Unknown faction key!", LogLevel.ERROR);
				return null;
		}

		// Choose a spawn area and find a safe spawn position inside it
		WR_SpawnAreaPlayerSpawnHandlerComponent spawnArea = GetRandomSpawnArea(spawnCategory);
		if (!spawnArea)
			return null; // Fail if no spawn area found
		
		// Configure spawn position parameters
		float areaToCheck = 10; 			// Radius that will be checked if the initially passed pos is not safe
		float xzPaddingRadius = 0.5;	 	// Minimum radius of empty space to have around the chosen position
		float yPaddingDistance = 2; 		// Minimum distance of empty space to have above and below the chosen position 
		
		vector respawnPos;
		bool foundSafePos = WR_Utils.TryGetRandomSafePosWithinRadius(
												respawnPos
												, spawnArea._parent.GetOrigin()
												, spawnArea._parent.GetSphereRadius()
												, areaToCheck
												, xzPaddingRadius
												, yPaddingDistance);
		
		if (!foundSafePos) return null; // Fail if no safe position found

		vector heading = WR_Utils.GetRandomHorizontalDirectionAngles();
		
		// Instantiate a FreeSpawnData object using the spawn position
		return new SCR_FreeSpawnData(characterPrefabName, respawnPos, heading);
	}

	private static WR_SpawnAreaPlayerSpawnHandlerComponent GetRandomSpawnArea(SpawnAreaCategory spawnCategory)
	{
		array<WR_SpawnAreaPlayerSpawnHandlerComponent> spawnAreas = WR_SpawnAreaPlayerSpawnHandlerComponent.s_aPlayerSpawnHandlerComponents;
		
		// Perform validation
		if (!spawnAreas)
		{
			Print("[WASTELAND] WR_SpawnAreaPlayerSpawnHandlerComponent: The player spawn area list is null!", LogLevel.ERROR);
			return null;
		}
		else if (spawnAreas.Count() == 0)
		{
			Print("[WASTELAND] WR_SpawnAreaPlayerSpawnHandlerComponent: The player spawn area list is empty! Place one or more spawn areas with player spawn handler components attached.", LogLevel.ERROR);
			return null;
		}

		// Roll a random spawn area until it picks one in the selected region; Cannot handle null value for category
		WR_SpawnAreaPlayerSpawnHandlerComponent spawnArea = spawnAreas.GetRandomElement();
		while (spawnArea.GetSpawnAreaCategory() != spawnCategory)
		{
			spawnArea = spawnAreas.GetRandomElement();
		}
		
		return spawnArea;
	}

	string GetSpawnAreaName()
	{
		return _parent.GetSpawnAreaName();
	}
	
	SpawnAreaCategory GetSpawnAreaCategory()
	{
		return _parent.GetSpawnAreaCategory();
	}
	
	void ~WR_SpawnAreaPlayerSpawnHandlerComponent()
	{
		if (!s_aPlayerSpawnHandlerComponents) 
			return;
		
		if (s_aPlayerSpawnHandlerComponents.Count() == 0)
		{
			s_aPlayerSpawnHandlerComponents = null;
			return;
		}
		
		s_aPlayerSpawnHandlerComponents.RemoveItem(this);
	}
}