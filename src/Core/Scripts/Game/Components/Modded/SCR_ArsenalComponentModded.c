modded class SCR_ArsenalComponent : ScriptComponent
{
	override bool GetFilteredArsenalItems(out notnull array<SCR_ArsenalItem> filteredArsenalItems, EArsenalItemDisplayType requiresDisplayType = -1)
	{
		//~ Is overwritting Arsenal
		if (m_OverwriteArsenalConfig)
		{
			GetFilteredOverwriteArsenalItems(filteredArsenalItems, requiresDisplayType);
			return !filteredArsenalItems.IsEmpty();
		}

		//~ Cannot get items if no entity catalog manager
		SCR_EntityCatalogManagerComponent catalogManager = SCR_EntityCatalogManagerComponent.GetInstance();
		if (!catalogManager)
			return false;

		//~ Get filtered list from faction (if any) or default
		filteredArsenalItems = catalogManager.GetFilteredArsenalItems(m_eSupportedArsenalItemTypes, m_eSupportedArsenalItemModes, SCR_ArsenalManagerComponent.GetArsenalGameModeType_Static(), SCR_Faction.Cast(m_FactionManager.GetFactionByKey("US")), requiresDisplayType);
		return !filteredArsenalItems.IsEmpty();
	}
	
}