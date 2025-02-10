class WR_RespawnIndicatorClass : ChimeraMenuBase
{
	protected static const string TEXT_TITLE = "TextTitle";
	
	PlayerController pc;
	IEntity oldChar;

	//------------------------------------------------------------------------------------------------
	protected override void OnMenuOpen()
	{
		RequestCustomRespawn(SpawnAreaCategory.SPAWN_NORTH);
		
		Widget rootWidget = GetRootWidget();
		if (!rootWidget) return;
	}
	
	protected void RequestCustomRespawn(SpawnAreaCategory spawnCategory)
	{
		Widget rootWidget = GetRootWidget();
		if (!rootWidget) return;
		
		// Change title text
		TextWidget textTitle = TextWidget.Cast(rootWidget.FindWidget(TEXT_TITLE));
		textTitle.SetText("Waiting for respawn...");
		
		// Respawn player
		PlayerController pc = GetGame().GetPlayerController();
		SCR_RespawnComponent m_SpawnRequestManager = SCR_RespawnComponent.Cast(pc.GetRespawnComponent());
		
		SCR_PlayerFactionAffiliationComponent m_PlyFactionAffilComp = SCR_PlayerFactionAffiliationComponent.Cast(pc.FindComponent(SCR_PlayerFactionAffiliationComponent));
		
		m_SpawnRequestManager.SGetOnLocalPlayerSpawned().Insert(Close);

		string factionKey = m_PlyFactionAffilComp.GetAffiliatedFaction().GetFactionKey();
		SCR_FreeSpawnData rspData = WR_SpawnAreaPlayerSpawnHandlerComponent.GetSpawnData(factionKey, spawnCategory);
		if (rspData)
			m_SpawnRequestManager.RequestSpawn(rspData);	
	}
}

modded enum ChimeraMenuPreset
{
	WR_SpawnProgressUi_Id
}

