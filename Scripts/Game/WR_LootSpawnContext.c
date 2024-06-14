// This class provides methods to get all necessary ResourceNames for various loot spawning purposes
class WR_LootSpawnContext
{
    private ref SPHCU_WeightedItemArray<SPHCU_WeightedItemArray<ResourceName>> ItemCategories;

    void WR_LootSpawnContext(
        float rifleWeight = 1.0,
        float machineGunWeight = 0.0,
        float sniperWeight = 0.0,
        float handgunWeight = 0.0,
        float launcherWeight = 0.0,
        float ordnanceWeight = 0.0,
        float attachmentWeight = 0.0,
        float utilItemWeight = 0.0,
        float mediItemWeight = 0.0,
        float backpackWeight = 0.0,
        float vestWeight = 0.0,
        float headgearWeight = 0.0
    )
    {
        ItemCategories = new SPHCU_WeightedItemArray<SPHCU_WeightedItemArray<ResourceName>>();

        // Weapons
        ItemCategories.AddItem(rifleWeight,       SPHCU_ResourceNamesWeighted.GetRifles());
        ItemCategories.AddItem(machineGunWeight,  SPHCU_ResourceNamesWeighted.GetMachineGuns());
        ItemCategories.AddItem(sniperWeight,      SPHCU_ResourceNamesWeighted.GetSnipers());
        ItemCategories.AddItem(handgunWeight,     SPHCU_ResourceNamesWeighted.GetHandguns());
        ItemCategories.AddItem(launcherWeight,    SPHCU_ResourceNamesWeighted.GetLaunchers());
        ItemCategories.AddItem(ordnanceWeight,    SPHCU_ResourceNamesWeighted.GetOrdnance());
        // Items
        ItemCategories.AddItem(attachmentWeight,  SPHCU_ResourceNamesWeighted.GetAttachments());
        ItemCategories.AddItem(utilItemWeight,    SPHCU_ResourceNamesWeighted.GetUtilityItems());
        ItemCategories.AddItem(mediItemWeight,    SPHCU_ResourceNamesWeighted.GetMedicalItems());
        // Wearables
        ItemCategories.AddItem(backpackWeight,    SPHCU_ResourceNamesWeighted.GetBackpacks());
        ItemCategories.AddItem(vestWeight,        SPHCU_ResourceNamesWeighted.GetVests());
        ItemCategories.AddItem(headgearWeight,    SPHCU_ResourceNamesWeighted.GetHeadgear());
    }

    array<ResourceName> GetRandomItems(int itemCount)
    {
        if (!ItemCategories) return {};
        
        array<ResourceName> items = {};

        for (int i = 0; i < itemCount; i++)
        {
            SPHCU_WeightedItemArray<ResourceName> category = ItemCategories.GetRandomItem();
            ResourceName item = category.GetRandomItem();
            items.Insert(item);
        }
		
		return items;
    }
}
