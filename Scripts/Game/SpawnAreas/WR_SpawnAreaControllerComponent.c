class WR_SpawnAreaControllerComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class WR_SpawnAreaControllerComponent : SCR_BaseGameModeComponent
{
	[Attribute(defvalue: "true", desc: "Spawn vehicles in spawn areas.")]
	protected bool spawnVehicles;
	protected IEntity _parent;
	
	override void OnPostInit(IEntity owner)
	{
		_parent = owner;
	}
	
	override void OnGameModeStart()
	{
		BaseRplComponent rplComponent = BaseRplComponent.Cast(_parent.FindComponent(BaseRplComponent));
		
		if (rplComponent.Role() == RplRole.Authority)
		{
			if (spawnVehicles)
				SpawnVehiclesInSpawnAreas();			
		}
	}
	
	private void SpawnVehiclesInSpawnAreas()
	{
		Print("[WASTELAND] Spawning vehicles...");
		int totalVehiclesSpawned = 0;
	
		foreach (auto vehicleSpawnHandler : WR_SpawnAreaVehicleSpawnHandlerComponent.VehicleSpawnHandlerComponents)
		{
			int successfulVehSpawnCount;
			vehicleSpawnHandler.SpawnVehicles(successfulVehSpawnCount);
			totalVehiclesSpawned += successfulVehSpawnCount;
		}
		
		Print("[WASTELAND] Spawned a total of " + totalVehiclesSpawned + " vehicle(s).");
	}
}