class WR_RoamingBotSystem : GameSystem
{
	ref WR_Logger<WR_RoamingBotSystem> logger = new WR_Logger<WR_RoamingBotSystem>(this);
	
	static ref array<WR_SpawnAreaBotSpawnHandlerComponent> s_aBotSpawnHandlers = {};
	
	float m_fTimeElaspedS = 0;
	float m_tickRateS = 3*60;
	
	protected override void OnStarted()
	{
		logger.LogNormal("Starting roaming bot system...");
		
		if (!s_aBotSpawnHandlers || s_aBotSpawnHandlers.Count() == 0)
		{
			logger.LogWarning("No spawn areas with WR_SpawnAreaBotSpawnHandlerComponent found! Cannot start roaming bot system.");
			return;
		}
		
		CheckSpawnAreas();
		
		logger.LogNormal("Roaming bot system started.");
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
	
	protected override void OnCleanup()
	{
		s_aBotSpawnHandlers = {};
		logger.LogNormal("Roaming bot system cleaned up.");
	}

	protected void CheckSpawnAreas()
	{
		logger.LogDebug("Checking spawn areas.");
		
		foreach (WR_SpawnAreaBotSpawnHandlerComponent spawnArea : s_aBotSpawnHandlers)
		{
			if (spawnArea)	//TODO: Figure out why the first set of entries in the array are set to null
				spawnArea.CheckGroups(true);
		}
	}
	
	static void InsertBotSpawnHandlerComponent(WR_SpawnAreaBotSpawnHandlerComponent handlerComponent)
	{
		if (!s_aBotSpawnHandlers.Contains(handlerComponent))
			s_aBotSpawnHandlers.Insert(handlerComponent);
	}
	
	static void RemoveBotSpawnHandlerComponent(WR_SpawnAreaBotSpawnHandlerComponent handlerComponent)
	{
		s_aBotSpawnHandlers.RemoveItem(handlerComponent);
	}
}