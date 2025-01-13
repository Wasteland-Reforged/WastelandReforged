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
		//Populate Master Loot Map
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
		
		//Populate Master Loot Context Map
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
			logger.LogError("Cannot find Weighted Category Array for the provided Loot Context");
		
		return cat;
	}
	
	int GetRandomItemsFromCategory(out array<ResourceName> items, WR_LootCategory category)		//Returns budget used, includes additional items
	{
		SCR_WeightedArray<WR_LootItemConfig> lootArr = masterLootMap.Get(category);
		WR_LootItemConfig newItem;
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
	
	array<ResourceName> GetRandomItemsByBudget(WR_LootContext lootContext, float totalBudget = 0.5)
	{
		array<ResourceName> items = {};
		float budgetUsed = 0;
		
		while (budgetUsed < totalBudget)
		{
			WR_LootCategory randomCat = GetRandomCategoryFromContext(lootContext);
			float itemWeight = GetRandomItemsFromCategory(items, randomCat);
			//Print("[WR_LootSpawningComponent]: new item has a budget of " + itemWeight);
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
	
}