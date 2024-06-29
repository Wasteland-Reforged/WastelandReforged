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
			Print("[WASTELAND] Parent entity of WR_SpawnAreaPlayerSpawnHandlerComponent must be a WR_SpawnAreaEntity!");
			return;
		}
		
		if (!PlayerSpawnHandlerComponents)
		{
			PlayerSpawnHandlerComponents = {};
			Print("[WASTELAND] Initialized player spawn area handler component list.");
		}
		
		PlayerSpawnHandlerComponents.Insert(this);
		Print("[WASTELAND] Inserted " + GetSpawnAreaName() + " into the player spawn handler component list.");
	}
	
	static bool TryRespawnPlayer(out vector respawnPos)
	{
		PlayerController playerController = GetGame().GetPlayerController();
		IEntity originalPlayerEntity = playerController.GetControlledEntity();
		
		SCR_RespawnComponent respawnComponent = SCR_RespawnComponent.Cast(playerController.GetRespawnComponent());
		if (!respawnComponent) return false;
		
		//Dynamic Character Prefab Selection
		ResourceName characterPrefab;
		SCR_ChimeraCharacter playerChar = SCR_ChimeraCharacter.Cast(originalPlayerEntity);
		switch (playerChar.GetFactionKey()) {
			case "BLUFOR":
				characterPrefab = "{B39F86B378284BFC}Prefabs/Characters/Factions/BLUFOR/US_Army/WR_Character_US_Base.et";
				break;
			case "OPFOR":
				characterPrefab = "{8449DECFA1B5831F}Prefabs/Characters/Factions/OPFOR/USSR_Army/WR_Character_USSR_Base.et";
				break;
			case "INDFOR":
				characterPrefab = "{111788B8922611CC}Prefabs/Characters/Factions/INDFOR/FIA/WR_Character_FIA_Base.et";
				break;
			default:
				Print("[WR_SpawnAreaPlayerSpawnHandlerComponent] Unknown Faction Key!", LogLevel.ERROR);
				return false;
		}

		// Choose a spawn area and find a safe spawn position inside it
		WR_SpawnAreaPlayerSpawnHandlerComponent spawnArea = GetRandomSpawnArea();
		if (!spawnArea) return false; // Fail if no spawn area found
		
		// Configure spawn position parameters
		float areaToCheck = 10; 			// Radius that will be checked if the initially passed pos is not safe
		float xzPaddingRadius = 0.5;	 	// Minimum radius of empty space to have around the chosen position
		float yPaddingDistance = 2; 		// Minimum distance of empty space to have above and below the chosen position 
		
		bool foundSafePos = WR_Utils.TryGetSafePos(
												respawnPos
												, spawnArea._parent.GetOrigin()
												, spawnArea._parent.GetSphereRadius()
												, areaToCheck
												, xzPaddingRadius
												, yPaddingDistance);
		
		if (!foundSafePos) return false; // Fail if no safe position found

		// Instantiate a FreeSpawnData object using the spawn position
		SCR_FreeSpawnData data = new SCR_FreeSpawnData(characterPrefab, respawnPos);
		
		// Spawn the player
		Print("[WASTELAND] Attempting to spawn player in " + spawnArea.GetSpawnAreaName() + " at " + respawnPos);
		bool isSpawnSuccessful = respawnComponent.RequestSpawn(data);
		
		// Rotate player in random direction
		IEntity newlySpawnedPlayer = playerController.GetControlledEntity();
		newlySpawnedPlayer.SetYawPitchRoll(WR_Utils.GetRandomHorizontalDirectionAngles());
		
		if (isSpawnSuccessful)
		{
			if (originalPlayerEntity)
				delete originalPlayerEntity; // Delete player's original body if this is not the player's initial spawn (i.e. a respawn)
			
			Print("[WASTELAND] Player successfully spawned in " + spawnArea.GetSpawnAreaName());
		}
		
		return isSpawnSuccessful;
	}

	static SCR_FreeSpawnData GetSpawnData()
	{
		PlayerController playerController = GetGame().GetPlayerController();
		IEntity originalPlayerEntity = playerController.GetControlledEntity();
		
		SCR_RespawnComponent respawnComponent = SCR_RespawnComponent.Cast(playerController.GetRespawnComponent());
		if (!respawnComponent) return null;
		
		// Select loadout
		ResourceName characterPrefabName = "{B39F86B378284BFC}Prefabs/Characters/Factions/BLUFOR/US_Army/WR_Character_US_Base.et";
//		SCR_ChimeraCharacter playerChar = SCR_ChimeraCharacter.Cast(originalPlayerEntity);
//		switch (playerChar.GetFactionKey()) {
//			case "BLUFOR":
//				characterPrefab = "{B39F86B378284BFC}Prefabs/Characters/Factions/BLUFOR/US_Army/WR_Character_US_Base.et";
//				break;
//			case "OPFOR":
//				characterPrefab = "{8449DECFA1B5831F}Prefabs/Characters/Factions/OPFOR/USSR_Army/WR_Character_USSR_Base.et";
//				break;
//			case "INDFOR":
//				characterPrefab = "{111788B8922611CC}Prefabs/Characters/Factions/INDFOR/FIA/WR_Character_FIA_Base.et";
//				break;
//			default:
//				Print("[WR_SpawnAreaPlayerSpawnHandlerComponent] Unknown Faction Key!", LogLevel.ERROR);
//				return null;
//		}

		// Choose a spawn area and find a safe spawn position inside it
		WR_SpawnAreaPlayerSpawnHandlerComponent spawnArea = GetRandomSpawnArea();
		if (!spawnArea) return null; // Fail if no spawn area found
		
		// Configure spawn position parameters
		float areaToCheck = 10; 			// Radius that will be checked if the initially passed pos is not safe
		float xzPaddingRadius = 0.5;	 	// Minimum radius of empty space to have around the chosen position
		float yPaddingDistance = 2; 		// Minimum distance of empty space to have above and below the chosen position 
		
		vector respawnPos;
		bool foundSafePos = WR_Utils.TryGetSafePos(
												respawnPos
												, spawnArea._parent.GetOrigin()
												, spawnArea._parent.GetSphereRadius()
												, areaToCheck
												, xzPaddingRadius
												, yPaddingDistance);
		
		if (!foundSafePos) return null; // Fail if no safe position found

		// Instantiate a FreeSpawnData object using the spawn position
		return new SCR_FreeSpawnData(characterPrefabName, respawnPos);
	}
	
//	private static bool TryGetSafePos(out vector safePos, WR_SpawnAreaPlayerSpawnHandlerComponent spawnArea)
//	{
//		// Find a random safe position within the spawn area to spawn the player at
//		vector spawnAreaCenterPos = spawnArea._parent.GetOrigin();
//		float spawnAreaRadius = spawnArea._parent.GetSphereRadius();
//		
//		vector randomDir = Vector(Math.RandomFloat(0, 360), 0, 0).AnglesToVector();
//		float randomDist = Math.RandomFloat(0, spawnAreaRadius);
//		
//		float areaToCheck = 25;
//		vector posToCheck = spawnAreaCenterPos + randomDir * randomDist;
//		
//		return SCR_WorldTools.FindEmptyTerrainPosition(safePos, posToCheck, areaToCheck);
//	}

	private static WR_SpawnAreaPlayerSpawnHandlerComponent GetRandomSpawnArea()
	{
		array<WR_SpawnAreaPlayerSpawnHandlerComponent> spawnAreas = WR_SpawnAreaPlayerSpawnHandlerComponent.PlayerSpawnHandlerComponents;
		
		// Perform validation
		if (!spawnAreas)
		{
			Print("[WASTELAND] The player spawn area list is null!");
			return null;
		}
		else if (spawnAreas.Count() < 1)
		{
			Print("[WASTELAND] The player spawn area list is empty! Place one or more spawn areas with player spawn handler components attached.");
			return null;
		}

		return WR_SpawnAreaPlayerSpawnHandlerComponent.PlayerSpawnHandlerComponents.GetRandomElement();
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