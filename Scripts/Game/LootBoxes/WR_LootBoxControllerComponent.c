[ComponentEditorProps(category: "Spawning", description: "Populate loot boxes with random loot")]
class WR_LootBoxHandlerComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class WR_LootBoxControllerComponent : SCR_BaseGameModeComponent
{
	[Attribute( "50", UIWidgets.Slider, "Chance per roll that the spawned item is a weapon", "0 100 5")]
	protected float weaponChance;
	
	[Attribute( "10", UIWidgets.Slider, "Chance that a spawned weapon will be Rare", "0 100 5")]
	protected float rareWeaponChance;
	
	[Attribute( "1", UIWidgets.Slider, "Minimum number of items to spawn in a loot box", "0 10 1")]
	protected float minLootBoxItems;
	
	[Attribute( "5", UIWidgets.Slider, "Maximum number of items to spawn in a loot box", "0 20 1")]
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
		Print("[WASTELAND] Filling loot boxes...");

		foreach (WR_LootBoxComponent box : WR_LootBoxComponent.LootBoxComponents)
		{
			if (!box) return;
			
			auto inventoryStorage = SCR_UniversalInventoryStorageComponent.Cast(box.FindComponent(SCR_UniversalInventoryStorageComponent));
			auto inventoryStorageManager = SCR_VehicleInventoryStorageManagerComponent.Cast(box.FindComponent(SCR_VehicleInventoryStorageManagerComponent));

			foreach (string item : GetBoxLoot())
				inventoryStorageManager.TrySpawnPrefabToStorage(item, inventoryStorage);
		}
	}
}