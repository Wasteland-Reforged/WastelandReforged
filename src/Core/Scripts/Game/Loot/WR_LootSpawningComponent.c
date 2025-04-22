class WR_LootSpawningComponentClass : SCR_BaseGameModeComponentClass {}


class WR_LootSpawningComponent : SCR_BaseGameModeComponent
{
	const string CATEGORY_WR = "Wasteland";
	
	ref WR_Logger<WR_LootSpawningComponent> logger = new WR_Logger<WR_LootSpawningComponent>(this);
	ref map<WR_LootCategory, ref SCR_WeightedArray<WR_LootItemConfig>> masterLootMap = new map<WR_LootCategory, ref SCR_WeightedArray<WR_LootItemConfig>>();
	ref map<WR_LootContext, ref SCR_WeightedArray<WR_LootCategory>> masterContextMap = new map<WR_LootContext, ref SCR_WeightedArray<WR_LootCategory>>();
	
	[Attribute("", UIWidgets.Object, "Loot system configuration.")];
	protected ref WR_LootTableConfig m_Config;

	override void OnPostInit(IEntity owner)
	{
		WR_GameModeWasteland gameMode = WR_GameModeWasteland.Cast(GetGame().GetGameMode());
		if (!gameMode)
			logger.LogError("Wasteland game mode entity not found! Cannot initialize loot maps. Place a 'GameMode_Wasteland' prefab in the world to resolve.");

		if (!m_Config)
		{
			logger.LogError("Loot item table config is missing from loot system config! Cannot initialize loot maps.");
			return;
		}
		
		InitializeLootMaps();
		
		logger.LogNormal("Initialized.");
	}
	
	void InitializeLootMaps()
	{			
		if (!m_Config.m_aItemConfigs || m_Config.m_aItemConfigs.Count() == 0)
		{
			logger.LogError("No loot item configs from loot table config! Cannot initialize loot maps.");
			return;
		}
		
		if (!m_Config.m_aContextConfigs || m_Config.m_aContextConfigs.Count() == 0)
		{
			logger.LogError("No loot item configs from loot table config! Cannot initialize loot maps.");
			return;
		}
		
//		if (!m_LootItemsConfig || !m_LootContextConfig)
//		{
//			logger.LogError("Loot context and/or loot category configs are missing from WR_LootSpawningComponent! Cannot initialize loot maps.");
//			return;
//		}
//		
//		if (!m_LootItemsConfig.m_aLootItemConfigs || !m_LootContextConfig.m_aLootContextConfigs 
//			|| m_LootItemsConfig.m_aLootItemConfigs.Count() == 0 || m_LootContextConfig.m_aLootContextConfigs.Count() == 0)
//		{
//			logger.LogError("Loot context and/or loot category configs do not have any elements set! Cannot initialize loot maps.");
//			return;
//		}
		
		// Populate master loot item map
		foreach (WR_LootItemConfig lootItemConfig : m_Config.m_aItemConfigs)
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
		
		// Populate master loot Context Map
		foreach (WR_LootContextConfig lootContextConfig : m_Config.m_aContextConfigs)
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
			if (itemWeight)
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