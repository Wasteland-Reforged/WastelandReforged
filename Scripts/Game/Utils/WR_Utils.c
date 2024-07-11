class WR_Utils
{
	private static ref map<ResourceName, ResourceName> WeaponAmmoResourceNames;
	private static ref map<ResourceName, int> Weapons;
	
	//------------------------------------------------------------------------------------------------
	static vector GetRandomPointWithinCircle(vector center, float radius)
	{
		float randomDisplacement = Math.RandomFloat(0, radius);
		vector randomDirection = GetRandomHorizontalDirectionAngles().AnglesToVector();
		
		return center + randomDisplacement * randomDirection;
	}
	
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
	static bool TryGetSafePos(out vector safePos, vector centerPos, float radiusToSelectPointsWithin, float radiusToCheckAroundInitiallySelectedPos, float xzPaddingRadius = 0.5, float yPaddingDistance = 2)
	{					
		// TODO: The code in this function is very repetitive and needs to be refactored.
		
		vector posToCheck = WR_Utils.GetRandomPointWithinCircle(centerPos, radiusToSelectPointsWithin);
		
		// Calculate safe pos
		vector selectedPos;
		bool foundSafePos = SCR_WorldTools.FindEmptyTerrainPosition(selectedPos, posToCheck, radiusToCheckAroundInitiallySelectedPos, xzPaddingRadius, yPaddingDistance);

		// Check if selected position is underwater
		// TODO: This does not account for points in terrain that dip below sea level.
		// I'm not sure if it's possible to have terrain like that in Enfusion, but it
		// would be worth improving this logic if it is.
		bool isPosUnderWater = SCR_TerrainHelper.GetTerrainY(selectedPos, GetGame().GetWorld(), true) == 0;
		
		// If it is, try to find another empty position.
		while (isPosUnderWater)
		{
			posToCheck = WR_Utils.GetRandomPointWithinCircle(centerPos, radiusToSelectPointsWithin);
			foundSafePos = SCR_WorldTools.FindEmptyTerrainPosition(selectedPos, posToCheck, radiusToCheckAroundInitiallySelectedPos, xzPaddingRadius, yPaddingDistance);
			isPosUnderWater = SCR_TerrainHelper.GetTerrainY(selectedPos, GetGame().GetWorld(), true) == 0;
		}
		
		// If selected position is too far above ground, find another one
		float maxAllowedHeightAboveGround = 2.0;
		bool isPosTooFarAboveGround = selectedPos[1] - SCR_TerrainHelper.GetTerrainY(selectedPos, GetGame().GetWorld(), true) > maxAllowedHeightAboveGround;
		while (isPosUnderWater)
		{
			posToCheck = WR_Utils.GetRandomPointWithinCircle(centerPos, radiusToSelectPointsWithin);
			foundSafePos = SCR_WorldTools.FindEmptyTerrainPosition(selectedPos, posToCheck, radiusToCheckAroundInitiallySelectedPos, xzPaddingRadius, yPaddingDistance);
			isPosTooFarAboveGround = selectedPos[1] - SCR_TerrainHelper.GetTerrainY(selectedPos, GetGame().GetWorld(), true) > maxAllowedHeightAboveGround;
		}
		
		safePos = selectedPos;
		return foundSafePos;
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

	// TODO: unite the Weapons map with the WeaponAmmoResouceName map. If something is a weapon, it probably takes ammo, so just use the weapon-ammo map
	// TODO: also maybe upgrade this to check for IsReloadableWeapon instead of just IsWeapon. Then we can exclude things like bayonets and non-reloadable weapons like the M72 launcher
	static bool IsWeapon(ResourceName resourceName)
	{
		if (!Weapons)
		{
			Weapons = new map<ResourceName, int>(); // This map is simply for checking if a given resource name is a weapon. The int value is a throwaway.

			array<ResourceName> weaponResourceNames = {};
			
			weaponResourceNames.InsertAll(WR_ResourceNamesWeighted.GetRifles().GetAllItems());
			weaponResourceNames.InsertAll(WR_ResourceNamesWeighted.GetMachineGuns().GetAllItems());
			weaponResourceNames.InsertAll(WR_ResourceNamesWeighted.GetSnipers().GetAllItems());
			weaponResourceNames.InsertAll(WR_ResourceNamesWeighted.GetHandguns().GetAllItems());
			weaponResourceNames.InsertAll(WR_ResourceNamesWeighted.GetLaunchers().GetAllItems());

			foreach (ResourceName rn : weaponResourceNames)
				Weapons.Insert(rn, 0);
		}
		
		return Weapons.Contains(resourceName);
	}
}