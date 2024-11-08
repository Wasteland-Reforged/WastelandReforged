class WR_LootSystem : GameSystem
{
	float m_fTimeElaspedS = 0;
	
	float m_tickRateS = 60;
	
	private static int m_iSuccessfulSpawns = 0;
	private static float subsequentLootSpawnChance = 0.01;
	private static float initialLootSpawnChance = 0.40;
	
	static ref array<WR_LootSpawner> m_aLootSpawners;
	
	private const string LogPrefix = "WR_LootSystem: ";

	protected override void OnStarted()
	{
		WR_Logger.LogNormal(LogPrefix + "Loot system started.");
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
		WR_Logger.LogNormal(LogPrefix + "Loot system cleaned up.");
	}
	
	protected void RefreshLootSpawners(float spawnChance)
	{
		if (!m_aLootSpawners || m_aLootSpawners.Count() == 0)
			return;
		
		WR_Logger.LogNormal(LogPrefix + "Spawning items at loot spawners...");
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
		
		WR_Logger.LogNormal(LogPrefix + "Items spawned at " + m_iSuccessfulSpawns + " of " + m_aLootSpawners.Count() + " loot spawners.");
	}

	static void CountSuccessfulSpawn()
	{
		m_iSuccessfulSpawns++;
	}
}