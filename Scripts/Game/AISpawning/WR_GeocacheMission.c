class WR_GeocacheMission : WR_Mission
{
	void WR_GeocacheMission(string name, vector pos)
	{
		createGeocache();
		createCompletionTrigger();
		createMissionMarker(4, 18);
	}
	
	protected void createGeocache()
	{
		spawnEquipmentCrates("Regular", 1, 5, 10);
	}

}