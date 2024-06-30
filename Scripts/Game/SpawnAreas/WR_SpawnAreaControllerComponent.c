class WR_SpawnAreaControllerComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class WR_SpawnAreaControllerComponent : SCR_BaseGameModeComponent
{
	[Attribute(defvalue: "true", desc: "Spawn vehicles in spawn areas.")]
	protected bool spawnVehicles;
	
	override void OnGameModeStart()
	{
		if (spawnVehicles)
			SpawnVehiclesInSpawnAreas();
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