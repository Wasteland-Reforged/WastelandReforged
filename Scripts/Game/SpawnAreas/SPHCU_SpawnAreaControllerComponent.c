class SPHCU_SpawnAreaControllerComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class SPHCU_SpawnAreaControllerComponent : SCR_BaseGameModeComponent
{
	override void OnGameModeStart()
	{
		SpawnVehiclesInSpawnAreas();
	}
	
	private void SpawnVehiclesInSpawnAreas()
	{
		Print("[WASTELAND] Spawning vehicles...");
		int totalVehiclesSpawned = 0;
	
		foreach (auto vehicleSpawnHandler : SPHCU_SpawnAreaVehicleSpawnHandlerComponent.VehicleSpawnHandlerComponents)
		{
			int successfulVehSpawnCount;
			vehicleSpawnHandler.SpawnVehicles(successfulVehSpawnCount);
			totalVehiclesSpawned += successfulVehSpawnCount;
		}
		
		Print("[WASTELAND] Spawned a total of " + totalVehiclesSpawned + " vehicle(s).");
	}
}