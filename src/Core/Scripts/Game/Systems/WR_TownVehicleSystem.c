class WR_TownVehicleSystem : GameSystem
{
	ref WR_Logger<WR_TownVehicleSystem> logger = new WR_Logger<WR_TownVehicleSystem>(this);
	
	float m_fTimeElaspedS = 0;
	float m_tickRateS = 15;
	
	private static int m_iSuccessfulSpawns = 0;
	
	private ref static array<WR_SpawnAreaVehicleSpawnHandlerComponent> s_aVehicleSpawnHandlerComponents = {};

	protected override void OnStarted()
	{
		logger.LogNormal("Starting town vehicle system...");
		
		if (!s_aVehicleSpawnHandlerComponents || s_aVehicleSpawnHandlerComponents.Count() == 0)
		{
			logger.LogWarning("No spawn areas with WR_SpawnAreaVehicleSpawnHandlerComponent found! Cannot start roaming bot system.");
			return;
		}
		
		CheckTownVehicles();
		
		logger.LogNormal("Town vehicle system started.");
	}
	
	protected override void OnUpdate(ESystemPoint point)
	{
		float timeSlice = GetWorld().GetTimeSlice();
		m_fTimeElaspedS += timeSlice;
		
		if (m_fTimeElaspedS >= m_tickRateS)
		{
			BeginUpdate();
			
			CheckTownVehicles();
			
			Update();
			EndUpdate();
			
			m_fTimeElaspedS = 0.0;
		}
	}
	
	protected override void OnCleanup()
	{
		s_aVehicleSpawnHandlerComponents = {};
		logger.LogNormal("Town vehicle system cleaned up.");
	}
	
	protected void CheckTownVehicles()
	{
		foreach (WR_SpawnAreaVehicleSpawnHandlerComponent townVehicleComp : s_aVehicleSpawnHandlerComponents)
		{
			townVehicleComp.CheckVehicles();
		}
	}
	
	static void InsertVehicleSpawnHandlerComponent(WR_SpawnAreaVehicleSpawnHandlerComponent handlerComponent)
	{
		if (!s_aVehicleSpawnHandlerComponents.Contains(handlerComponent))
			s_aVehicleSpawnHandlerComponents.Insert(handlerComponent);
	}
	
	static void RemoveVehicleSpawnHandlerComponent(WR_SpawnAreaVehicleSpawnHandlerComponent handlerComponent)
	{
		s_aVehicleSpawnHandlerComponents.RemoveItem(handlerComponent);
	}
	
	static void CountSuccessfulSpawn()
	{
		m_iSuccessfulSpawns++;
	}
}