[BaseContainerProps(configRoot: true)]
class WR_LootItemsConfig
{
	[Attribute("", UIWidgets.Object, desc: "List of all loot objects that can spawn.")]
	ref array<ref WR_LootItemConfig> m_aLootItemConfigs;
}