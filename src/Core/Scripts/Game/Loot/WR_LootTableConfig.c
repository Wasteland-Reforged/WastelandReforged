[BaseContainerProps(configRoot: true)]
class WR_LootTableConfig
{
	[Attribute("", UIWidgets.Object, desc: "Contexts in which a loot item can spawn, each with their own weights for item categories.")]
	ref array<ref WR_LootContextConfig> m_aContextConfigs;
	
	[Attribute("", UIWidgets.Object, desc: "Items that can spawn as loot.")]
	ref array<ref WR_LootItemConfig> m_aItemConfigs;
}