// TODO: See if we can turn this class into a workbench config. If not, just have it read from a file somewhere.
class WR_LootSpawnContextPresets
{
    static WR_LootSpawnContext GetLootBoxContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 500.0,
            machineGunWeight: 1.0,
            sniperWeight: 1.0,
            handgunWeight: 1.0,
            launcherWeight: 500.0,
            ordnanceWeight: 1.0,
            attachmentWeight: 1.0,
            utilItemWeight: 1.0,
            mediItemWeight: 1.0,
            backpackWeight: 1.0,
            vestWeight: 1.0,
            headgearWeight: 1.0
        );
    }

    static WR_LootSpawnContext GetRandomVehicleContext()
    {
        return new WR_LootSpawnContext
        (
            rifleWeight: 1.0,
            machineGunWeight: 500.0,
            sniperWeight: 1.0,
            handgunWeight: 1.0,
            launcherWeight: 1.0,
            ordnanceWeight: 1.0,
            attachmentWeight: 1.0,
            utilItemWeight: 1.0,
            mediItemWeight: 1.0,
            backpackWeight: 500.0,
            vestWeight: 1.0,
            headgearWeight: 1.0
        );
    }
}
