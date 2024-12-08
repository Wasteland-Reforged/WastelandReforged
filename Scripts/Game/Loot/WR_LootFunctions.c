class WR_LootFunctions
{
	WR_LootCategory GetRandomCategory(WR_MasterContextConfig contextConf, WR_LootContext lootContext)
	{
		
	}
	
	int GetRandomItemFromCategory(out array<ResourceName> items, WR_MasterLootConfig lootConf, WR_LootCategory category)		//Returns budget used, includes additional items
	{
		
	}
	
	array<ResourceName> GetRandomItemsByBudget(WR_MasterLootConfig lootConf, WR_MasterContextConfig contextConf, WR_LootContext lootContext, int totalBudget)
	{
		array<ResourceName> items = {};
		int budgetUsed = 0;
		
		while (budgetUsed < totalBudget)
		{
			WR_LootCategory randomCat = GetRandomCategory(contextConf, lootContext);
			budgetUsed += GetRandomItemFromCategory(items, lootConf, randomCat);
		}
		
		return items;
	}
	
}