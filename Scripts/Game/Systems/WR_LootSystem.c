class WR_LootSystem : GameSystem
{
	ref WR_Logger<WR_LootSystem> logger = new WR_Logger<WR_LootSystem>(this);
	
	float m_fTimeElaspedS = 0;
	
	float m_tickRateS = 60;
	
	private static int m_iSuccessfulSpawns = 0;
	private static float subsequentLootSpawnChance = 0.01;
	private static float initialLootSpawnChance = 0.75;
	
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
		
		logger.LogNormal("Spawning items at loot spawners...");
		m_iSuccessfulSpawns = 0;
		
		foreach (WR_LootSpawner ls : m_aLootSpawners)
		{
			if (!ls)
				continue;
			if (Math.RandomFloat01() > spawnChance)
				continue;
			
			ls.TrySpawnLoot();
			AddEntity(ls);
		}
		
		logger.LogNormal("Items spawned at " + m_iSuccessfulSpawns + " of " + m_aLootSpawners.Count() + " loot spawners.");
	}

	static void CountSuccessfulSpawn()
	{
		m_iSuccessfulSpawns++;
	}
}