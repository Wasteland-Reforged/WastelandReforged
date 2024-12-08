// TODO: See if this class can be turned into a workbench config. If not, eventually just have these values get read from a file somewhere.
class WR_LootSpawnContextPresets
{
	static ref map<WR_LootContext, ref WR_LootSpawnContext> LootContexts;
	
	static WR_LootSpawnContext GetLootContextByType(WR_LootContext type)
	{
		if (!LootContexts)
			LootContexts = new map<WR_LootContext, ref WR_LootSpawnContext>();
		
		WR_LootSpawnContext context;
		
		bool found = LootContexts.Find(type, context);
		
		if (!found)
		{
			switch (type)
			{
				case WR_LootContext.WEAPON_BOX:
					context = GetWeaponBoxContext();
					break;
				case WR_LootContext.HEAVY_WEAPON_BOX:
					context = GetHeavyWeaponBoxContext();
					break;
				case WR_LootContext.MEDICAL:
					context = GetMedicalBoxContext();
					break;
				case WR_LootContext.VEHICLE_LOOT:
					context = GetRandomVehicleContext();
					break;
				default:
					context = GetDefaultContext();
			}
			
			// Cache the context in the map so we don't have to regenerate it every time we need it
			LootContexts.Insert(type, context);
		}
		
		return context;
	}
	
    private static WR_LootSpawnContext GetDefaultContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 		20.0,
            machineGunWeight: 	1.0,
            sniperWeight: 		1.5,
            handgunWeight: 		5.0,
            launcherWeight: 	1.0,
            ordnanceWeight: 	8.0,
            attachmentWeight: 	5.0,
            utilItemWeight: 	5.0,
            mediItemWeight: 	8.0,
            backpackWeight: 	5.0,
            vestWeight: 		5.0,
            helmetWeight: 		5.0,
            hatWeight: 			5.0
        );
    }

    private static WR_LootSpawnContext GetRandomVehicleContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 		5.0,
            machineGunWeight: 	1.0,
            sniperWeight: 		0.5,
            handgunWeight: 		5.0,
            launcherWeight: 	1.0,
            ordnanceWeight: 	8.0,
            attachmentWeight: 	5.0,
            utilItemWeight: 	5.0,
            mediItemWeight: 	8.0,
            backpackWeight: 	5.0,
            vestWeight: 		5.0,
            helmetWeight: 		5.0,
            hatWeight: 			5.0
        );
    }
	
	private static WR_LootSpawnContext GetWeaponBoxContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 		10.0,
            machineGunWeight: 	4.0,
            sniperWeight: 		2.0,
            handgunWeight: 		5.0,
            launcherWeight: 	2.0,
            ordnanceWeight: 	5.0,
            attachmentWeight: 	5.0,
            utilItemWeight: 	0.0,
            mediItemWeight: 	0.0,
            backpackWeight: 	5.0,
            vestWeight: 		5.0,
            helmetWeight: 		5.0,
            hatWeight: 			0.0
        );
    }
	
	private static WR_LootSpawnContext GetHeavyWeaponBoxContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 		10.0,
            machineGunWeight: 	10.0,
            sniperWeight: 		10.0,
            handgunWeight: 		5.0,
            launcherWeight: 	2.0,
            ordnanceWeight: 	5.0,
            attachmentWeight: 	5.0,
            utilItemWeight: 	0.0,
            mediItemWeight: 	0.0,
            backpackWeight: 	5.0,
            vestWeight: 		5.0,
            helmetWeight: 		5.0,
            hatWeight: 			0.0
        );
    }
	
	private static WR_LootSpawnContext GetMedicalBoxContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 		0.0,
            machineGunWeight: 	0.0,
            sniperWeight: 		0.0,
            handgunWeight: 		0.0,
            launcherWeight: 	0.0,
            ordnanceWeight: 	0.0,
            attachmentWeight: 	0.0,
            utilItemWeight: 	5.0,
            mediItemWeight: 	15.0,
            backpackWeight: 	5.0,
            vestWeight: 		5.0,
            helmetWeight: 		0.0,
            hatWeight: 			0.0
        );
    }
}
