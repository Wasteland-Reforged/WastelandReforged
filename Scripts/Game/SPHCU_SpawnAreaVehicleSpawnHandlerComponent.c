[ComponentEditorProps(category: "World Vehicle Spawning", description: "Handles the random spawning of vehicles in Towns on server restart")]
class SPHCU_SpawnAreaVehicleSpawnHandlerComponentClass : ScriptComponentClass
{
	
}

class SPHCU_SpawnAreaVehicleSpawnHandlerComponent : ScriptComponent
{
	private SPHCU_SpawnAreaEntity _parent;
	
	ref static array<SPHCU_SpawnAreaVehicleSpawnHandlerComponent> VehicleSpawnHandlerComponents;

	[Attribute(defvalue: "50", desc: "Number of vehicles to spawn per square kilometer of surface area inside this spawn area.")]
	protected int vehiclesPerSqKm;
	
	[Attribute(defvalue: "4", desc: "Number of vehicles guaranteed to spawn inside this spawn area. Overrides vehiclesPerSqKm if it is less than this value.")]
	protected int vehiclesFlatRate;
	
	override void OnPostInit(IEntity owner)
	{
		_parent = SPHCU_SpawnAreaEntity.Cast(owner);
		if (!_parent)
		{
			Print("[WASTELAND] Parent entity of SPHCU_SpawnAreaVehicleSpawnHandlerComponent must be a SPHCU_SpawnAreaEntity!");
			return;
		}
		
		if (!VehicleSpawnHandlerComponents)
		{
			VehicleSpawnHandlerComponents = {};
			Print("[WASTELAND] Initialized vehicle spawn area handler component list.");
		}
		
		VehicleSpawnHandlerComponents.Insert(this);
		Print("[WASTELAND] Inserted " + GetSpawnAreaName() + " into the vehicle spawn handler component list");
	}
	
	void SpawnVehicles(out int successfulVehSpawnCount, array<string> vehiclePrefabs)
	{
		if (!vehiclePrefabs || vehiclePrefabs.Count() == 0)
		{
			Print("[WASTELAND] No prefabs were supplied! Please provide at least one prefab.");
			return;
		}
		
		int desiredVehCount = GetVehicleCountPerSqKm();
		if (desiredVehCount < vehiclesFlatRate) desiredVehCount = vehiclesFlatRate;
		
		successfulVehSpawnCount = 0;
		
		// Configure spawn position parameters
		float areaToCheck = 100; 		// Radius that will be checked if the initially passed pos is not safe
		float xzPaddingRadius = 15;		// Minimum radius of empty space to have around the chosen position
		float yPaddingDistance = 10; 	// Minimum distance of empty space to have above and below the chosen position 

		for (int i = 0; i < desiredVehCount; i++)
		{
			// Select a random position				
			vector spawnPos;
			bool foundSafePos = SPHCU_Utils.TryGetSafePos(
												spawnPos
												, _parent.GetOrigin()
												, _parent.GetSphereRadius()
												, areaToCheck
												, xzPaddingRadius
												, yPaddingDistance);
			if (!foundSafePos) return;
			
			// Spawn the vehicle
			string prefab = vehiclePrefabs.GetRandomElement();
			Resource resource = Resource.Load(prefab);
			
			EntitySpawnParams spawnParams = new EntitySpawnParams();
			spawnParams.Transform[3] = spawnPos;
			
			IEntity vehicle = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), spawnParams);
			vehicle.SetYawPitchRoll(SPHCU_Utils.GetRandomHorizontalDirectionAngles());

			successfulVehSpawnCount++;
		}
		
		Print("[WASTELAND] Successfully spawned " + successfulVehSpawnCount + " vehicle(s) of " + desiredVehCount + " attempted at " + _parent.GetSpawnAreaName());
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
	
	void ~SPHCU_SpawnAreaVehicleSpawnHandlerComponent()
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