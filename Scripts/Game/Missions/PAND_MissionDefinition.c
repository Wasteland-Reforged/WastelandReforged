[BaseContainerProps(configRoot: true), BaseContainerCustomTitleField("m_sName")]
class PAND_MissionDefinition
{
	const string CATEGORY_MISSION = "Mission";
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Mission type. When designing a new mission, add your mission type in WR_MissionType.c", enums: ParamEnumArray.FromEnum(PAND_MissionType))]
	int m_eType;
	
	[Attribute(defvalue: "", desc: "Mission name. Displayed on the map and in hint notifications.", category: "Mission")]
	string m_sName;
	
	[Attribute(defvalue: "", desc: "Mission description. Displayed on the map and in hint notifications.", category: "Mission")]
	string m_sDescription;

	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "List of AI group prefabs that spawn at this mission.", params: "et class=SCR_AIGroup")]
	ref array<ResourceName> m_aAIGroupPrefabs;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Prop to spawn at mission site (crashed helicopter, abandoned base)", params: "et")]
	ResourceName m_sPropPrefab;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "List of mission rewards (i.e. weapon crate, APC, helicopter). \nPUT PRIMARY REWARD FIRST IN THIS LIST!", params: "et")]
	ref array<ResourceName> m_sRewardPrefabs;
	
	// TODO: add options for mission marker icon and color.
	// If left blank, default to what's specified in the controller DSConfig
	// And if that's blank, then use the classic red cross
	
	// TODO: add options for loot contexts, amount of loot to spawn, etc.
	
	// TODO: see if we can add categories to this config
}