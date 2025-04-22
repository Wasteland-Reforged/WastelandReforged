[BaseContainerProps(configRoot: true), BaseContainerCustomTitleField("m_sName")]
class WR_MissionDefinition
{
	const string CATEGORY_MISSION = "Mission";
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Mission type. When creating a new mission, make sure to add your mission type to the enum in WR_MissionType.c", enums: ParamEnumArray.FromEnum(WR_MissionType))]
	int m_eType;
	
	[Attribute(defvalue: "", desc: "Mission name. Displayed on the map and in hint notifications.", category: "Mission")]
	string m_sName;
	
	[Attribute(defvalue: "", desc: "Mission description. Displayed on the map and in hint notifications.", category: "Mission")]
	string m_sDescription;

	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "List of NPC group prefabs to spawn at this mission. Leave empty for no NPCs to spawn.", params: "et class=SCR_AIGroup")]
	ref array<ResourceName> m_aAIGroupPrefabs;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Prop to spawn at mission site (e.g. crashed helicopter, abandoned base).", params: "et")]
	ResourceName m_sPropPrefab;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Possible mission rewards. Rewards are chosen from this list at random", params: "et")]
	ref array<ResourceName> m_sRewardPrefabChoices; // TODO: make this a weighted array
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Slider, params: "0 5 1", desc: "Number of rewards to spawn.")]
	int m_iNumberOfRewards;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Loot context to use for reward loot boxes. Set to none for no boxes to be spawned.", enums: ParamEnumArray.FromEnum(WR_LootContext))]
	int m_eLootContext;
	
	[Attribute(defvalue: "0.75", uiwidget: UIWidgets.Slider, params: "0 2.0 0.1", desc: "Amount of loot to spawn in the reward box if present.")]
	float m_fLootBudget;
	
	[Attribute(defvalue: "1.0", uiwidget: UIWidgets.Slider, params: "0 10.0 0.1", desc: "Multiplier for magazines and other additional items in reward box if present.")]
	float m_fLootBoxAdditionalItemMultiplier;
	
	[Attribute(defvalue: "LARGE", uiwidget: UIWidgets.ComboBox, desc: "Size of the mission location. Determines which missions spawn here. Does NOT affect the radius of the location.", enums: ParamEnumArray.FromEnum(WR_MissionLocationSize))]
	int m_eSize;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Icon shape to use for the mission map marker", enums: ParamEnumArray.FromEnum(WR_MapMarkerConfigQuadIndices))]
	int m_eMissionIcon;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Icon color to use for the mission map marker", enums: ParamEnumArray.FromEnum(WR_MapMarkerConfigColorIndices))]
	int m_eMissionColor;
}