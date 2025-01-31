class WR_LootSpawningComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class WR_LootSpawningComponent : SCR_BaseGameModeComponent
{
	const string CATEGORY_WR = "Wasteland";
	
	ref WR_Logger<WR_LootSpawningComponent> logger = new WR_Logger<WR_LootSpawningComponent>(this);
	ref map<WR_LootCategory, ref SCR_WeightedArray<WR_LootItemConfig>> masterLootMap = new map<WR_LootCategory, ref SCR_WeightedArray<WR_LootItemConfig>>();
	ref map<WR_LootContext, ref SCR_WeightedArray<WR_LootCategory>> masterContextMap = new map<WR_LootContext, ref SCR_WeightedArray<WR_LootCategory>>();
	
	[Attribute("", UIWidgets.Object, "Master Loot Config.", category: CATEGORY_WR)]
	protected ref WR_MasterLootConfig m_MasterLootConfig;
	
	[Attribute("", UIWidgets.Object, "Master Loot Context Config.", category: CATEGORY_WR)]
	protected ref WR_MasterContextConfig m_MasterContextConfig;

	override void OnPostInit(IEntity owner)
	{
		InitializeLootMaps();
		Print("[WASTELAND] WR_LootSpawningComponent: Initialized Loot Maps");
	}
	
	void InitializeLootMaps()
	{		
		// Populate Master Loot Map
		foreach (WR_LootItemConfig lootItemConfig : m_MasterLootConfig.m_aLootItemConfigs)
		{
			SCR_WeightedArray<WR_LootItemConfig> lootArr;
			masterLootMap.Find(lootItemConfig.m_eCategory, lootArr);
			if (!lootArr)
			{
				lootArr = new SCR_WeightedArray<WR_LootItemConfig>();
				masterLootMap.Insert(lootItemConfig.m_eCategory, lootArr);
			}
			
			lootArr.Insert(lootItemConfig, lootItemConfig.m_iWeight);
		}
		
		// Populate Master Loot Context Map
		foreach (WR_LootContextConfig lootContextConfig : m_MasterContextConfig.m_aLootContextConfigs)
		{
			SCR_WeightedArray<WR_LootCategory> categoryArr;
			masterContextMap.Find(lootContextConfig.m_eContext, categoryArr);
			if (!categoryArr)
			{
				categoryArr = new SCR_WeightedArray<WR_LootCategory>();
				masterContextMap.Insert(lootContextConfig.m_eContext, categoryArr);
			}
			
			foreach (WR_LootCategoryWeightConfig weightConf : lootContextConfig.m_aLootCategoryWeightConfigs)
			{
				categoryArr.Insert(weightConf.m_eCategory, weightConf.m_iWeight);
			}
		}
	
	}
	
	WR_LootCategory GetRandomCategoryFromContext(WR_LootContext lootContext)
	{
		SCR_WeightedArray<WR_LootCategory> categoryArr = masterContextMap.Get(lootContext);
		WR_LootCategory cat;
		
		if (categoryArr)
			categoryArr.GetRandomValue(cat);
		else
			logger.LogError("Cannot find weighted category array for the provided loot context");
		
		return cat;
	}
	
	int GetRandomItemsFromCategory(out array<ResourceName> items, WR_LootCategory category, float additionalItemsCoeff = 1.0) // Returns budget used, includes additional items
	{
		SCR_WeightedArray<WR_LootItemConfig> lootArr = masterLootMap.Get(category);
		WR_LootItemConfig newItem;
		int weight = 0;
		
		if (!lootArr)
		{
			logger.LogError("Cannot find weighted array of loot item configs for the provided loot category");
			return 0;
		}

		// Select a random entry from the loot table
		lootArr.GetRandomValue(newItem);
		
		if (!newItem || !WR_LootItemConfig.Cast(newItem))
			return 0;
		
		items = {};
		
		// Insert base item
		items.Insert(newItem.m_sItemPrefab);
		weight += newItem.m_iWeight;
		
		// This item does not have any additional items specified, so exit.
		if (newItem.m_aAdditionalItemChoices.IsEmpty())
			return weight;
		
		if (newItem.m_bAlwaysSpawnAllAdditionalItems)
		{
			foreach (auto addlItem : newItem.m_aAdditionalItemChoices)
			{
				for (int i = 0; i < GetQuantity(newItem, additionalItemsCoeff); i++)
					items.Insert(addlItem);
			}
		}
		else
		{
			auto addlItem = newItem.m_aAdditionalItemChoices.GetRandomElement();
			for (int i = 0; i < GetQuantity(newItem, additionalItemsCoeff); i++)
				items.Insert(addlItem);
		}
		
		return weight;
	}
	
	ResourceName GetRandomSingleItemFromCategory(WR_LootCategory category)
	{
		SCR_WeightedArray<WR_LootItemConfig> lootArr = masterLootMap.Get(category);
		WR_LootItemConfig newItem;
		
		if (lootArr) {
			lootArr.GetRandomValue(newItem);
		}
		else {
			logger.LogError("Cannot find Weighted Loot Item array for the provided Loot Category");
		}
		
		return newItem.m_sItemPrefab;
	}
	
	array<ResourceName> GetRandomItemsByBudget(WR_LootContext lootContext, float totalBudget = 0.5, float additionalItemsCoeff = 1.0)
	{
		array<ResourceName> items = {};
		float budgetUsed = 0;
		
		while (budgetUsed < totalBudget)
		{
			WR_LootCategory randomCat = GetRandomCategoryFromContext(lootContext);
			float itemWeight = GetRandomItemsFromCategory(items, randomCat, additionalItemsCoeff);
			budgetUsed += 1 / itemWeight;
		}
		
		return items;
	}
	
	array<ResourceName> GetRandomItemsByCount(WR_LootContext lootContext, int minItems, int maxItems)
	{
		array<ResourceName> items = {};
		
		for (int i = 0; i < Math.RandomIntInclusive(minItems, maxItems); i++)
		{
			WR_LootCategory randomCat = GetRandomCategoryFromContext(lootContext);
			GetRandomItemsFromCategory(items, randomCat);
		}
		
		return items;
	}
	
	static WR_LootSpawningComponent GetInstance()
	{
		WR_GameModeWasteland gameMode = WR_GameModeWasteland.Cast(GetGame().GetGameMode());
		if (!gameMode)
			return null;
		
		return WR_LootSpawningComponent.Cast(gameMode.FindComponent(WR_LootSpawningComponent));
	}
	
	int GetQuantity(WR_LootItemConfig itemConfig, float multiplier)
	{
		return Math.RandomInt(itemConfig.m_iMinAdditionalItems * multiplier, itemConfig.m_iMaxAdditionalItems * multiplier);
	}
	
}