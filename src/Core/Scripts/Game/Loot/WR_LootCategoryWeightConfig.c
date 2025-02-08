[BaseContainerProps(configRoot: true)]
class WR_LootCategoryWeightConfig
{
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(WR_LootCategory))]
	int m_eCategory;
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Slider, params: "0 100 1", desc: "Weight relative to other categories in this context.")]
	int m_iWeight;
}