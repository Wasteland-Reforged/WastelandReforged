class SPHCU_RandomSpawnConfig
{
	// TODO: upgrade this system to support weights for each prefab, so we can tune how often certain items/vehicles spawn
	
	array<string> GetRandomVehiclePrefabs()
	{
		array<string> list = {};
		
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
	
	static array<string> GetAllUaz469(PrefabFactionFlag factions = PrefabFactionFlag.Us | PrefabFactionFlag.Ussr | PrefabFactionFlag.Fia | PrefabFactionFlag.Civilian)
	{
		array<string> list = {};

		if (CheckFactionFlagsContain(factions, PrefabFactionFlag.Ussr))
		{
			list.Insert("{259EE7B78C51B624}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469.et");
			list.Insert("{99F1610551D54D17}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_base.et");
			list.Insert("{16A674FE31B0921C}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_uncovered.et");
		}

		if (CheckFactionFlagsContain(factions, PrefabFactionFlag.Fia))
		{
			list.Insert("{F7E9AA0C813EABDA}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_FIA.et");
			list.Insert("{E28501E93F8EFDC0}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_FIA_uncovered.et");
		}

		if (CheckFactionFlagsContain(factions, PrefabFactionFlag.Civilian))
		{
			list.Insert("{C8B16ABCA57679A2}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_blue.et");
			list.Insert("{D7C51A961BF675B2}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_brown.et");
			list.Insert("{B6D330E7E40FE7DD}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_forest.et");
			list.Insert("{5070B3749C7782D8}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_JZD.et");
			list.Insert("{BDFB2377B6B4ABBB}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_PTS.et");
			list.Insert("{A4D16A62B432451A}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_red.et");
			list.Insert("{870EB660433857D3}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_teal.et");
			list.Insert("{B2CC137FE424B129}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_VAK.et");
		}

		return list;
	}
	
	private static bool CheckFactionFlagsContain(PrefabFactionFlag input, PrefabFactionFlag toCheckIfIsContained)
	{
		return (input & toCheckIfIsContained) == toCheckIfIsContained;
	}
}

enum PrefabFactionFlag
{
	Us = 1,
	Ussr = 2,
	Fia = 4,
	Civilian = 8
}
