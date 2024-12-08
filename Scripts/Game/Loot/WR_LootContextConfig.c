[BaseContainerProps(configRoot: true)]
class WR_LootContextConfig
{
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(WR_LootContext))]
	int m_eContext;
	
	[Attribute("", UIWidgets.Object, desc: "List of categories of loot which can spawn in this context.")]
	ref array<ref WR_LootCategoryWeightConfig> m_aLootCategoryWeightConfigs;
}