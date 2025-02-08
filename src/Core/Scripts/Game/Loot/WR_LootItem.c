class WR_LootItem
{
	ResourceName m_sItemPrefab;
	int m_iMinAdditionalItems;
	int m_iMaxAdditionalItems;
	int m_iWeight;
	ref array<ResourceName> m_aAdditionalItemChoices;
	
	void WR_LootItem(WR_LootItemConfig conf)
	{
		m_sItemPrefab = conf.m_sItemPrefab;
		m_iMinAdditionalItems = conf.m_iMinAdditionalItems;
		m_iMaxAdditionalItems = conf.m_iMaxAdditionalItems;
		m_iWeight = conf.m_iWeight;
		m_aAdditionalItemChoices = conf.m_aAdditionalItemChoices;
	}
	
}