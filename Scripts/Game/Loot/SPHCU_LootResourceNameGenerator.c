// This class provides methods to get all necessary ResourceNames for various loot spawning purposes
class SPHCU_LootResourceNameGenerator
{
	// TODO: Upgrade this class' methods to pull config values (amount of mags to spawn, chances to spawn a weapon in town vehicles, etc) from a component we place on the gamemode or something pretty global like that.

	static array<ResourceName> GetSpawnAreaVehicleItems()
	{
		array<ResourceName> itemsToSpawn = {};
		
		// Spawn some items
		SPHCU_WeightedItemArray<ResourceName> spawnAreaItems = SPHCU_ResourceNamesWeighted.GetSpawnAreaItems();
		int itemCount = Math.RandomIntInclusive(1, 3);
		for (int i = 0; i < itemCount; i++)
		{
			itemsToSpawn.Insert(spawnAreaItems.GetRandomItem());
		}

		// Chance to spawn a weapon
		float weaponChance = 0.95;
		if (Math.RandomFloat(0, 1) < weaponChance)
		{
			// Select a weapon
			ResourceName weapon = SPHCU_ResourceNamesWeighted.GetSpawnAreaWeapons().GetRandomItem();
			itemsToSpawn.Insert(weapon); // FYI: Weapons spawn with 1 mag

			// Get its ammo
			ResourceName ammo = SPHCU_Utils.GetDefaultAmmo(weapon);
			int ammoCount = Math.RandomIntInclusive(0, 3);
			
			for (int i = 0; i < ammoCount; i++)
				itemsToSpawn.Insert(ammo);
		}
		
		return itemsToSpawn;
	}
}