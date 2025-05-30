class WR_GameModeWastelandClass : SCR_BaseGameModeClass {}

class WR_GameModeWasteland : SCR_BaseGameMode
{
	const string CATEGORY_WR = "Wasteland";
	
	private ref WR_Logger<WR_GameModeWasteland> logger = new WR_Logger<WR_GameModeWasteland>(this);
	private IEntity m_SpawnLobby;
	
	[Attribute("0", desc: "Enables brighter lighting at night time.", category: CATEGORY_WR)]
	protected bool m_bEnableBrightNightMode;
	
	[Attribute("25", desc: "Supply cost to respawn at buildable HQ tents.", category: CATEGORY_WR)]
	protected int m_iTentRespawnSupplyCost;
	
	override void OnGameStart()
	{
		super.OnGameStart();
		
		if (m_bEnableBrightNightMode)
		{
			SCR_NightModeGameModeComponent nightModeComponent = SCR_NightModeGameModeComponent.Cast(FindComponent(SCR_NightModeGameModeComponent));
			if (!nightModeComponent)
			{
				logger.LogError("No NightModeGameModeComponent found! Cannot enable global bright night mode.");
			}
			else
			{
				nightModeComponent.EnableGlobalNightMode(true);
			}
		}
		
		// Attempt to get spawn bunker entity. The presence of the spawn bunker determines the spawn flow.
		m_SpawnLobby = GetGame().GetWorld().FindEntityByName("SpawnLobby");
		if (m_SpawnLobby)
			logger.LogNormal("A spawn lobby was found in the world. The lobby spawn flow will be used for this session.");
		else
			logger.LogNormal("No spawn lobby was found in the world. The vanilla deployment map spawn flow will be used for this session.");

		logger.LogNormal("Game mode started.");
	}
	
	int GetTentRespawnSupplyCost()
	{
		return m_iTentRespawnSupplyCost;
	}
	
	bool IsSpawnLobbyPresent()
	{
		return m_SpawnLobby != null;
	}
}