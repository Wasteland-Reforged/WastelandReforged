class WR_SpawnPointClass : SCR_SpawnPointClass {}

class WR_SpawnPoint : SCR_SpawnPoint
{
	[Attribute("", UIWidgets.CheckBox)]
	private bool m_bIsSpawnLobbySpawnPoint;
	
	[Attribute("", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(WR_SpawnRegion))]
	private WR_SpawnRegion m_eSpawnRegion;
	
	bool IsSpawnLobbySpawnPoint()
	{
		return m_bIsSpawnLobbySpawnPoint;
	}
	
	WR_SpawnRegion GetSpawnRegion()
	{
		return m_eSpawnRegion;
	}
	
	override string GetFactionKey()
	{
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController)
			return m_sFaction;
		
		SCR_PlayerFactionAffiliationComponent playerFactionAffil = SCR_PlayerFactionAffiliationComponent.Cast(playerController.FindComponent(SCR_PlayerFactionAffiliationComponent));
		Faction playerFaction = playerFactionAffil.GetAffiliatedFaction();
		
		if (playerFaction)
			return playerFaction.GetFactionKey();
		
		return m_sFaction;
	}
}