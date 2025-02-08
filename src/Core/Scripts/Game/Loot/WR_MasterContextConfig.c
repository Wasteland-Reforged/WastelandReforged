[BaseContainerProps(configRoot: true)]
class WR_MasterContextConfig
{
	[Attribute("", UIWidgets.Object, desc: "List of loot contexts, each with their own list of weights for which loot categories they can spawn.")]
	ref array<ref WR_LootContextConfig> m_aLootContextConfigs;

}