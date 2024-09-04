class WR_SpawnAreaControllerComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class WR_SpawnAreaControllerComponent : SCR_BaseGameModeComponent
{
	[Attribute(defvalue: "true", desc: "Spawn vehicles in spawn areas.")]
	protected bool spawnVehicles;
	
	[Attribute(defvalue: "true", desc: "Spawn roaming AI in spawn areas.")]
	protected bool spawnAI;
	
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
			
			if (spawnAI)
				SpawnAIInSpawnAreas();	
		}
	}
	
	private void SpawnVehiclesInSpawnAreas()
	{
		Print("[WASTELAND] WR_SpawnAreaControllerComponent: Spawning vehicles...");
		int totalVehiclesSpawned = 0;
	
		foreach (auto vehicleSpawnHandler : WR_SpawnAreaVehicleSpawnHandlerComponent.VehicleSpawnHandlerComponents)
		{
			int successfulVehSpawnCount;
			vehicleSpawnHandler.SpawnInitialVehicles(successfulVehSpawnCount);
			totalVehiclesSpawned += successfulVehSpawnCount;
		}
		
		Print("[WASTELAND] WR_SpawnAreaControllerComponent: Spawned a total of " + totalVehiclesSpawned + " vehicle(s).");
	}
	
	private void SpawnAIInSpawnAreas()
	{
		Print("[WASTELAND] WR_SpawnAreaControllerComponent: Spawning AI...");
		int totalAIGroupsSpawned = 0;
	
		foreach (auto AISpawnHandler : WR_SpawnAreaAISpawnHandlerComponent.AISpawnHandlerComponents)
		{
			int successfulAIGroupSpawnCount;
			AISpawnHandler.SpawnInitialAIGroups(successfulAIGroupSpawnCount);
			totalAIGroupsSpawned += successfulAIGroupSpawnCount;
		}
		
		Print("[WASTELAND] WR_SpawnAreaControllerComponent: Spawned a total of " + totalAIGroupsSpawned + " roaming bot groups.");
	}
}