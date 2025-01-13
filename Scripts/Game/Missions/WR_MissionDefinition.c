[BaseContainerProps(configRoot: true), BaseContainerCustomTitleField("m_sName")]
class WR_MissionDefinition
{
	const string CATEGORY_MISSION = "Mission";
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Mission type. When designing a new mission, add your mission type in WR_MissionType.c", enums: ParamEnumArray.FromEnum(WR_MissionType))]
	int m_eType;
	
	[Attribute(defvalue: "", desc: "Mission name. Displayed on the map and in hint notifications.", category: "Mission")]
	string m_sName;
	
	[Attribute(defvalue: "", desc: "Mission description. Displayed on the map and in hint notifications.", category: "Mission")]
	string m_sDescription;

	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "List of AI group prefabs that spawn at this mission.", params: "et class=SCR_AIGroup")]
	ref array<ResourceName> m_aAIGroupPrefabs;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Prop to spawn at mission site (crashed helicopter, abandoned base)", params: "et")]
	ResourceName m_sPropPrefab;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Possible mission rewards. Rewards are chosen from this list at random", params: "et")]
	ref array<ResourceName> m_sRewardPrefabChoices;
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Slider, params: "0 5 1", desc: "Number of rewards to spawn.")]
	int m_iNumberOfRewards;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Which loot context to use for reward loot boxes (set to None if no boxes are to be spawned)", enums: ParamEnumArray.FromEnum(WR_LootContext))]
	int m_eLootContext;
	
	[Attribute(defvalue: "3", uiwidget: UIWidgets.Slider, params: "0 15 1", desc: "Minimum items to spawn from loot context.")]
	int m_iMinItemsInBox;
	
	[Attribute(defvalue: "6", uiwidget: UIWidgets.Slider, params: "0 15 1", desc: "Maximum items to spawn from loot context.")]
	int m_iMaxItemsInBox;
	
	[Attribute(defvalue: "LARGE", uiwidget: UIWidgets.ComboBox, desc: "Size of the mission location. Determines which missions spawn here. Does NOT affect the radius of the location.", enums: ParamEnumArray.FromEnum(WR_MissionLocationSize))]
	int m_eSize;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Which icon shape to use for the mission on the map", enums: ParamEnumArray.FromEnum(WR_MapMarkerConfigQuadIndices))]
	int m_eMissionIcon;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Which icon color to use for the mission on the map", enums: ParamEnumArray.FromEnum(WR_MapMarkerConfigColorIndices))]
	int m_eMissionColor;
	
	// TODO: add options for mission marker icon and color.
	// If left blank, default to what's specified in the controller DSConfig
	// And if that's blank, then use the classic red cross
	
	// TODO: add options for loot contexts, amount of loot to spawn, etc.
	
	// TODO: see if we can add categories to this config
}