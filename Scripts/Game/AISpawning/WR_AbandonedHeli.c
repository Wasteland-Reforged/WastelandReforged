class WR_AbandonedHeli : WR_Mission
{
	void WR_AbandonedHeli(string name, vector pos)
	{
		Print("Spawning Abandoned Heli..");
		spawnHeli();
		createCompletionTrigger();
		createMissionMarker(4, 18);
	}
	
	protected void spawnHeli()
	{
		array<ResourceName> heliList = {
			"{70BAEEFC2D3FEE64}Prefabs/Vehicles/Helicopters/UH1H/UH1H.et",
			"{DF5CCB7C0FF049F4}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport.et"
		};
		
		//Find safe position to spawn heli
		vector spawnPos;
		bool foundSafePos = SCR_WorldTools.FindEmptyTerrainPosition(spawnPos, missionLocation, 25, 5, 5);
		if (!foundSafePos) {
			Print("[WR_AIMission]: Could not find safe place to spawn heli");
			return;
		}
		
		//Load resource and generate spawn parameters
		Resource resource = Resource.Load(heliList.GetRandomElement());
		EntitySpawnParams params = GenerateSpawnParameters(missionLocation);
		
		//Spawn wreck and give it a random rotation
		IEntity heli = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		heli.SetYawPitchRoll(WR_Utils.GetRandomHorizontalDirectionAngles());
		missionEntityList.Insert(heli);
	}

}