[ComponentEditorProps(category: "World Vehicle Spawning", description: "Handles the random spawning of vehicles in Towns on server restart")]
class WR_SpawnAreaVehicleSpawnHandlerComponentClass : ScriptComponentClass
{
	
}

class WR_SpawnAreaVehicleSpawnHandlerComponent : ScriptComponent
{
	private WR_SpawnAreaEntity _parent;
	
	ref static array<WR_SpawnAreaVehicleSpawnHandlerComponent> VehicleSpawnHandlerComponents;

	[Attribute(defvalue: "50", desc: "Number of vehicles to spawn per square kilometer of surface area inside this spawn area.")]
	protected int vehiclesPerSqKm;
	
	[Attribute(defvalue: "7", desc: "Number of vehicles guaranteed to spawn inside this spawn area. Overrides vehiclesPerSqKm if it is less than this value.")]
	protected int vehiclesFlatRate;
	
	override void OnPostInit(IEntity owner)
	{
		_parent = WR_SpawnAreaEntity.Cast(owner);
		if (!_parent)
		{
			Print("[WASTELAND] WR_SpawnAreaVehicleSpawnHandlerComponent: Parent entity of WR_SpawnAreaVehicleSpawnHandlerComponent must be a WR_SpawnAreaEntity!", LogLevel.ERROR);
			return;
		}
		
		if (!VehicleSpawnHandlerComponents)
		{
			VehicleSpawnHandlerComponents = {};
			Print("[WASTELAND] WR_SpawnAreaVehicleSpawnHandlerComponent: Initialized vehicle spawn area handler component list.", LogLevel.NORMAL);
		}
		
		VehicleSpawnHandlerComponents.Insert(this);
		Print("[WASTELAND] WR_SpawnAreaVehicleSpawnHandlerComponent: Inserted " + GetSpawnAreaName() + " into the vehicle spawn handler component list", LogLevel.SPAM);
	}
	
	void SpawnVehicles(out int successfulVehSpawnCount)
	{
		WR_WeightedItemArray<ResourceName> vehicleResourceNames = WR_ResourceNamesWeightedOld.GetSpawnAreaVehicles();
		
		if (!vehicleResourceNames || vehicleResourceNames.Count() == 0)
		{
			Print("[WASTELAND] WR_SpawnAreaVehicleSpawnHandlerComponent: No spawn area vehicle resource names were supplied! Please provide at least one resource name.");
			return;
		}
		
		int desiredVehCount = GetVehicleCountPerSqKm();
		if (desiredVehCount < vehiclesFlatRate) desiredVehCount = vehiclesFlatRate;
		
		successfulVehSpawnCount = 0;
		
		// Configure spawn position parameters
		float areaToCheck = 100; 		// Radius that will be checked if the initially passed pos is not safe
		float xzPaddingRadius = 3;		// Minimum radius of empty space to have around the chosen position
		float yPaddingDistance = 10; 		// Minimum distance of empty space to have above and below the chosen position 

		// Get loot spawning context
		WR_LootSpawnContext lootContext = WR_LootSpawnContextPresets.GetLootContextByType(PAND_LootContextType.RANDOM_VEHICLE);
		int minItems = 2; // TODO: add validation and make these read from a global config
		int maxItems = 6;
		
		for (int i = 0; i < desiredVehCount; i++)
		{
			// Select a random position				
			vector spawnPos;
			bool foundSafePos = WR_Utils.TryGetRandomSafePosWithinRadius(
												spawnPos
												, _parent.GetOrigin()
												, _parent.GetSphereRadius()
												, areaToCheck
												, xzPaddingRadius
												, yPaddingDistance);
			if (!foundSafePos) continue;

			// Spawn the vehicle
			ResourceName vehResourceName = vehicleResourceNames.GetRandomItem();
			
			EntitySpawnParams spawnParams = new EntitySpawnParams();
			spawnParams.Transform[3] = spawnPos; // Transform[3] is position in world
			
			IEntity vehicle = GetGame().SpawnEntityPrefab(vehResourceName, false, GetGame().GetWorld(), spawnParams);
			if (!vehicle) continue;
			
			vehicle.SetYawPitchRoll(WR_Utils.GetRandomHorizontalDirectionAngles());
			
			//Remove intial items
			if (!WR_Utils.removeAllItemsFromVehicle(vehicle))
			{
				Print("[WASTELAND] WR_SpawnAreaVehicleSpawnHandlerComponent: Could not remove initial items from vehicle");
			}
			
			auto inventoryStorage = SCR_UniversalInventoryStorageComponent.Cast(vehicle.FindComponent(SCR_UniversalInventoryStorageComponent));
			auto inventoryStorageManager = SCR_VehicleInventoryStorageManagerComponent.Cast(vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
			
			// Place weapons and items in inventory			
			array<ResourceName> itemResourceNamesToSpawn = lootContext.GetRandomItems(Math.RandomIntInclusive(minItems, maxItems));
			foreach (ResourceName name : itemResourceNamesToSpawn)
				inventoryStorageManager.TrySpawnPrefabToStorage(name, inventoryStorage);

			successfulVehSpawnCount++;
		}
		
		Print("[WASTELAND] WR_SpawnAreaVehicleSpawnHandlerComponent: Successfully spawned " + successfulVehSpawnCount + " vehicle(s) of " + desiredVehCount + " attempted at " + _parent.GetSpawnAreaName(), LogLevel.SPAM);
	}

	private int GetVehicleCountPerSqKm()
	{
		// Calculate the surface area of the spawn
		float spawnAreaRadius = _parent.GetSphereRadius();
		float areaSqKm = (Math.PI * spawnAreaRadius * spawnAreaRadius) / (1000 * 1000);

		return Math.Floor(areaSqKm * vehiclesPerSqKm);
	}
	
	string GetSpawnAreaName()
	{
		return _parent.GetSpawnAreaName();
	}
	
	void ~WR_SpawnAreaVehicleSpawnHandlerComponent()
	{
		if (!VehicleSpawnHandlerComponents) return;
		
		if (VehicleSpawnHandlerComponents.Count() == 0)
		{
			VehicleSpawnHandlerComponents = null;
			return;
		}
		
		VehicleSpawnHandlerComponents.RemoveItem(this);
	}
}