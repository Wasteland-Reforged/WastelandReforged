[BaseContainerProps(configRoot: true)]
class WR_LootItemConfig
{
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "et")]
	ResourceName m_sItemPrefab;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(WR_LootCategory))]
	int m_eCategory;
	
	[Attribute(defvalue: "20", uiwidget: UIWidgets.Slider, params: "0 100 1", desc: "Weight relative to other items in same category.")]
	int m_iWeight;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.Slider, params: "0 10 1")]
	int m_iMinAdditionalItems;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.Slider, params: "0 10 1")]
	int m_iMaxAdditionalItems;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "et", desc: "ALL additional items will be copies of a single entry in this list, chosen randomly, equal weights")]
	ref array<ResourceName> m_aAdditionalItemChoices;
}