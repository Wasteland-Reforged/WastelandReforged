modded class SCR_MapUIElementContainer : SCR_MapUIBaseComponent
{
	override protected void ShowSpawnPoint(notnull SCR_SpawnPoint spawnPoint)
	{
		if (!m_bIsDeployMap && spawnPoint.GetVisibleInDeployMapOnly())
			return;
		
		WR_SpawnPoint wrSpawnPoint = WR_SpawnPoint.Cast(spawnPoint);
		if (wrSpawnPoint)
			return; // Do not render WR_SpawnPoints on map

		if (!spawnPoint.IsSpawnPointVisibleForPlayer(SCR_PlayerController.GetLocalPlayerId()))
			return;

		Widget w = GetGame().GetWorkspace().CreateWidgets(m_sSpawnPointElement, m_wIconsContainer);
		SCR_MapUISpawnPoint handler = SCR_MapUISpawnPoint.Cast(w.FindHandler(SCR_MapUISpawnPoint));
		if (!handler)
			return;

		handler.Init(spawnPoint);
		handler.SetParent(this);
		m_mIcons.Set(w, handler);

		FrameSlot.SetSizeToContent(w, true);
		FrameSlot.SetAlignment(w, 0.5, 0.5);
	}
}
