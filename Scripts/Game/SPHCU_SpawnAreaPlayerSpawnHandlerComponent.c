[ComponentEditorProps(category: "Tutorial/Component", description: "Warn then teleport humans that are too close to the entity")]
class SPHCU_SpawnAreaPlayerSpawnHandlerComponentClass : ScriptComponentClass
{
	
}

class SPHCU_SpawnAreaPlayerSpawnHandlerComponent : ScriptComponent
{
	private SPHCU_SpawnAreaEntity _parent;
	
	ref static array<SPHCU_SpawnAreaPlayerSpawnHandlerComponent> PlayerSpawnHandlerComponents;
	
	override void OnPostInit(IEntity owner)
	{
		_parent = SPHCU_SpawnAreaEntity.Cast(owner);
		if (!_parent)
		{
			Print("[WASTELAND] Parent entity of SPHCU_SpawnAreaPlayerSpawnHandlerComponent must be a SPHCU_SpawnAreaEntity!");
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
		
		// TODO: Make character prefab selection dynamic
		ResourceName characterPrefab = "{84B40583F4D1B7A3}Prefabs/Characters/Factions/INDFOR/FIA/Character_FIA_Rifleman.et";

		// Choose a spawn area and find a safe spawn position inside it
		SPHCU_SpawnAreaPlayerSpawnHandlerComponent spawnArea = GetRandomSpawnArea();
		if (!spawnArea) return false; // Fail if no spawn area found
		
		// Configure spawn position parameters
		float areaToCheck = 10; 			// Radius that will be checked if the initially passed pos is not safe
		float xzPaddingRadius = 0.5;	 	// Minimum radius of empty space to have around the chosen position
		float yPaddingDistance = 2; 		// Minimum distance of empty space to have above and below the chosen position 
		
		bool foundSafePos = SPHCU_Utils.TryGetSafePos(
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
		newlySpawnedPlayer.SetYawPitchRoll(SPHCU_Utils.GetRandomHorizontalDirectionAngles());
		
		if (isSpawnSuccessful)
		{
			if (originalPlayerEntity)
				delete originalPlayerEntity; // Delete player's original body if this is not the player's initial spawn (i.e. a respawn)
			
			Print("[WASTELAND] Player successfully spawned in " + spawnArea.GetSpawnAreaName());
		}
		
		return isSpawnSuccessful;
	}
	
//	private static bool TryGetSafePos(out vector safePos, SPHCU_SpawnAreaPlayerSpawnHandlerComponent spawnArea)
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

	private static SPHCU_SpawnAreaPlayerSpawnHandlerComponent GetRandomSpawnArea()
	{
		array<SPHCU_SpawnAreaPlayerSpawnHandlerComponent> spawnAreas = SPHCU_SpawnAreaPlayerSpawnHandlerComponent.PlayerSpawnHandlerComponents;
		
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

		return SPHCU_SpawnAreaPlayerSpawnHandlerComponent.PlayerSpawnHandlerComponents.GetRandomElement();
	}

	string GetSpawnAreaName()
	{
		return _parent.GetSpawnAreaName();
	}
	
	void ~SPHCU_SpawnAreaPlayerSpawnHandlerComponent()
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