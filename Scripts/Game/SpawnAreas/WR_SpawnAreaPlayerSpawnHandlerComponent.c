[ComponentEditorProps(category: "Tutorial/Component", description: "Warn then teleport humans that are too close to the entity")]
class WR_SpawnAreaPlayerSpawnHandlerComponentClass : ScriptComponentClass
{
	
}

class WR_SpawnAreaPlayerSpawnHandlerComponent : ScriptComponent
{
	private WR_SpawnAreaEntity _parent;
	
	ref static array<WR_SpawnAreaPlayerSpawnHandlerComponent> PlayerSpawnHandlerComponents;
	
	override void OnPostInit(IEntity owner)
	{
		_parent = WR_SpawnAreaEntity.Cast(owner);
		if (!_parent)
		{
			Print("[WASTELAND] WR_SpawnAreaPlayerSpawnHandlerComponent: Parent entity of WR_SpawnAreaPlayerSpawnHandlerComponent must be a WR_SpawnAreaEntity!", LogLevel.ERROR);
			return;
		}
		
		if (!PlayerSpawnHandlerComponents)
		{
			PlayerSpawnHandlerComponents = {};
			Print("[WASTELAND] WR_SpawnAreaPlayerSpawnHandlerComponent: Initialized player spawn area handler component list.", LogLevel.NORMAL);
		}
		
		PlayerSpawnHandlerComponents.Insert(this);
		Print("[WASTELAND] Inserted " + GetSpawnAreaName() + " into the player spawn handler component list.", LogLevel.SPAM);
	}

	static SCR_FreeSpawnData GetSpawnData(string factionKey)
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
		WR_SpawnAreaPlayerSpawnHandlerComponent spawnArea = GetRandomSpawnArea();
		if (!spawnArea) return null; // Fail if no spawn area found
		
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

	private static WR_SpawnAreaPlayerSpawnHandlerComponent GetRandomSpawnArea()
	{
		array<WR_SpawnAreaPlayerSpawnHandlerComponent> spawnAreas = WR_SpawnAreaPlayerSpawnHandlerComponent.PlayerSpawnHandlerComponents;
		
		// Perform validation
		if (!spawnAreas)
		{
			Print("[WASTELAND] WR_SpawnAreaPlayerSpawnHandlerComponent: The player spawn area list is null!");
			return null;
		}
		else if (spawnAreas.Count() < 1)
		{
			Print("[WASTELAND] WR_SpawnAreaPlayerSpawnHandlerComponent: The player spawn area list is empty! Place one or more spawn areas with player spawn handler components attached.");
			return null;
		}

		return spawnAreas.GetRandomElement();
	}

	string GetSpawnAreaName()
	{
		return _parent.GetSpawnAreaName();
	}
	
	void ~WR_SpawnAreaPlayerSpawnHandlerComponent()
	{
		if (!PlayerSpawnHandlerComponents) return;
		
		if (PlayerSpawnHandlerComponents.Count() == 0)
		{
			PlayerSpawnHandlerComponents = null;
			return;
		}
		
		PlayerSpawnHandlerComponents.RemoveItem(this);
	}
}