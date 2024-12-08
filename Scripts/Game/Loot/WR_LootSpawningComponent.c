class WR_LootSpawningComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class WR_LootSpawningComponent : SCR_BaseGameModeComponent
{
	ref WR_Logger<WR_LootSpawningComponent> logger = new WR_Logger<WR_LootSpawningComponent>(this);
	ref map<WR_LootCategory, SCR_WeightedArray<WR_LootItem>> masterLootMap = new map<WR_LootCategory, SCR_WeightedArray<WR_LootItem>>();
	ref map<WR_LootContext, SCR_WeightedArray<WR_LootCategory>> masterContextMap = new map<WR_LootContext, SCR_WeightedArray<WR_LootCategory>>();

	void InitializeLootMaps()
	{
		//Find Gamemode
		WR_GameModeWasteland gameMode = WR_GameModeWasteland.Cast(GetGame().GetGameMode());
		if (!gameMode)
			logger.LogError("Wasteland game mode entity not found! Cannot send mission notifications. Place a 'GameMode_Wasteland' prefab in the world to resolve.");
		
		//Populate Master Loot Map
		WR_MasterLootConfig lootConf = gameMode.GetMasterLootConfig();
		foreach (WR_LootItemConfig lootItemConfig : lootConf.m_aLootItemConfigs)
		{
			SCR_WeightedArray<WR_LootItem> lootArr;
			masterLootMap.Find(lootItemConfig.m_eCategory, lootArr);
			if (!lootArr)
			{
				lootArr = new SCR_WeightedArray<WR_LootItem>();
				masterLootMap.Insert(lootItemConfig.m_eCategory, lootArr);
			}
			
			lootArr.Insert(new WR_LootItem(lootItemConfig), lootItemConfig.m_iWeight);
		}
		
		//Populate Master Loot Context Map
		WR_MasterContextConfig contextConf = gameMode.GetMasterLootContextConfig();
		foreach (WR_LootContextConfig lootContextConfig : contextConf.m_aLootContextConfigs)
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
			logger.LogError("Cannot find Weighted Category Array for the provided Loot Context");
		
		return cat;
	}
	
	int GetRandomItemFromCategory(out array<ResourceName> items, WR_LootCategory category)		//Returns budget used, includes additional items
	{
		SCR_WeightedArray<WR_LootItem> lootArr = masterLootMap.Get(category);
		WR_LootItem newItem;
		int weight = 0;
		if (lootArr)
		{
			lootArr.GetRandomValue(newItem);
			items.Insert(newItem.m_sItemPrefab);
			weight += newItem.m_iWeight;
			if (newItem.m_aAdditionalItemChoices)
			{
				auto additionalItem = newItem.m_aAdditionalItemChoices.GetRandomElement();
				for (int i = 0; i < Math.RandomInt(newItem.m_iMinAdditionalItems, newItem.m_iMaxAdditionalItems); i++)
					items.Insert(additionalItem);
			}
		}
		else
		{
			logger.LogError("Cannot find Weighted Loot Item array for the provided Loot Category");
		}
		
		return weight;
	}
	
	array<ResourceName> GetRandomItemsByBudget(WR_LootContext lootContext, int totalBudget)
	{
		array<ResourceName> items = {};
		int budgetUsed = 0;
		
		while (budgetUsed < totalBudget)
		{
			WR_LootCategory randomCat = GetRandomCategoryFromContext(lootContext);
			budgetUsed += GetRandomItemFromCategory(items, randomCat);
		}
		
		return items;
	}
	
	
	
}