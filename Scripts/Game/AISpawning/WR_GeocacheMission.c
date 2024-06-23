class WR_GeocacheMission : WR_Mission
{
	void WR_GeocacheMission(string name, vector pos)
	{
		createGeocache();
		createCompletionTrigger();
		createMissionMarker(1, 11);
	}
	
	protected void createGeocache()
	{
		spawnEquipmentCrates("Regular", 1, 5, 10);
	}

}