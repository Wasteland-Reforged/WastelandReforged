class WR_MapMarkerComponentClass : ScriptComponentClass {}

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
	
	private ref SCR_MapMarkerBase m_MapMarker;
	
	override void OnPostInit(IEntity owner)
	{
		if (m_MapMarker)
			return;
		
		BaseRplComponent rpl = BaseRplComponent.Cast(owner.FindComponent(BaseRplComponent));
		if (!rpl || rpl.Role() != RplRole.Authority)
			return;
		
		SCR_MapMarkerManagerComponent mgr = SCR_MapMarkerManagerComponent.GetInstance();
		if (!mgr)
			return;
		
		m_MapMarker = new SCR_MapMarkerBase();
		
		vector pos = owner.GetOrigin();
		m_MapMarker.SetWorldPos(pos[0], pos[2]);
		m_MapMarker.SetType(SCR_EMapMarkerType.PLACED_CUSTOM);
		
		// Custom parameters
		m_MapMarker.SetIconEntry(m_eIcon);
		m_MapMarker.SetColorEntry(m_eColor);
		m_MapMarker.SetCustomText(m_sLabel);

		mgr.InsertStaticMarker(m_MapMarker, isLocal: false, isServerMarker: true);
	}
}