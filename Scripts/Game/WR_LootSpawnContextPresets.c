// TODO: See if this class can be turned into a workbench config. If not, eventually just have these values get read from a file somewhere.
class WR_LootSpawnContextPresets
{
	static ref map<WR_LootContextType, ref WR_LootSpawnContext> LootContexts;
	
	static WR_LootSpawnContext GetLootContextByType(WR_LootContextType type)
	{
		if (!LootContexts)
			LootContexts = new map<WR_LootContextType, ref WR_LootSpawnContext>();
		
		WR_LootSpawnContext context;
		
		bool found = LootContexts.Find(type, context);
		
		if (!found)
		{
			switch (type)
			{
				case WR_LootContextType.WEAPONS:
					context = GetWeaponBoxContext();
					break;
				case WR_LootContextType.HEAVY_WEAPONS:
					context = GetHeavyWeaponBoxContext();
					break;
				case WR_LootContextType.MEDICAL:
					context = GetMedicalBoxContext();
					break;
				case WR_LootContextType.RANDOM_VEHICLE:
					context = GetRandomVehicleContext();
					break;
				case WR_LootContextType.LOOT_BOX:
					context = GetLootBoxContext();
					break;
				default:
					context = GetLootBoxContext();
			}
			
			// Cache the context in the map so we don't have to regenerate it every time we need it
			LootContexts.Insert(type, context);
		}
		
		return context;
	}
	
    private static WR_LootSpawnContext GetLootBoxContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 5.0,
            machineGunWeight: 1.0,
            sniperWeight: 1.0,
            handgunWeight: 5.0,
            launcherWeight: 1.0,
            ordnanceWeight: 30.0,
            attachmentWeight: 10.0,
            utilItemWeight: 5.0,
            mediItemWeight: 10.0,
            backpackWeight: 5.0,
            vestWeight: 5.0,
            helmetWeight: 5.0,
            hatWeight: 1.0
        );
    }

    private static WR_LootSpawnContext GetRandomVehicleContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 10.0,
            machineGunWeight: 3.0,
            sniperWeight: 1.0,
            handgunWeight: 4.0,
            launcherWeight: 1.0,
            ordnanceWeight: 4.0,
            attachmentWeight: 5.0,
            utilItemWeight: 8.0,
            mediItemWeight: 5.0,
            backpackWeight: 2.5,
            vestWeight: 2.5,
            helmetWeight: 2.5,
            hatWeight: 3.0
        );
    }
	
	private static WR_LootSpawnContext GetWeaponBoxContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 7.0,
            machineGunWeight: 1.0,
            sniperWeight: 1.0,
            handgunWeight: 5.0,
            launcherWeight: 1.0,
            ordnanceWeight: 0.0,
            attachmentWeight: 10.0,
            utilItemWeight: 0.0,
            mediItemWeight: 0.0,
            backpackWeight: 5.0,
            vestWeight: 2.0,
            helmetWeight: 1.0,
            hatWeight: 1.0
        );
    }
	
	private static WR_LootSpawnContext GetHeavyWeaponBoxContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 5.5,
            machineGunWeight: 8.0,
            sniperWeight: 11.0,
            handgunWeight: 0.0,
            launcherWeight: 7.5,
            ordnanceWeight: 5.0,
            attachmentWeight: 15.0,
            utilItemWeight: 2.0,
            mediItemWeight: 0.0,
            backpackWeight: 5.0,
            vestWeight: 2.5,
            helmetWeight: 1.0,
            hatWeight: 1.0
        );
    }
	
	private static WR_LootSpawnContext GetMedicalBoxContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 0.0,
            machineGunWeight: 0.0,
            sniperWeight: 0.0,
            handgunWeight: 0.0,
            launcherWeight: 0.0,
            ordnanceWeight: 0.0,
            attachmentWeight: 0.0,
            utilItemWeight: 0.0,
            mediItemWeight: 15.0,
            backpackWeight: 5.0,
            vestWeight: 0.0,
            helmetWeight: 0.0,
            hatWeight: 0.0
        );
    }
}
