class WR_LootSystem : GameSystem
{
	float m_fTimeElaspedS = 0.0;
	
	float m_tickRateS = 60*3;
	
	ref array<WR_LootSpawner> m_aLootSpawners = {};

	protected override void OnUpdate(ESystemPoint point)
	{
		float timeSlice = GetWorld().GetTimeSlice();
		m_fTimeElaspedS += timeSlice;
		
		if (m_fTimeElaspedS >= m_tickRateS)
		{
			BeginUpdate();
			Print("[WASTELAND] Spawning loot at " + m_aLootSpawners.Count() + " loot spawners...");
			
			int successfulSpawns;
			RefreshAllLootSpawners(successfulSpawns);
			
			Update(); //< This is the call where entities are updated in batch
			EndUpdate();
			
			Print("[WASTELAND] Loot spawned at " + successfulSpawns + " loot spawners.");
			m_fTimeElaspedS = 0.0;
		}
	}
	
	protected int RefreshAllLootSpawners(out int successfulSpawns)
	{
		int itemsSpawned = 0;
		
		foreach (WR_LootSpawner ls : m_aLootSpawners)
		{
			bool success = ls.SpawnItem();
			AddEntity(ls);
			if (success) itemsSpawned++;
		}
		
		return itemsSpawned;
	}
}

class WR_LootSpawner : IEntity
{
	bool SpawnItem() {}
}