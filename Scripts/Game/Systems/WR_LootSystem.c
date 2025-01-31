class WR_LootSystem : GameSystem
{
	ref WR_Logger<WR_LootSystem> logger = new WR_Logger<WR_LootSystem>(this);
	
	float m_fTimeElaspedS = 0;
	float m_fTotalTimeS = 0;
	
	float m_tickRateS = 30;
	
	private static int m_iSuccessfulSpawns = 0;
	private static float subsequentLootSpawnChance = 0.1;	//Chance for future empty loot spawners to be given new loot
	private static float TimedOutLootRespawnChance = 0.3;	//Chance for an item that has timed out to be respawned
	private static float initialLootSpawnChance = 0.8;		// Percent of loot spawners that have loot when the server starts
	
	static ref array<WR_LootSpawner> m_aLootSpawners;
	
	private const string LogPrefix = "WR_LootSystem: ";

	protected override void OnStarted()
	{
		logger.LogNormal("Loot system started.");
		RefreshLootSpawners(initialLootSpawnChance);
	}
	
	protected override void OnUpdate(ESystemPoint point)
	{
		float timeSlice = GetWorld().GetTimeSlice();
		m_fTimeElaspedS += timeSlice;
		m_fTotalTimeS += timeSlice;
		
		if (m_fTimeElaspedS >= m_tickRateS)
		{
			BeginUpdate();
			
			RefreshLootSpawners(subsequentLootSpawnChance);
			
			Update();
			EndUpdate();
			
			m_fTimeElaspedS = 0.0;
		}
	}
	
	protected override void OnCleanup()
	{
		m_aLootSpawners = null;
		logger.LogNormal("Loot system cleaned up.");
	}
	
	protected void RefreshLootSpawners(float spawnChance)
	{
		if (!m_aLootSpawners || m_aLootSpawners.Count() == 0)
			return;
		
		logger.LogDebug("Spawning items at loot spawners...");
		m_iSuccessfulSpawns = 0;
		
		foreach (WR_LootSpawner ls : m_aLootSpawners)
		{
			if (!ls)
				continue;
			
			ls.TrySpawnLootTimeout(m_fTotalTimeS, spawnChance, TimedOutLootRespawnChance);
			AddEntity(ls);
		}
		
		logger.LogDebug("Items spawned at " + m_iSuccessfulSpawns + " of " + m_aLootSpawners.Count() + " loot spawners.");
	}

	static void CountSuccessfulSpawn()
	{
		m_iSuccessfulSpawns++;
	}
}