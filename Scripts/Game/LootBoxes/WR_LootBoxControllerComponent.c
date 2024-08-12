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
	
	protected SCR_BaseGameMode _parent;

	override void OnPostInit(IEntity owner)
	{
		_parent = SCR_BaseGameMode.Cast(owner);
		if (!_parent)
		{
			Print("[WASTELAND] WR_LootBoxControllerComponent: Parent entity of WR_LootBoxControllerComponent must be a SCR_BaseGameMode!", LogLevel.ERROR);
			return;
		}

		if (minLootBoxItems > maxLootBoxItems)
		{
			Print("[WASTELAND] WR_LootBoxControllerComponent: Minimum amount of loot items must be smaller than maximum amount! Setting maximum to match minimum...", LogLevel.WARNING);
			maxLootBoxItems = minLootBoxItems;
		}
	}

	override void OnGameModeStart()
	{
		BaseRplComponent rplComponent = BaseRplComponent.Cast(_parent.FindComponent(BaseRplComponent));
		if (rplComponent.Role() != RplRole.Authority) return;

		SpawnItemsInLootBoxes();
	}
	
	private void SpawnItemsInLootBoxes()
	{		
		array<WR_LootBoxComponent> lootBoxComponents = WR_LootBoxComponent.LootBoxComponents;
		if (!lootBoxComponents || lootBoxComponents.Count() == 0) return;
		
		Print("[WASTELAND] WR_LootBoxControllerComponent: Spawning loot box(es)...", LogLevel.NORMAL);
		
		// Get loot spawning context
		WR_LootSpawnContext lootContext = WR_LootSpawnContextPresets.GetLootContextByType(PAND_LootContextType.LOOT_BOX);
		
		int lootBoxesSpawned = 0;
		foreach (WR_LootBoxComponent boxComponent : lootBoxComponents)
		{
			if (!boxComponent) continue;
			
			IEntity boxOwner = boxComponent.GetOwner();
			
			//Refactor at some point
			if (Math.RandomFloat(0, 1) > percentageLootBoxesToSpawn)
			{
				delete boxOwner;
				continue;
			}
			
			lootBoxesSpawned += 1;
			
			auto inventoryStorage = SCR_UniversalInventoryStorageComponent.Cast(boxOwner.FindComponent(SCR_UniversalInventoryStorageComponent));
			auto inventoryStorageManager = SCR_InventoryStorageManagerComponent.Cast(boxOwner.FindComponent(SCR_InventoryStorageManagerComponent));

			array<ResourceName> items = lootContext.GetRandomItems(Math.RandomIntInclusive(minLootBoxItems, maxLootBoxItems));
			foreach (ResourceName item : items)
				inventoryStorageManager.TrySpawnPrefabToStorage(item, inventoryStorage);
		}
		
		Print("[WASTELAND] WR_LootBoxControllerComponent: Spawned a total of " + lootBoxesSpawned + " loot box(es).", LogLevel.NORMAL);
	}
}