class WR_Utils
{
	private static ref map<ResourceName, ResourceName> WeaponAmmoResourceNames;
	private static ref map<ResourceName, int> Weapons;
	private static ref map<ResourceName, int> WeaponsNonReloadable;
	
	//------------------------------------------------------------------------------------------------
	static vector GetRandomHorizontalDirectionAngles()
	{
		return Vector(Math.RandomFloat(0, 360), 0, 0);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find a safe position. Leverages SCR_WorldTools.FindEmptyTerrainPosition
	Generally used for spawning players and vehicles within spawn areas.
	\param[out] safePos Variable filled with found position.
	\param centerPos Center of queried area. Height is irrelevant, only terrain positions are queried.
	\param radiusToSelectPointsWithin Radius within which a point will be randomly selected.
	\param radiusToCheckAroundInitiallySelectedPos Radius within which a safe position will be searched for if the initially selected point is not safe.
	\param xzPaddingRadius Radius of the cylinder that we are attempting to find a safe position for.
	\param yPadding Height of the cylinder that we are attempting to find a safe position for. 
	\return True if position was found
	*/
	static bool TryGetRandomSafePosWithinRadius(out vector safePos, vector centerPos, float radiusToSelectPointsWithin, float radiusToCheckAroundInitiallySelectedPos, float xzPaddingRadius = 0.5, float yPaddingDistance = 2)
	{					
		RandomGenerator gen = new RandomGenerator();
		vector posToCheck = gen.GenerateRandomPointInRadius(0, radiusToSelectPointsWithin, centerPos);
		
		// Calculate safe pos
		vector selectedPos;
		bool foundSafePos = SCR_WorldTools.FindEmptyTerrainPosition(selectedPos, posToCheck, radiusToCheckAroundInitiallySelectedPos, xzPaddingRadius, yPaddingDistance);

		// Check if selected position is underwater, too far above ground, or too close to player characters	
		bool isPosUnderWater = SCR_TerrainHelper.GetTerrainY(selectedPos, GetGame().GetWorld(), true) == 0;
		
		float maxAllowedHeightAboveGround = 2.0;
		bool isPosTooFarAboveGround = selectedPos[1] - SCR_TerrainHelper.GetTerrainY(selectedPos, GetGame().GetWorld(), true) > maxAllowedHeightAboveGround;
		
		float desiredDistanceFromPlayers = 50;
		bool posIsNearPlayers = nearPlayersToVector(selectedPos, desiredDistanceFromPlayers);
		
		int currentAttempts = 0, maxAttempts = 5;
		
		// If it is, try to find another empty position.
		while (isPosUnderWater || isPosTooFarAboveGround || posIsNearPlayers)
		{	
			// Generate new point
			posToCheck = gen.GenerateRandomPointInRadius(0, radiusToSelectPointsWithin, centerPos);
			foundSafePos = SCR_WorldTools.FindEmptyTerrainPosition(selectedPos, posToCheck, radiusToCheckAroundInitiallySelectedPos, xzPaddingRadius, yPaddingDistance);
			
			// Retry checks
			isPosUnderWater = SCR_TerrainHelper.GetTerrainY(selectedPos, GetGame().GetWorld(), true) == 0;
			isPosTooFarAboveGround = selectedPos[1] - SCR_TerrainHelper.GetTerrainY(selectedPos, GetGame().GetWorld(), true) > maxAllowedHeightAboveGround;
			posIsNearPlayers = nearPlayersToVector(selectedPos, desiredDistanceFromPlayers);
			
			// Break loop if too many attempts
			currentAttempts++;
			if (currentAttempts >= maxAttempts) return false;
		}
		
		safePos = selectedPos;
		return foundSafePos;
	}
	
	static bool nearPlayersToVector(vector v, int distance)
	{
		array<int> players = {};
		GetGame().GetPlayerManager().GetPlayers(players);
		foreach(int playerId : players)
		{
			IEntity pce = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			if (!pce) continue;
			if (vector.Distance(v, pce.GetOrigin()) <= distance) {
				Print("[WR_Utils]: Player found too close to spawned entity; aborting..", LogLevel.WARNING);
				return true;
			}
		}
		return false;
	}


	static ResourceName GetDefaultAmmo(ResourceName weaponResourceName)
	{
		ResourceName ammoResourceName;
		
		// Check map if we found the ammo for this gun already
		if (WeaponAmmoResourceNames)
		{
			ammoResourceName = WeaponAmmoResourceNames.Get(weaponResourceName);
			if (ammoResourceName) return ammoResourceName;
		}
		
		// If not in map, find the resource name
		// TODO: There's gotta be a more efficient way of getting a weapon's default/compatible magazines. Will figure it out later.
		Resource resource = Resource.Load(weaponResourceName);
		IEntity weaponEntity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld());
		WeaponComponent weaponComponent = WeaponComponent.Cast(weaponEntity.FindComponent(WeaponComponent));
		BaseMuzzleComponent muzzleComponent = weaponComponent.GetCurrentMuzzle();
		ammoResourceName = muzzleComponent.GetDefaultMagazineOrProjectileName();
		
		if (!ammoResourceName) return "";
		
		// Add found resource to map and return it
		if (!WeaponAmmoResourceNames)
			WeaponAmmoResourceNames = new map<ResourceName, ResourceName>();
		WeaponAmmoResourceNames.Set(weaponResourceName, ammoResourceName);
		
		return ammoResourceName;
	}
	
	static bool IsReloadableWeapon(ResourceName resourceName)
	{
		if (!Weapons)
		{
			// This map is simply for checking if a given resource name is a weapon. The int value is a throwaway.
			Weapons = new map<ResourceName, int>(); 

			array<ResourceName> weaponResourceNames = {};
			
			weaponResourceNames.InsertAll(WR_ResourceNamesWeighted.GetRifles().GetAllItems());
			weaponResourceNames.InsertAll(WR_ResourceNamesWeighted.GetMachineGuns().GetAllItems());
			weaponResourceNames.InsertAll(WR_ResourceNamesWeighted.GetSnipers().GetAllItems());
			weaponResourceNames.InsertAll(WR_ResourceNamesWeighted.GetHandguns().GetAllItems());
			weaponResourceNames.InsertAll(WR_ResourceNamesWeighted.GetLaunchers().GetAllItems());

			foreach (ResourceName rn : weaponResourceNames)
				Weapons.Insert(rn, 0);
		}

		if (!WeaponsNonReloadable)
		{
			// This map is simply for checking if a given resource name is a weapon. The int value is a throwaway.
			WeaponsNonReloadable = new map<ResourceName, int>(); 

			// TODO: generate this list dynamically or read it from a config
			array<ResourceName> nonReloadableWeaponResourceNames =
			{
				"{9C5C20FB0E01E64F}Prefabs/Weapons/Launchers/M72/Launcher_M72A3.et"
			};

			foreach (ResourceName rn : nonReloadableWeaponResourceNames)
				WeaponsNonReloadable.Insert(rn, 0);
		}
		
		return Weapons.Contains(resourceName) && !WeaponsNonReloadable.Contains(resourceName);
	}
	
	static WorldTimestamp TimestampNow()
	{
		return GetGame().GetWorld().GetTimestamp();
	}
	
	static EntitySpawnParams GetEntityWorldSpawnParams(vector position)
	{
		EntitySpawnParams params = new EntitySpawnParams();
		
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = position;
		
		return params;
	}
	
	static IEntity SpawnPrefabInWorld(ResourceName resourceName, vector position)
	{
		EntitySpawnParams spawnParams = GetEntityWorldSpawnParams(position);
		
		IEntity entity = GetGame().SpawnEntityPrefabEx(
			resourceName,
			false,
			GetGame().GetWorld(),
			spawnParams
		);
		
		return entity;
	}
	
	static void RandomlyRotateAndOrientEntity(IEntity ent)
	{
		vector randomDir = WR_Utils.GetRandomHorizontalDirectionAngles();
		ent.SetYawPitchRoll(randomDir);
			
		// Orient the object to terrain normal
		vector transform[4];
		ent.GetTransform(transform);
		SCR_TerrainHelper.OrientToTerrain(transform);
		ent.SetTransform(transform);
	}
	
	static bool RemoveAllItemsFromVehicle(IEntity vehicle)
	{
		// Get vehicle inventory components
		auto inventoryStorage = SCR_UniversalInventoryStorageComponent.Cast(vehicle.FindComponent(SCR_UniversalInventoryStorageComponent));
		auto inventoryStorageManager = SCR_VehicleInventoryStorageManagerComponent.Cast(vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
		
		if (!inventoryStorage || !inventoryStorageManager) return false;
			
		// Remove all items from inventory
		array<IEntity> currItems = {};
		inventoryStorageManager.GetItems(currItems);
		foreach (IEntity item : currItems)
			inventoryStorageManager.TryDeleteItem(item);
		
		return true;
	}
	
	static float MinutesToMilliseconds(float m)
	{
		float s = MinutesToSeconds(m);
		float ms = SecondsToMilliseconds(s);
		return ms;
	}
	
	static float MinutesToSeconds(float m)
	{
		return m * 60;
	}
	
	static float SecondsToMilliseconds(float s)
	{
		return s * 1000;
	}
}