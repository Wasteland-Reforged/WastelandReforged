class SPHCU_SpawnAreaControllerComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class SPHCU_SpawnAreaControllerComponent : SCR_BaseGameModeComponent
{
	override void OnGameModeStart()
	{
		Print("[WASTELAND] Game started!");
		SpawnVehiclesInSpawnAreas();
	}
	
	private void SpawnVehiclesInSpawnAreas()
	{
		Print("[WASTELAND] Spawning vehicles...");
		int totalVehiclesSpawned = 0;
		
		array<string> vehiclePrefabs = SPHCU_RandomSpawnConfig.GetAllUaz469(PrefabFactionFlag.Ussr | PrefabFactionFlag.Fia | PrefabFactionFlag.Civilian);
		
		foreach (auto vehicleSpawnHandler : SPHCU_SpawnAreaVehicleSpawnHandlerComponent.VehicleSpawnHandlerComponents)
		{
			int successfulVehSpawnCount;
			vehicleSpawnHandler.SpawnVehicles(successfulVehSpawnCount, vehiclePrefabs);
			totalVehiclesSpawned += successfulVehSpawnCount;
		}
		
		Print("[WASTELAND] Spawned a total of " + totalVehiclesSpawned + " vehicle(s).");
	}
}