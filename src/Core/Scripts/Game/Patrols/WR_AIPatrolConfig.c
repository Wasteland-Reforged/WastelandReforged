[BaseContainerProps(configRoot: true)]
class WR_AIPatrolConfig
{
	[Attribute(defvalue: "3", uiwidget: UIWidgets.Slider, params: "0 10 1", desc: "Max amount of patrols to be active at once.")]
	int m_iMaxPatrols;
	
	[Attribute(defvalue: "", desc: "AI Groups that can be spawned with the patrol", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "et")]
	ref array<ResourceName> m_aAIGroupChoices;
	
	[Attribute(defvalue: "", desc: "Vehicles that can be spawned with the patrol", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "et")]
	ref array<ResourceName> m_aPatrolVehicleChoices;
}