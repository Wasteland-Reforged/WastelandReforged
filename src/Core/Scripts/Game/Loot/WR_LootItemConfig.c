[BaseContainerProps(configRoot: true)]
class WR_LootItemConfig
{
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "et")]
	ResourceName m_sItemPrefab;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(WR_LootCategory))]
	int m_eCategory;
	
	[Attribute(defvalue: "20", uiwidget: UIWidgets.Slider, params: "0 100 1", desc: "Weight relative to other items in same category.")]
	int m_iWeight;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "et", desc: "All additional items will be copies of a single entry in this list, chosen randomly, equal weights")]
	ref array<ResourceName> m_aAdditionalItemChoices;
	
	[Attribute("0", desc: "Guarantee that at all additional items will attempt to spawn. Useful for vehicle loadouts.")]
	bool m_bAlwaysSpawnAllAdditionalItems;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.Slider, params: "0 100 1")]
	int m_iMinAdditionalItems;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.Slider, params: "0 100 1")]
	int m_iMaxAdditionalItems;
}