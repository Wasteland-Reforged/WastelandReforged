class WR_Utils
{
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
		vector randomPoint, selectedPos;
		
		bool posUnderwater, posTooHigh, posNearPlayers = false;
		const int MAX_ATTEMPTS = 10;
		const float MAX_HEIGHT_ABOVE_GROUND = 2.0;
		const float MIN_DISTANCE_FROM_PLAYERS = 50.0;
		
		for (int i = 0; i < MAX_ATTEMPTS; i++)
		{
			// Generate new point
			randomPoint = gen.GenerateRandomPointInRadius(0, radiusToSelectPointsWithin, centerPos);
			if (!SCR_WorldTools.FindEmptyTerrainPosition(selectedPos, randomPoint, radiusToCheckAroundInitiallySelectedPos, xzPaddingRadius, yPaddingDistance))
				continue;
			
			// Retry checks
			posUnderwater = SCR_TerrainHelper.GetTerrainY(selectedPos, GetGame().GetWorld(), true) == 0;
			posTooHigh = selectedPos[1] - SCR_TerrainHelper.GetTerrainY(selectedPos, GetGame().GetWorld(), true) > MAX_HEIGHT_ABOVE_GROUND;
			posNearPlayers = nearPlayersToVector(selectedPos, MIN_DISTANCE_FROM_PLAYERS);
			
			if (!posUnderwater && !posTooHigh && !posNearPlayers)
			{
				safePos = selectedPos;
				return true;
			}
		}

		return false;
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
			true,
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
		
		if (!inventoryStorage || !inventoryStorageManager)
			return false;
			
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