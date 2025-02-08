[ComponentEditorProps(category: "Spawning", description: "Populate loot boxes with random loot")]
class WR_LootBoxControllerComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class WR_LootBoxControllerComponent : SCR_BaseGameModeComponent
{	
	// TODO: add validation and make these read from a global config
	[Attribute("1", UIWidgets.Slider, "Average percentage of loot boxes to spawn on game start", "0 1 0.01")]
	protected float percentageLootBoxesToSpawn;
	
	[Attribute("1.0", UIWidgets.Slider, "Budget of items to spawn in the loot box", "0 3.0 0.1")]
	protected float lootBoxBudget;
	
	[Attribute("2.0", UIWidgets.Slider, "Multiplier for additional items in the loot box", "0 3.0 0.1")]
	protected float addItemCoeff;
	
	protected SCR_BaseGameMode _parent;

	override void OnPostInit(IEntity owner)
	{
		_parent = SCR_BaseGameMode.Cast(owner);
		if (!_parent)
		{
			Print("[WASTELAND] WR_LootBoxControllerComponent: Parent entity of WR_LootBoxControllerComponent must be a SCR_BaseGameMode!", LogLevel.ERROR);
			return;
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
		
		int lootBoxesSpawned = 0;
		WR_LootSpawningComponent lootSpawningComponent = WR_LootSpawningComponent.GetInstance();
		foreach (WR_LootBoxComponent boxComponent : lootBoxComponents)
		{
			if (!boxComponent)
				continue;
			
			IEntity boxOwner = boxComponent.GetOwner();
			
			if (Math.RandomFloat01() > percentageLootBoxesToSpawn)
			{
				delete boxOwner;
				continue;
			}
			
			auto inventoryStorage = SCR_UniversalInventoryStorageComponent.Cast(boxOwner.FindComponent(SCR_UniversalInventoryStorageComponent));
			auto inventoryStorageManager = SCR_InventoryStorageManagerComponent.Cast(boxOwner.FindComponent(SCR_InventoryStorageManagerComponent));

			array<ResourceName> items = lootSpawningComponent.GetRandomItemsByBudget(WR_LootContext.WEAPON_BOX, lootBoxBudget, additionalItemsCoeff: addItemCoeff);
			foreach (ResourceName item : items)
				inventoryStorageManager.TrySpawnPrefabToStorage(item, inventoryStorage);
			
			lootBoxesSpawned += 1;
		}
		
		Print("[WASTELAND] WR_LootBoxControllerComponent: Spawned a total of " + lootBoxesSpawned + " loot box(es).", LogLevel.NORMAL);
	}
}