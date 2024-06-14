[ComponentEditorProps(category: "Spawning", description: "Populate loot boxes with random loot")]
class WR_LootBoxControllerComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class WR_LootBoxControllerComponent : SCR_BaseGameModeComponent
{	
	// TODO: add validation and make these read from a global config
	[Attribute("0.5", UIWidgets.Slider, "Average percentage of loot boxes to spawn on game start", "0 1 0.01")]
	protected float percentageLootBoxesToSpawn;
	
	[Attribute("5", UIWidgets.Slider, "Minimum number of items to spawn in a loot box", "0 10 1")]
	protected float minLootBoxItems;
	
	[Attribute("15", UIWidgets.Slider, "Maximum number of items to spawn in a loot box", "0 20 1")]
	protected float maxLootBoxItems;
	
	
	override void OnPostInit(IEntity owner)
	{
		if (minLootBoxItems > maxLootBoxItems)
		{
			Print("[WASTELAND] Minimum amount of loot items must be smaller than maximum amount! Setting maximum to match minimum...");
			maxLootBoxItems = minLootBoxItems;
		}
	}

	override void OnGameModeStart()
	{
		SpawnItemsInLootBoxes();
	}
	
	private void SpawnItemsInLootBoxes()
	{		
		array<WR_LootBoxComponent> lootBoxComponents = WR_LootBoxComponent.LootBoxComponents;
		if (!lootBoxComponents || lootBoxComponents.Count() == 0) return;
		
		Print("[WASTELAND] Filling loot box(es)...");
		
		// Get loot spawning context
		WR_LootSpawnContext lootContext = WR_LootSpawnContextPresets.GetLootBoxContext();
		
		foreach (WR_LootBoxComponent boxComponent : lootBoxComponents)
		{
			if (!boxComponent) continue;
			
			IEntity boxOwner = boxComponent.GetOwner();
			
			// Delete inverse of percentageLootBoxesToSpawn
			// TODO: potentially refactor this
			if (Math.RandomFloat(0, 1) < 1 - percentageLootBoxesToSpawn)
			{
				delete boxOwner;
				continue;
			}
			
			auto inventoryStorage = SCR_UniversalInventoryStorageComponent.Cast(boxOwner.FindComponent(SCR_UniversalInventoryStorageComponent));
			auto inventoryStorageManager = SCR_InventoryStorageManagerComponent.Cast(boxOwner.FindComponent(SCR_InventoryStorageManagerComponent));

			array<ResourceName> items = lootContext.GetRandomItems(Math.RandomIntInclusive(minLootBoxItems, maxLootBoxItems));
			foreach (ResourceName item : items)
				inventoryStorageManager.TrySpawnPrefabToStorage(item, inventoryStorage);
		}
	}
}