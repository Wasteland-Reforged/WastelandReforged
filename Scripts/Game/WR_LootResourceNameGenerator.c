// This class provides methods to get all necessary ResourceNames for various loot spawning purposes
class WR_LootResourceNameGenerator
{
	// TODO: Upgrade this class' methods to pull config values (amount of mags to spawn, chances to spawn a weapon in town vehicles, etc) from a component we place on the gamemode or something pretty global like that.

//	static array<ResourceName> GetSpawnAreaVehicleItems()
//	{
//		array<ResourceName> itemsToSpawn = {};
//		
//		// Spawn some items
//		WR_WeightedItemArray<ResourceName> resourceNames = WR_ResourceNamesWeighted.GetSpawnAreaItems();
//		int itemCount = Math.RandomIntInclusive(1, 3);
//		for (int i = 0; i < itemCount; i++)
//		{
//			itemsToSpawn.Insert(resourceNames.GetRandomItem());
//		}
//
//		// Chance to spawn a weapon
//		float weaponChance = 0.95;
//		if (Math.RandomFloat(0, 1) < weaponChance)
//		{
//			// Select a weapon
//			ResourceName weapon = WR_ResourceNamesWeighted.GetSpawnAreaWeapons().GetRandomItem();
//			itemsToSpawn.Insert(weapon); // FYI: Weapons spawn with 1 mag
//
//			// Get its ammo
//			ResourceName ammo = WR_Utils.GetDefaultAmmo(weapon);
//			int ammoCount = Math.RandomIntInclusive(0, 3);
//			
//			for (int i = 0; i < ammoCount; i++)
//				itemsToSpawn.Insert(ammo);
//		}
//		
//		return itemsToSpawn;
//	}

	static array<string> GetLootBoxItems()
	{
		array<string> itemsToSpawn = {};
		
//		itemsToSpawn.Insert(WR_ResourceNamesWeighted.GetSpawnAreaWeapons().GetRandomItem());
//		itemsToSpawn.Insert(WR_ResourceNamesWeighted.GetSpawnAreaWeapons().GetRandomItem());
//		itemsToSpawn.Insert(WR_ResourceNamesWeighted.GetSpawnAreaWeapons().GetRandomItem());
		
//		for (int i = 0; i < Math.RandomIntInclusive(0, 4); i++)
//		{
//			if (Math.RandomFloatInclusive(0, 1) < 0.5)
//			{
//				if (Math.RandomFloatInclusive(0, 1) < 0.5)
//				{
//					//itemsToSpawn.Insert(WR_RandomLootConfig.getWeightedRareWeaponList().getRandomWeightedItemValue());
//					itemsToSpawn.Insert(WR_ResourceNamesWeighted.GetSpawnAreaWeapons().GetRandomItem());
//				}
//				else
//				{
//					//itemsToSpawn.Insert(WR_RandomLootConfig.getWeightedCommonWeaponList().getRandomWeightedItemValue());
//					itemsToSpawn.Insert(WR_ResourceNamesWeighted.GetSpawnAreaWeapons().GetRandomItem());
//				}
//			}
//			else
//			{
//				//itemsToSpawn.Insert(WR_RandomLootConfig.getWeightedOtherItemList().getRandomWeightedItemValue());
//				WR_ResourceNamesWeighted.GetSpawnAreaItems().GetRandomItem()
//			}
//		}
		
		return itemsToSpawn;
	}
}