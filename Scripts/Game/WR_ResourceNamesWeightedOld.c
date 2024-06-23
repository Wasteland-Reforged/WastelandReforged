class WR_ResourceNamesWeightedOld
{
    private static ref WR_WeightedItemArray<ResourceName> SpawnAreaWeaponsWeighted;
    private static ref WR_WeightedItemArray<ResourceName> SpawnAreaItemsWeighted;
	private static ref WR_WeightedItemArray<ResourceName> SpawnAreaVehiclesWeighted;
	private static ref WR_WeightedItemArray<ResourceName> CaptureVehiclesWeighted;
	
	static WR_WeightedItemArray<ResourceName> GetSpawnAreaWeapons()
	{
		if (SpawnAreaWeaponsWeighted) return SpawnAreaWeaponsWeighted;
		SpawnAreaWeaponsWeighted = new WR_WeightedItemArray<ResourceName>();

		SpawnAreaWeaponsWeighted.AddItem(1.0, "{3E413771E1834D2F}Prefabs/Weapons/Rifles/M16/Rifle_M16A2.et");
		SpawnAreaWeaponsWeighted.AddItem(1.0, "{FA5C25BF66A53DCF}Prefabs/Weapons/Rifles/AK74/Rifle_AK74.et");	

		return SpawnAreaWeaponsWeighted;	
	}

	static WR_WeightedItemArray<ResourceName> GetSpawnAreaItems()
	{
		if (SpawnAreaItemsWeighted) return SpawnAreaItemsWeighted;
		SpawnAreaItemsWeighted = new WR_WeightedItemArray<ResourceName>();

		SpawnAreaItemsWeighted.AddItem(40.0, "{A81F501D3EF6F38E}Prefabs/Items/Medicine/FieldDressing_US_01.et");

		SpawnAreaItemsWeighted.AddItem(1.0, "{0CF54B9A85D8E0D4}Prefabs/Items/Equipment/Binoculars/Binoculars_M22/Binoculars_M22.et");
		SpawnAreaItemsWeighted.AddItem(1.0, "{243948B23D90BECB}Prefabs/Items/Equipment/Binoculars/Binoculars_B8/Binoculars_B8.et");
		SpawnAreaItemsWeighted.AddItem(1.0, "{F2539FA5706E51E4}Prefabs/Items/Equipment/Binoculars/Binoculars_B12/Binoculars_B12.et");

		return SpawnAreaItemsWeighted;
	}
	
	static WR_WeightedItemArray<ResourceName> GetSpawnAreaVehicles()
	{
		if (SpawnAreaVehiclesWeighted) return SpawnAreaVehiclesWeighted;
		SpawnAreaVehiclesWeighted = new WR_WeightedItemArray<ResourceName>();

		// M997/M998/M1025 (Humvee)
		array<ResourceName> humvee = {
			"{5674FAEB9AB7BDD0}Prefabs/Vehicles/Wheeled/M998/M998.et",
			"{27E2E58E734A80EC}Prefabs/Vehicles/Wheeled/M998/M1025_MERDC.et",
			"{00C9BBE426F7D459}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance.et"
		};
		SpawnAreaVehiclesWeighted.AddItemsDistributedWeight(8.0, humvee);
		
		// M151A2 (American jeep)
		array<ResourceName> m151a2 = {
			"{F649585ABB3706C4}Prefabs/Vehicles/Wheeled/M151A2/M151A2.et",
			"{86D830868F026D54}Prefabs/Vehicles/Wheeled/M151A2/M151A2_MERDC.et",
			"{47D94E1193A88497}Prefabs/Vehicles/Wheeled/M151A2/M151A2_transport.et",
			"{94DE32169691AC34}Prefabs/Vehicles/Wheeled/M151A2/M151A2_transport_MERDC.et"
		};
		SpawnAreaVehiclesWeighted.AddItemsDistributedWeight(8.0, m151a2);

		
		// UAZ 469 (russian jeep)
		array<ResourceName> uazMilitary = {
			"{259EE7B78C51B624}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469.et",
			"{99F1610551D54D17}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_base.et",
			"{16A674FE31B0921C}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_uncovered.et",
			"{F7E9AA0C813EABDA}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_FIA.et",
			"{E28501E93F8EFDC0}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_FIA_uncovered.et"
		};
		SpawnAreaVehiclesWeighted.AddItemsDistributedWeight(10.0, uazMilitary);

		array<ResourceName> uazCiv = {
			"{C8B16ABCA57679A2}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_blue.et",
			"{D7C51A961BF675B2}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_brown.et",
			"{B6D330E7E40FE7DD}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_forest.et",
			"{5070B3749C7782D8}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_JZD.et",
			"{BDFB2377B6B4ABBB}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_PTS.et",
			"{A4D16A62B432451A}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_red.et",
			"{870EB660433857D3}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_teal.et",
			"{B2CC137FE424B129}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_covered_CIV_VAK.et"
		};
		SpawnAreaVehiclesWeighted.AddItemsDistributedWeight(10.0, uazCiv);
		
		// S105 sedan
		array<ResourceName> s105 = {
			"{54C3CC22DEBD57BE}Prefabs/Vehicles/Wheeled/S105/S105_beige.et",
			"{9C3269FB43F989A3}Prefabs/Vehicles/Wheeled/S105/S105_darkblue.et",
			"{A1CD41DFE0C93CAD}Prefabs/Vehicles/Wheeled/S105/S105_darkbrown.et",
			"{8000A0F649FB4B0D}Prefabs/Vehicles/Wheeled/S105/S105_darkred.et",
			"{54C3CC22DEBD57BE}Prefabs/Vehicles/Wheeled/S105/S105_green.et",
			"{8EB38BBD860DA06E}Prefabs/Vehicles/Wheeled/S105/S105_lightblue.et",
			"{321016E0F9361A22}Prefabs/Vehicles/Wheeled/S105/S105_lightgreen.et",
			"{5555DCAFADB8CE2A}Prefabs/Vehicles/Wheeled/S105/S105_rally.et",
			"{85F0BD6AF0F42D6D}Prefabs/Vehicles/Wheeled/S105/S105_red.et",
			"{63880E311299A016}Prefabs/Vehicles/Wheeled/S105/S105_white.et"
		};
		SpawnAreaVehiclesWeighted.AddItemsDistributedWeight(10.0, s105);
		SpawnAreaVehiclesWeighted.AddItem(1.0, "{5555DCAFADB8CE2A}Prefabs/Vehicles/Wheeled/S105/S105_yellow.et");
		
		// S1203 van
		array<ResourceName> s1203 = {
			"{F77C41245A580FD1}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_blue.et",
			"{543799AC5C52989C}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_beige.et",
			"{ACED839A6C5BE657}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_brown.et",
			"{11A28C90DB40318D}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_red.et"
		};
		SpawnAreaVehiclesWeighted.AddItemsDistributedWeight(10.0, s1203);
		SpawnAreaVehiclesWeighted.AddItem(1.0, "{4C79EB5FADB89B7A}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_yellow.et");
		
		// UAZ 452 (Russian van)
		SpawnAreaVehiclesWeighted.AddItem(4.0, "{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et");

		// Ural 4320 (Russian transport truck)
		array<ResourceName> uralTransportMilitary = {
			"{16C1F16C9B053801}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport.et",
			"{D9B91FAB817A6033}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport_covered.et",
			"{16E32C3ABEAFC2C6}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_FIA_transport.et",
			"{B70E6D12A8EC2410}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_FIA_transport_covered.et"

		};
		SpawnAreaVehiclesWeighted.AddItemsDistributedWeight(10.0, uralTransportMilitary);

		array<ResourceName> uralUtilityMilitary = {
			"{3336BE330C4F355B}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_ammo.et",
			"{A5647958579A4149}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_repair.et",
			"{4C81D7ED8F8C0D87}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_tanker.et",
			"{3F5D19AC3C7E20DC}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_ammo_FIA.et",
			"{34178E3A92776DA6}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_repair_FIA.et",
			"{DC1C2350CF665F9E}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_FIA_tanker.et"
		};
		SpawnAreaVehiclesWeighted.AddItemsDistributedWeight(10.0, uralUtilityMilitary);

		array<ResourceName> uralTransportCiv = {
			"{F66EAD0D2016B6BA}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport_covered_CIV_blue.et",
			"{66241E0CEDFCEDFF}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport_covered_CIV_orange.et"
		};
		SpawnAreaVehiclesWeighted.AddItemsDistributedWeight(10.0, uralTransportCiv);

		array<ResourceName> uralUtilityCiv = {
			"{4A59DAEFE645E8A0}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_tanker_CIV_orange.et",
			"{B3C8EC88E7782C78}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_tanker_CIV_pohona.et"
		};
		SpawnAreaVehiclesWeighted.AddItemsDistributedWeight(10.0, uralUtilityCiv);

		// M923A1 (American transport truck)
		array<ResourceName> m923a1Transport = {
			"{F1FBD0972FA5FE09}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport.et",
			"{81FDAD5EB644CC3D}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport_covered.et",
			"{3F2AA823B6C65E1E}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport_MERDC.et",
			"{04BDACC0BB83284E}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport_covered_MERDC.et",
		};
		SpawnAreaVehiclesWeighted.AddItemsDistributedWeight(10.0, m923a1Transport);

		array<ResourceName> m923a1Utility = {
			"{92264FF932676C13}Prefabs/Vehicles/Wheeled/M923A1/M923A1_ammo.et",
			"{A042ACE5C2B13206}Prefabs/Vehicles/Wheeled/M923A1/M923A1_repair.et",
			"{2BE1F8B9299B67C1}Prefabs/Vehicles/Wheeled/M923A1/M923A1_tanker.et",
			"{EC657C9B3C73E192}Prefabs/Vehicles/Wheeled/M923A1/M923A1_ammo_MERDC.et",
			"{C8656AECF5DF41D9}Prefabs/Vehicles/Wheeled/M923A1/M923A1_repair_MERDC.et",
			"{26013123314183EA}Prefabs/Vehicles/Wheeled/M923A1/M923A1_tanker_MERDC.et"
		};
		SpawnAreaVehiclesWeighted.AddItemsDistributedWeight(10.0, m923a1Utility);


		//// Armed vehicles ////


		// BTR-70 (Russian APC)
		SpawnAreaVehiclesWeighted.AddItem(2.0, "{C012BB3488BEA0C2}Prefabs/Vehicles/Wheeled/BTR70/BTR70.et");
		SpawnAreaVehiclesWeighted.AddItem(2.0, "{B47110AA1A806556}Prefabs/Vehicles/Wheeled/BTR70/BTR70_FIA.et");

		// UAZ 469 (Russian jeep)
		SpawnAreaVehiclesWeighted.AddItem(5.0, "{0B4DEA8078B78A9B}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_PKM.et");
		SpawnAreaVehiclesWeighted.AddItem(5.0, "{22B327C6752EC4D4}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_PKM_FIA.et");
		SpawnAreaVehiclesWeighted.AddItem(7.5, "{E72D78E7F45532EC}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_UK59_FIA.et");

		// M1025 (Humvee)
		SpawnAreaVehiclesWeighted.AddItem(3.0, "{3EA6F47D95867114}Prefabs/Vehicles/Wheeled/M998/M1025_armed_M2HB.et");
		SpawnAreaVehiclesWeighted.AddItem(3.0, "{DD774A8FD0989A78}Prefabs/Vehicles/Wheeled/M998/M1025_armed_M2HB_MERDC.et");

		// M151A2 (American jeep)
		SpawnAreaVehiclesWeighted.AddItem(5.0, "{F6B23D17D5067C11}Prefabs/Vehicles/Wheeled/M151A2/M151A2_M2HB.et");
		SpawnAreaVehiclesWeighted.AddItem(5.0, "{5168FEA3054D6D15}Prefabs/Vehicles/Wheeled/M151A2/M151A2_M2HB_MERDC.et");

		
		return SpawnAreaVehiclesWeighted;
	}
	
	static WR_WeightedItemArray<ResourceName> GetCaptureVehicles()
	{
		if (CaptureVehiclesWeighted) return CaptureVehiclesWeighted;
		CaptureVehiclesWeighted = new WR_WeightedItemArray<ResourceName>();
		
		//Ural Utility
		array<ResourceName> uralUtilityMilitary = {
			"{3336BE330C4F355B}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_ammo.et",
			"{A5647958579A4149}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_repair.et",
			"{4C81D7ED8F8C0D87}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_tanker.et",
			"{3F5D19AC3C7E20DC}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_ammo_FIA.et",
			"{34178E3A92776DA6}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_repair_FIA.et",
			"{DC1C2350CF665F9E}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_FIA_tanker.et"
		};
		CaptureVehiclesWeighted.AddItemsDistributedWeight(20.0, uralUtilityMilitary);
		
		//M923A1 Utility
		array<ResourceName> m923a1Utility = {
			"{92264FF932676C13}Prefabs/Vehicles/Wheeled/M923A1/M923A1_ammo.et",
			"{A042ACE5C2B13206}Prefabs/Vehicles/Wheeled/M923A1/M923A1_repair.et",
			"{2BE1F8B9299B67C1}Prefabs/Vehicles/Wheeled/M923A1/M923A1_tanker.et",
			"{EC657C9B3C73E192}Prefabs/Vehicles/Wheeled/M923A1/M923A1_ammo_MERDC.et",
			"{C8656AECF5DF41D9}Prefabs/Vehicles/Wheeled/M923A1/M923A1_repair_MERDC.et",
			"{26013123314183EA}Prefabs/Vehicles/Wheeled/M923A1/M923A1_tanker_MERDC.et"
		};
		CaptureVehiclesWeighted.AddItemsDistributedWeight(20.0, m923a1Utility);
		
		// BTR-70 (Russian APC)
		CaptureVehiclesWeighted.AddItem(2.0, "{C012BB3488BEA0C2}Prefabs/Vehicles/Wheeled/BTR70/BTR70.et");
		CaptureVehiclesWeighted.AddItem(2.0, "{B47110AA1A806556}Prefabs/Vehicles/Wheeled/BTR70/BTR70_FIA.et");

		// UAZ 469 (Russian jeep)
		CaptureVehiclesWeighted.AddItem(5.0, "{0B4DEA8078B78A9B}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_PKM.et");
		CaptureVehiclesWeighted.AddItem(5.0, "{22B327C6752EC4D4}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_PKM_FIA.et");
		CaptureVehiclesWeighted.AddItem(7.5, "{E72D78E7F45532EC}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_UK59_FIA.et");

		// M1025 (Humvee)
		CaptureVehiclesWeighted.AddItem(3.0, "{3EA6F47D95867114}Prefabs/Vehicles/Wheeled/M998/M1025_armed_M2HB.et");
		CaptureVehiclesWeighted.AddItem(3.0, "{DD774A8FD0989A78}Prefabs/Vehicles/Wheeled/M998/M1025_armed_M2HB_MERDC.et");

		// M151A2 (American jeep)
		CaptureVehiclesWeighted.AddItem(5.0, "{F6B23D17D5067C11}Prefabs/Vehicles/Wheeled/M151A2/M151A2_M2HB.et");
		CaptureVehiclesWeighted.AddItem(5.0, "{5168FEA3054D6D15}Prefabs/Vehicles/Wheeled/M151A2/M151A2_M2HB_MERDC.et");
		
		return CaptureVehiclesWeighted;
	}
	
}