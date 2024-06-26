// TODO: See if we can turn this class into a workbench config. If not, just have it read from a file somewhere.
class WR_LootSpawnContextPresets
{
    static WR_LootSpawnContext GetLootBoxContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 5.0,
            machineGunWeight: 1.0,
            sniperWeight: 1.0,
            handgunWeight: 5.0,
            launcherWeight: 1.0,
            ordnanceWeight: 1.0,
            attachmentWeight: 10.0,
            utilItemWeight: 5.0,
            mediItemWeight: 10.0,
            backpackWeight: 5.0,
            vestWeight: 5.0,
            headgearWeight: 5.0
        );
    }

    static WR_LootSpawnContext GetRandomVehicleContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 10.0,
            machineGunWeight: 1.0,
            sniperWeight: 1.0,
            handgunWeight: 10.0,
            launcherWeight: 1.0,
            ordnanceWeight: 1.0,
            attachmentWeight: 1.0,
            utilItemWeight: 1.0,
            mediItemWeight: 5.0,
            backpackWeight: 5.0,
            vestWeight: 5.0,
            headgearWeight: 5.0
        );
    }
	
	static WR_LootSpawnContext GetWeaponBoxContext()
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
            vestWeight: 0.0,
            headgearWeight: 0.0
        );
    }
	
	static WR_LootSpawnContext GetHeavyWeaponBoxContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 5.0,
            machineGunWeight: 10.0,
            sniperWeight: 10.0,
            handgunWeight: 0.0,
            launcherWeight: 5.0,
            ordnanceWeight: 0.0,
            attachmentWeight: 10.0,
            utilItemWeight: 0.0,
            mediItemWeight: 0.0,
            backpackWeight: 5.0,
            vestWeight: 0.0,
            headgearWeight: 0.0
        );
    }
	
	static WR_LootSpawnContext GetMedicalBoxContext()
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
            headgearWeight: 0.0
        );
    }
}
