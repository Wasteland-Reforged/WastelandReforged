class WR_GameModeWastelandClass : SCR_BaseGameModeClass {}

class WR_GameModeWasteland : SCR_BaseGameMode
{
	[Attribute("0", desc: "Enables bright lighting at night time.", category: "Settings")]
	protected bool m_bEnableBrightNightMode;
	
	ref WR_Logger<WR_GameModeWasteland> logger = new WR_Logger<WR_GameModeWasteland>(this);
	
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
		
		logger.LogNormal("Game mode started.");
	}
}