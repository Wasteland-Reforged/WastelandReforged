[BaseContainerProps(configRoot: true)]
class PAND_MissionControllerConfig
{
	[Attribute(defvalue: "3", uiwidget: UIWidgets.Slider, params: "0 10 1", desc: "Maximum number of missions that can be in progress simultaneously.")]
	int m_iMaxActiveMissions;
	
	[Attribute(defvalue: "2", uiwidget: UIWidgets.Slider, params: "0 30 0.25", desc: "(m) How long to wait after game mode start before starting missions")]
	float m_fInitialMissionDelay;
	
	[Attribute(defvalue: "5", uiwidget: UIWidgets.Slider, params: "0.25 30 0.25", desc: "(m) How long to wait to start a new mission after one ends.")]
	float m_fNewMissionDelay;
	
	[Attribute("", UIWidgets.Object, desc: "List of all mission definitions that can spawn.")]
	ref array<ref PAND_MissionDefinition> m_aMissionDefinitions;
}