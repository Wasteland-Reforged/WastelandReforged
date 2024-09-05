[ComponentEditorProps(category: "Tutorial/Component", description: "Warn then teleport humans that are too close to the entity")]
class WR_MapMarkerComponentClass : ScriptComponentClass
{
	
}

class WR_MapMarkerComponent : ScriptComponent
{
	// Categories
	private const string CATEGORY_MAP_MARKER = "Map Marker";
	
	[Attribute(defvalue: "", desc: "Marker text", category: CATEGORY_MAP_MARKER)]
	protected string m_sLabel;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Marker icon", enums: ParamEnumArray.FromEnum(WR_MapMarkerConfigQuadIndices), category: CATEGORY_MAP_MARKER)]
	private WR_MapMarkerConfigQuadIndices m_eIcon;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ComboBox, desc: "Marker color", enums: ParamEnumArray.FromEnum(WR_MapMarkerConfigColorIndices), category: CATEGORY_MAP_MARKER)]
	private WR_MapMarkerConfigColorIndices m_eColor;
	
	override void OnPostInit(IEntity owner)
	{
		BaseRplComponent rpl = BaseRplComponent.Cast(owner.FindComponent(BaseRplComponent));
		if (!rpl || rpl.Role() != RplRole.Authority)
			return;
		
		SCR_MapMarkerManagerComponent mgr = SCR_MapMarkerManagerComponent.GetInstance();
		if (!mgr)
			return;
		
		SCR_MapMarkerBase marker = new SCR_MapMarkerBase();
		
		vector pos = owner.GetOrigin();
		marker.SetWorldPos(pos[0], pos[2]);
		marker.SetType(SCR_EMapMarkerType.PLACED_CUSTOM);
		
		// Custom parameters
		marker.SetIconEntry(m_eIcon);
		marker.SetColorEntry(m_eColor);
		marker.SetCustomText(m_sLabel);

		mgr.InsertStaticMarker(marker, isLocal: false, isServerMarker: true);
	}
}