// This class provides methods to get all necessary ResourceNames for various loot spawning purposes
class WR_LootSpawnContext
{
    private ref WR_WeightedItemArray<WR_WeightedItemArray<ResourceName>> ItemCategories;

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
        float helmetWeight = 0.0,
	    float hatWeight = 0.0
    )
    {
        ItemCategories = new WR_WeightedItemArray<WR_WeightedItemArray<ResourceName>>();

        // Weapons
        ItemCategories.AddItem(rifleWeight,       WR_ResourceNamesWeighted.GetRifles());
        ItemCategories.AddItem(machineGunWeight,  WR_ResourceNamesWeighted.GetMachineGuns());
        ItemCategories.AddItem(sniperWeight,      WR_ResourceNamesWeighted.GetSnipers());
        ItemCategories.AddItem(handgunWeight,     WR_ResourceNamesWeighted.GetHandguns());
        ItemCategories.AddItem(launcherWeight,    WR_ResourceNamesWeighted.GetLaunchers());
        ItemCategories.AddItem(ordnanceWeight,    WR_ResourceNamesWeighted.GetOrdnance());
        // Items
        ItemCategories.AddItem(attachmentWeight,  WR_ResourceNamesWeighted.GetAttachments());
        ItemCategories.AddItem(utilItemWeight,    WR_ResourceNamesWeighted.GetUtilityItems());
        ItemCategories.AddItem(mediItemWeight,    WR_ResourceNamesWeighted.GetMedicalItems());
        // Wearables
        ItemCategories.AddItem(backpackWeight,    WR_ResourceNamesWeighted.GetBackpacks());
        ItemCategories.AddItem(vestWeight,        WR_ResourceNamesWeighted.GetVests());
        ItemCategories.AddItem(helmetWeight,    	WR_ResourceNamesWeighted.GetHelmets());
		ItemCategories.AddItem(hatWeight,    		WR_ResourceNamesWeighted.GetHats());
    }

    array<ResourceName> GetRandomItems(int itemCount, int minExtraMags = 0, int maxExtraMags = 3)
    {
        if (!ItemCategories) return {};
        
        array<ResourceName> items = {};

        for (int i = 0; i < itemCount; i++)
        {
            WR_WeightedItemArray<ResourceName> category = ItemCategories.GetRandomItem();
            ResourceName item = category.GetRandomItem();
            items.Insert(item);

            // If this item is a weapon, spawn some extra ammo
            if (maxExtraMags > 0 && WR_Utils.IsReloadableWeapon(item))
            {
                // maxExtraMags cannot be less than the min
                if (maxExtraMags < minExtraMags) maxExtraMags = minExtraMags;
                
                int ammoCount = Math.RandomIntInclusive(minExtraMags, maxExtraMags);
                ResourceName ammo = WR_Utils.GetDefaultAmmo(item);
                for (int j = 0; j < ammoCount; j++) 
					items.Insert(ammo);
            }
        }
		
		return items;
    }
	
}
