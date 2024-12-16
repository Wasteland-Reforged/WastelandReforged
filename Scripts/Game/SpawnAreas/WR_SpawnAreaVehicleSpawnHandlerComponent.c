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
	
	[Attribute(defvalue: "0.75", desc: "Chance for vehicle to spawn with a random amount of supplies")]
	protected float vehiclesSupplyChance;
	
	[Attribute(defvalue: "1", desc: "Amount of minutes to wait before checking if vehicles need respawning")]
	protected float vehiclesRespawnTimer;
	
	[Attribute(defvalue: "10", desc: "Amount of supplies to round to when adding supplies to town vehicles")]
	protected int vehicleSupplyStepSize;
	
	protected int _currentVehicles = 0;
	protected int _desiredVehCount = 0;
	
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
	
	bool SpawnTownVehicle()
	{
		// Get Random Vehicle Resource Name
		WR_LootSpawningComponent lootSpawnComp = WR_LootSpawningComponent.GetInstance();
		if (!lootSpawnComp)
			return false;
		
		ResourceName vehResourceName = lootSpawnComp.GetRandomSingleItemFromCategory(WR_LootCategory.SpawnAreaVehicles);
		
		// Configure spawn position parameters
		float areaToCheck = 100; 		// Radius that will be checked if the initially passed pos is not safe
		float xzPaddingRadius = 3;		// Minimum radius of empty space to have around the chosen position
		float yPaddingDistance = 10; 		// Minimum distance of empty space to have above and below the chosen position 
		
		// Select a random position				
		vector spawnPos;
		bool foundSafePos = WR_Utils.TryGetRandomSafePosWithinRadius(spawnPos, _parent.GetOrigin(), _parent.GetSphereRadius(), areaToCheck
																	, xzPaddingRadius, yPaddingDistance);
		
		if (!foundSafePos) return false;
		
		// Spawn and orient the vehicle	
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.Transform[3] = spawnPos; // Transform[3] is position in world
					
		IEntity vehicle = GetGame().SpawnEntityPrefabEx(vehResourceName, false, GetGame().GetWorld(), spawnParams);
		if (!vehicle)
			return false;
		
		vehicle.SetYawPitchRoll(WR_Utils.GetRandomHorizontalDirectionAngles());
		
		SpawnVehicleLoot(vehicle);
		
		// Roll chance to spawn with supplies. If successful, fill vehicle with random amount of supplies
		if (Math.RandomFloat01() <= vehiclesSupplyChance) 
		{
			auto supplyStorage = SCR_ResourceComponent.Cast(vehicle.FindComponent(SCR_ResourceComponent));
				
			if (supplyStorage && supplyStorage.GetContainers()) {
				foreach (SCR_ResourceContainer suppContainer : supplyStorage.GetContainers()) {
					int maxSteps = suppContainer.GetMaxResourceValue()/vehicleSupplyStepSize;
					int supplyToAdd = Math.RandomIntInclusive(2, maxSteps) * vehicleSupplyStepSize;
					suppContainer.IncreaseResourceValue(supplyToAdd);
				}
			}
			else {
				Print("[WASTELAND] WR_SpawnAreaVehicleSpawnHandlerComponent: Could not find supply storage for town vehicle");
			}
		}
		
		// Add function call to remove vehicle from current count when deleted
		SCR_AIVehicleUsageComponent vehicleUsageComp = SCR_AIVehicleUsageComponent.FindOnNearestParent(vehicle, vehicle);
		if (!vehicleUsageComp)
		{
			SCR_AIVehicleUsageComponent.ErrorNoComponent(vehicle);
			return false;
		}
		vehicleUsageComp.GetOnDeleted().Insert(OnVehicleDeleted);
		
		_currentVehicles++;
		return true;
	}
	
	protected void OnVehicleDeleted(SCR_AIVehicleUsageComponent comp)
	{
		_currentVehicles--;
	}
	
	protected void RespawnVehicleLoot(IEntity item, BaseInventoryStorageComponent storageOwner)
	{
		IEntity vehicle = storageOwner.GetOwner();
		
	}
	
	protected void SpawnVehicleLoot(IEntity vehicle)
	{
		//Remove initial items
		if (!WR_Utils.RemoveAllItemsFromVehicle(vehicle))
		{
			Print("[WASTELAND] WR_SpawnAreaVehicleSpawnHandlerComponent: Could not remove initial items from vehicle");
		}
		
		SCR_UniversalInventoryStorageComponent inventoryStorage = SCR_UniversalInventoryStorageComponent.Cast(vehicle.FindComponent(SCR_UniversalInventoryStorageComponent));
		SCR_VehicleInventoryStorageManagerComponent inventoryStorageManager = SCR_VehicleInventoryStorageManagerComponent.Cast(vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
		
		// Get loot spawning context
		WR_LootSpawnContext lootContext = WR_LootSpawnContextPresets.GetLootContextByType(WR_LootContext.VEHICLE_LOOT);
		int minItems = 2; // TODO: add validation and make these read from a global config
		int maxItems = 6;
		
		array<ResourceName> itemResourceNamesToSpawn = lootContext.GetRandomItems(Math.RandomIntInclusive(minItems, maxItems), minExtraMags: 0, maxExtraMags: 4);
		foreach (ResourceName name : itemResourceNamesToSpawn)
			inventoryStorageManager.TrySpawnPrefabToStorage(name, inventoryStorage);
	}
	
	void SpawnInitialVehicles(out int successfulVehSpawnCount)
	{
		// Initialize max/current vehicle counts
		_desiredVehCount = getMaxTownVehicles();
		successfulVehSpawnCount = 0;
		
		// Loop through all vehicles that need to be spawned
		for (int i = 0; i < _desiredVehCount; i++)
		{
			if (!SpawnTownVehicle()) {
				Print("[WASTELAND] WR_SpawnAreaVehicleSpawnHandlerComponent: Vehicle was not spawned", LogLevel.SPAM);
			}
			else {
				successfulVehSpawnCount++;
			}
		}
		
		//Attempt to respawn town vehicles every X minutes
		GetGame().GetCallqueue().CallLater(checkVehicles, vehiclesRespawnTimer * 60 * 1000, true);
			
		Print("[WASTELAND] WR_SpawnAreaVehicleSpawnHandlerComponent: Successfully spawned " + successfulVehSpawnCount + " vehicle(s) of " + _desiredVehCount + " attempted at " + _parent.GetSpawnAreaName(), LogLevel.SPAM);
	}
	
	private void checkVehicles()
	{
		while (_currentVehicles < _desiredVehCount) {
			SpawnTownVehicle();
		}
	}

	private int GetVehicleCountPerSqKm()
	{
		// Calculate the surface area of the spawn
		float spawnAreaRadius = _parent.GetSphereRadius();
		float areaSqKm = (Math.PI * spawnAreaRadius * spawnAreaRadius) / (1000 * 1000);

		return Math.Floor(areaSqKm * vehiclesPerSqKm);
	}
	
	private int getMaxTownVehicles()
	{
		return Math.Max(GetVehicleCountPerSqKm(), vehiclesFlatRate);
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