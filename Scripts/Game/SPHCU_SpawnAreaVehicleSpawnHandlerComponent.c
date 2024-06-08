[ComponentEditorProps(category: "World Vehicle Spawning", description: "Handles the random spawning of vehicles in Towns on server restart")]
class SPHCU_SpawnAreaVehicleSpawnHandlerComponentClass : ScriptComponentClass
{
	
}

class SPHCU_SpawnAreaVehicleSpawnHandlerComponent : ScriptComponent
{
	private SPHCU_SpawnAreaEntity _parent;

	[Attribute(defvalue: "20", desc: "Number of vehicles to spawn per square kilometer of surface area inside this spawn area.")]
	protected int vehiclesPerSqKm;
	
//	override void EOnActivate(IEntity owner)
//	{
//		_parent = SPHCU_SpawnAreaEntity.Cast(owner);
//		if (!_parent)
//		{
//			Print("Parent entity of SPHCU_SpawnAreaVehicleSpawnHandlerComponent must be a SPHCU_SpawnAreaEntity!");
//			return;
//		}
//		
//		SpawnVehicles();
//		Print("Spawned " + GetVehicleCountPerSqKm() + " vehicles at " + _parent.GetSpawnAreaName());
//	}
	
	override void EOnInit(IEntity owner)
	{
		_parent = SPHCU_SpawnAreaEntity.Cast(owner);
		if (!_parent)
		{
			Print("Parent entity of SPHCU_SpawnAreaVehicleSpawnHandlerComponent must be a SPHCU_SpawnAreaEntity!");
			return;
		}
		
		SpawnVehicles();
		Print("Spawned " + GetVehicleCountPerSqKm() + " vehicles at " + _parent.GetSpawnAreaName());
	}
	
	void SpawnVehicles()
	{
		int vehCount = GetVehicleCountPerSqKm();
		for (int i = 0; i < vehCount; i++)
		{
			// Select a random position				
			vector spawnPos;
			bool foundSafePos = TryGetSafePos(spawnPos, this);
		
			// Spawn the vehicle
			string vicPrefab = GetVehiclePrefabs().GetRandomElement();
			Resource vicResource = Resource.Load(vicPrefab);
			EntitySpawnParams vicParams = new EntitySpawnParams();
			vicParams.Transform[3] = spawnPos;
			
			IEntity newVic = GetGame().SpawnEntityPrefab(vicResource, GetGame().GetWorld(), vicParams);
		}
	}

	private int GetVehicleCountPerSqKm()
	{
		// Calculate the surface area of the spawn
		Print(_parent);
		float spawnAreaRadius = _parent.GetSphereRadius();
		float areaSqKm = (Math.PI * spawnAreaRadius * spawnAreaRadius) / (1000 * 1000);

		return (int)Math.Floor(areaSqKm * vehiclesPerSqKm);
	}

	private array<string> GetVehiclePrefabs()
	{
		array<string> list = {};
		
		list.Insert("{DD774A8FD0989A78}Prefabs/Vehicles/Wheeled/M998/M1025_armed_M2HB_MERDC.et");
		list.Insert("{F649585ABB3706C4}Prefabs/Vehicles/Wheeled/M151A2/M151A2.et");
		list.Insert("{47D94E1193A88497}Prefabs/Vehicles/Wheeled/M151A2/M151A2_transport.et");
		list.Insert("{81FDAD5EB644CC3D}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport_covered.et");
		list.Insert("{27E2E58E734A80EC}Prefabs/Vehicles/Wheeled/M998/M1025_MERDC.et");
		list.Insert("{5674FAEB9AB7BDD0}Prefabs/Vehicles/Wheeled/M998/M998.et");
		list.Insert("{00C9BBE426F7D459}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance.et");
		list.Insert("{54C3CC22DEBD57BE}Prefabs/Vehicles/Wheeled/S105/S105_beige.et");
		list.Insert("{85F0BD6AF0F42D6D}Prefabs/Vehicles/Wheeled/S105/S105_red.et");
		list.Insert("{5555DCAFADB8CE2A}Prefabs/Vehicles/Wheeled/S105/S105_rally.et");
		list.Insert("{ACED839A6C5BE657}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_brown.et");
		list.Insert("{F77C41245A580FD1}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_blue.et");
		list.Insert("{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et");
		list.Insert("{259EE7B78C51B624}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469.et");
		list.Insert("{99F1610551D54D17}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_base.et");
		list.Insert("{FB219B49A448A8EA}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport_covered_CIV_JZD.et");

		return list;
	}
	
	private static bool TryGetSafePos(out vector safePos, SPHCU_SpawnAreaVehicleSpawnHandlerComponent spawnArea)
	{
		// Find a random safe position within the spawn area to spawn the vehicle at
		vector spawnAreaCenterPos = spawnArea._parent.GetOrigin();
		float spawnAreaRadius = spawnArea._parent.GetSphereRadius();
		
		vector randomDir = Vector(Math.RandomFloat(0, 360), 0, 0).AnglesToVector();
		float randomDist = Math.RandomFloat(0, spawnAreaRadius);
		
		float areaToCheck = 25;
		vector posToCheck = spawnAreaCenterPos + randomDir * randomDist;
		
		return SCR_WorldTools.FindEmptyTerrainPosition(safePos, posToCheck, areaToCheck);
	}
}