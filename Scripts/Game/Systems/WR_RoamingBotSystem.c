class WR_RoamingBotSystem : GameSystem
{
	ref WR_Logger<WR_RoamingBotSystem> logger = new WR_Logger<WR_RoamingBotSystem>(this);
	
	float m_fTimeElaspedS = 0;
	float m_tickRateS = 30;
	
	protected override void OnStarted()
	{
		logger.LogNormal("Roaming bot system started.");
		CheckSpawnAreas();
	}
	
	protected override void OnUpdate(ESystemPoint point)
	{
		float timeSlice = GetWorld().GetTimeSlice();
		m_fTimeElaspedS += timeSlice;
		
		if (m_fTimeElaspedS >= m_tickRateS)
		{
			BeginUpdate();
			
			CheckSpawnAreas();
			
			Update();
			EndUpdate();
			
			m_fTimeElaspedS = 0.0;
		}
	}
	
	protected void CheckSpawnAreas()
	{
		logger.LogNormal("Checking Spawn Areas.");
		
		foreach (WR_SpawnAreaAISpawnHandlerComponent spawnArea : WR_SpawnAreaAISpawnHandlerComponent.m_aBotSpawnHandlers)
		{
			if (spawnArea)					//TODO: Figure out why the first set of entries in the array are set to null
				spawnArea.CheckGroups(true);
		}
	}
	
	protected override void OnCleanup()
	{
		logger.LogNormal("Roaming bot system cleaned up.");
	}
}