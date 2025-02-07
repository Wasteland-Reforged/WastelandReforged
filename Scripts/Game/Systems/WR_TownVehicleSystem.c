class WR_TownVehicleSystem : GameSystem
{
	ref WR_Logger<WR_TownVehicleSystem> logger = new WR_Logger<WR_TownVehicleSystem>(this);
	
	float m_fTimeElaspedS = 0;
	float m_tickRateS = 15;
	
	private static int m_iSuccessfulSpawns = 0;
	
	ref static array<WR_SpawnAreaVehicleSpawnHandlerComponent> VehicleSpawnHandlerComponents = {};
	
	private const string LogPrefix = "WR_TownVehicleSystem: ";

	protected override void OnStarted()
	{
		logger.LogNormal("Town Vehicle system started.");
		CheckTownVehicles();
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
		VehicleSpawnHandlerComponents = null;
		logger.LogNormal("Town Vehicle system cleaned up.");
	}
	
	protected void CheckTownVehicles()
	{
		//logger.LogNormal("Checking Town Vehicles.");
		foreach (WR_SpawnAreaVehicleSpawnHandlerComponent townVehicleComp : VehicleSpawnHandlerComponents)
		{
			townVehicleComp.CheckVehicles();
		}
	}

	static void CountSuccessfulSpawn()
	{
		m_iSuccessfulSpawns++;
	}
}