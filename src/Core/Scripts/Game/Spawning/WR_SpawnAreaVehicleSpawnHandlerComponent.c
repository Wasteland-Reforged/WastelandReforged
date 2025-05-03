[ComponentEditorProps(category: "World Vehicle Spawning", description: "Handles the random spawning of vehicles in Towns on server restart")]
class WR_SpawnAreaVehicleSpawnHandlerComponentClass : ScriptComponentClass {}

class WR_SpawnAreaVehicleSpawnHandlerComponent : ScriptComponent
{
	private WR_SpawnAreaEntity _parent;
	ref WR_Logger<WR_SpawnAreaVehicleSpawnHandlerComponent> logger = new WR_Logger<WR_SpawnAreaVehicleSpawnHandlerComponent>(this);

	[Attribute(defvalue: "50", desc: "Number of vehicles to spawn per square kilometer of surface area inside this spawn area.")]
	protected int vehiclesPerSqKm;
	
	[Attribute(defvalue: "7", desc: "Number of vehicles guaranteed to spawn inside this spawn area. Overrides vehiclesPerSqKm if it is less than this value.")]
	protected int vehiclesFlatRate;
	
	[Attribute(defvalue: "1.0", desc: "Chance for vehicle to spawn with a random amount of supplies")]
	protected float vehiclesSupplyChance;
	
	[Attribute(defvalue: "1", desc: "Amount of minutes to wait before checking if vehicles need respawning")]
	protected float vehiclesRespawnTimer;
	
	[Attribute(defvalue: "10", desc: "Amount of supplies to round to when adding supplies to town vehicles")]
	protected int vehicleSupplyStepSize;
	
	[Attribute(defvalue: "0.2", uiwidget: UIWidgets.Slider, params: "0 1.0 0.1", desc: "Amount of loot to spawn in the vehicle.")]
	float m_fLootBudget;
	
	protected int _currentVehicles = 0;
	
	override void OnPostInit(IEntity owner)
	{
		_parent = WR_SpawnAreaEntity.Cast(owner);
		if (!_parent)
		{
			logger.LogError("Parent entity of WR_SpawnAreaVehicleSpawnHandlerComponent must be a WR_SpawnAreaEntity!");
			return;
		}
		
		WR_TownVehicleSystem.InsertVehicleSpawnHandlerComponent(this);
		logger.LogDebug(string.Format("Inserted %1 into the vehicle spawn handler component list.", GetSpawnAreaName()));

		logger.LogDebug("Initialized.");
	}
	
	bool SpawnTownVehicle()
	{
		// Get Random Vehicle Resource Name
		WR_LootSpawningComponent lootSpawnComp = WR_LootSpawningComponent.GetInstance();
		if (!lootSpawnComp)
			return false;
		
		// Grab a vehicle and any additional items it may have
		WR_LootSpawningComponent lootSpawningComponent = WR_LootSpawningComponent.GetInstance();
		array<ResourceName> vehicleResources = {};
		lootSpawningComponent.GetRandomItemsFromCategory(vehicleResources, WR_LootCategory.SpawnAreaVehicles);
		if (!vehicleResources || vehicleResources.Count() == 0)
		{
			logger.LogError("No vehicle resource names were returned from WR_LootSpawningComponent!");
			return false;
		}
		
		// Configure spawn position parameters
		const float areaToCheck = 100; 		// Radius that will be checked if the initially passed pos is not safe
		const float xzPaddingRadius = 3;		// Minimum radius of empty space to have around the chosen position
		const float yPaddingDistance = 10; 		// Minimum distance of empty space to have above and below the chosen position 
		
		// Select a random position				
		vector spawnPos;
		bool foundSafePos = WR_Utils.TryGetRandomSafePosWithinRadius(spawnPos, _parent.GetOrigin(), _parent.GetSphereRadius(), areaToCheck
																	, xzPaddingRadius, yPaddingDistance);
		
		if (!foundSafePos)
			return false;
		
		// Spawn and orient the vehicle	
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.Transform[3] = spawnPos; // Transform[3] is position in world
		
		ResourceName vehicleResource = vehicleResources[0];
		IEntity vehicleEnt = GetGame().SpawnEntityPrefabEx(vehicleResource, false, GetGame().GetWorld(), spawnParams);
		if (!vehicleEnt)
			return false;
		
		vehicleEnt.SetYawPitchRoll(WR_Utils.GetRandomHorizontalDirectionAngles());
		
		SpawnVehicleLoot(vehicleEnt, vehicleResources);
		
		// Roll chance to spawn with supplies. If successful, fill vehicle with random amount of supplies
		if (Math.RandomFloat01() <= vehiclesSupplyChance) 
		{
			auto supplyStorage = SCR_ResourceComponent.Cast(vehicleEnt.FindComponent(SCR_ResourceComponent));
				
			if (supplyStorage && supplyStorage.GetContainers())
			{
				foreach (SCR_ResourceContainer suppContainer : supplyStorage.GetContainers())
				{
					int maxSteps = suppContainer.GetMaxResourceValue() / vehicleSupplyStepSize;
					int supplyToAdd = Math.RandomIntInclusive(1, maxSteps) * vehicleSupplyStepSize;
					suppContainer.IncreaseResourceValue(supplyToAdd);
				}
			}
			else
			{
				logger.LogWarning(string.Format("Could not find supply storage for town vehicle. Resource name: %1", vehicleResource));
			}
		}
		
		// Add function call to remove vehicle from current count when deleted
		SCR_AIVehicleUsageComponent vehicleUsageComp = SCR_AIVehicleUsageComponent.FindOnNearestParent(vehicleEnt, vehicleEnt);
		if (!vehicleUsageComp)
		{
			SCR_AIVehicleUsageComponent.ErrorNoComponent(vehicleEnt);
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
	
	protected void SpawnVehicleLoot(IEntity vehicle, array<ResourceName> additionalItems)
	{
		// Remove initial items
		if (!WR_Utils.RemoveAllItemsFromVehicle(vehicle))
		{
			logger.LogError("Could not remove initial items from vehicle.");
		}
		
		SCR_UniversalInventoryStorageComponent inventoryStorage = SCR_UniversalInventoryStorageComponent.Cast(vehicle.FindComponent(SCR_UniversalInventoryStorageComponent));
		SCR_VehicleInventoryStorageManagerComponent inventoryStorageManager = SCR_VehicleInventoryStorageManagerComponent.Cast(vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
		
		WR_LootSpawningComponent lootSpawnComp = WR_LootSpawningComponent.GetInstance();
		if (!lootSpawnComp)
			return;
		
		// Spawn any additional items
		if (additionalItems && additionalItems.Count() > 0)
		{
			for (int i = 1; i < additionalItems.Count(); i++)
			{
				if (!inventoryStorageManager.TrySpawnPrefabToStorage(additionalItems[i], inventoryStorage))		
					return;	// Vehicle is full
			}
		}
		
		array<ResourceName> itemResourceNamesToSpawn = lootSpawnComp.GetRandomItemsByBudget(WR_LootContext.RANDOM_VEHICLE, m_fLootBudget);
		
		foreach (ResourceName name : itemResourceNamesToSpawn) 
		{
			if (!inventoryStorageManager.TrySpawnPrefabToStorage(name, inventoryStorage))		
				return;	// Vehicle is full
		}
	}
	
	void CheckVehicles()
	{
		const int maxAllowedFailures = 10;
		int failures = 0;
		
		while (_currentVehicles < GetMaxTownVehicles() && failures < maxAllowedFailures)
		{
			if (!SpawnTownVehicle())
				failures++;
		}
	}

	private int GetVehicleCountPerSqKm()
	{
		// Calculate the surface area of the spawn
		float spawnAreaRadius = _parent.GetSphereRadius();
		float areaSqKm = (Math.PI * spawnAreaRadius * spawnAreaRadius) / (1000 * 1000);

		return Math.Floor(areaSqKm * vehiclesPerSqKm);
	}
	
	private int GetMaxTownVehicles()
	{
		return Math.Max(GetVehicleCountPerSqKm(), vehiclesFlatRate);
	}
	
	string GetSpawnAreaName()
	{
		return _parent.GetSpawnAreaName();
	}
	
	void ~WR_SpawnAreaVehicleSpawnHandlerComponent()
	{
		WR_TownVehicleSystem.RemoveVehicleSpawnHandlerComponent(this);
	}
}