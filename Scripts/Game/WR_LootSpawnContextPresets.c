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
            headgearWeight: 2.5
        );
    }
}
