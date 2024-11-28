class WR_GameModeWastelandClass : SCR_BaseGameModeClass {}

class WR_GameModeWasteland : SCR_BaseGameMode
{
	const string CATEGORY_WR = "Wasteland";
	
	[Attribute("", UIWidgets.Object, "Mission system parameters.", category: CATEGORY_WR)]
	protected ref WR_MissionSystemConfig m_MissionSystemConfiguration;
	
	[Attribute("0", desc: "Enables brighter lighting at night time.", category: CATEGORY_WR)]
	protected bool m_bEnableBrightNightMode;
	
	ref WR_Logger<WR_GameModeWasteland> logger = new WR_Logger<WR_GameModeWasteland>(this);
	
	override void OnGameStart()
	{
		super.OnGameStart();
		
		if (!m_MissionSystemConfiguration)
		{
			logger.LogError("No mission system config was specified! Cannot create any missions.");
		}
		
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
	
	WR_MissionSystemConfig GetMissionSystemConfig()
	{
		return m_MissionSystemConfiguration;
	}
}