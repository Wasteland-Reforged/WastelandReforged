modded class SCR_SpawnPoint
{
	//------------------------------------------------------------------------------------------------
	//! Get spawn points valid for given faction
	//! \param factionKey Valid faction key
	override static array<SCR_SpawnPoint> GetSpawnPointsForFaction(string factionKey)
	{
		array<SCR_SpawnPoint> spawnPoints = GetAllFactionSpawnPoints(factionKey);
		
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		Faction playerFaction = factionManager.GetFactionByKey(factionKey);
		
		// For factions friendly to themselves, return all available spawn points.
		if (playerFaction.IsFactionFriendly(playerFaction))
			return spawnPoints;
		
		// For factions not friendly to themselves, we need to filter out spawn points on base parts not built by members of the player's group.
		SCR_GroupsManagerComponent groupManager = SCR_GroupsManagerComponent.GetInstance();	
		
		foreach (SCR_SpawnPoint spawnPoint : spawnPoints)
		{
			IEntity spawnBaseComponent = spawnPoint.GetParent();
			if (!spawnBaseComponent)
			{
				// This spawn point is a standalone entity. OK.
				continue;
			}
				
			SCR_EditableEntityComponent editableEntityComponent = SCR_EditableEntityComponent.Cast(spawnBaseComponent.FindComponent(SCR_EditableEntityComponent));
			if (!editableEntityComponent)
			{
				// This spawn point is not an entity within a base part with spawning capabilities. OK.
				continue;
			}
				
			int builderPlayerId = editableEntityComponent.GetAuthorPlayerID();
			SCR_AIGroup builderPlayerGroup = groupManager.GetPlayerGroup(builderPlayerId);
			if (!builderPlayerGroup)
			{
				// This spawn point's owner has no group. Disqualify.
				spawnPoints.RemoveItem(spawnPoint);
				continue;
			}
			
			SCR_AIGroup thisPlayerGroup = groupManager.GetPlayerGroup(SCR_PlayerController.GetLocalPlayerId());
			if (thisPlayerGroup != builderPlayerGroup)
			{
				// This spawn point's owner is not in the same group as the player. Disqualify.
				spawnPoints.RemoveItem(spawnPoint);
				continue;
			}
		}
		
		return spawnPoints;
	}
	
	private static array<SCR_SpawnPoint> GetAllFactionSpawnPoints(string factionKey)
	{
		array<SCR_SpawnPoint> factionSpawnPoints = {};
		if (factionKey.IsEmpty())
			return factionSpawnPoints;

		array<SCR_SpawnPoint> spawnPoints = GetSpawnPoints();
		foreach (SCR_SpawnPoint spawnPoint : spawnPoints)
		{
			if (!spawnPoint)
				continue;
			
			WR_SpawnPoint wrSpawnPoint = WR_SpawnPoint.Cast(spawnPoint);
			if (wrSpawnPoint)
			{
				if (!wrSpawnPoint.IsSpawnLobbySpawnPoint())
				{
					factionSpawnPoints.Insert(spawnPoint);
				}
				continue;
			}

			if (spawnPoint.GetFactionKey() == factionKey)
			{
				factionSpawnPoints.Insert(spawnPoint);
			}
		}
		
		return factionSpawnPoints;
	}
}
