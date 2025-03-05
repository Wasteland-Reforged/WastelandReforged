modded class SCR_SpawnPoint
{
	//------------------------------------------------------------------------------------------------
	//! Get spawn points valid for given faction
	//! \param factionKey Valid faction key
	override static array<SCR_SpawnPoint> GetSpawnPointsForFaction(string factionKey)
	{
		int thisPlayerId = SCR_PlayerController.GetLocalPlayerId();

		array<SCR_SpawnPoint> spawnPoints = GetAllFactionSpawnPoints(factionKey);
		
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		Faction playerFaction = factionManager.GetFactionByKey(factionKey);
		
		// For factions friendly to themselves, show all available spawn points.
		if (playerFaction.IsFactionFriendly(playerFaction))
			return spawnPoints;
		
		// For factions not friendly to themselves, filter out spawn points not built by group members.
		PlayerManager playerManager = GetGame().GetPlayerManager();
		SCR_GroupsManagerComponent groupManager = SCR_GroupsManagerComponent.GetInstance();
		
		SCR_AIGroup thisPlayerGroup = groupManager.GetPlayerGroup(thisPlayerId);
		if (!thisPlayerGroup)
			return spawnPoints;
		
		foreach (SCR_SpawnPoint spawnPoint : spawnPoints)
		{
			IEntity spawnBaseComponent = spawnPoint.GetParent();
			if (!spawnBaseComponent)
				continue;
			
			SCR_EditableEntityComponent editableEntityComponent = SCR_EditableEntityComponent.Cast(spawnBaseComponent.FindComponent(SCR_EditableEntityComponent));
			if (!editableEntityComponent)
				continue;
			
			int builderPlayerId = editableEntityComponent.GetAuthorPlayerID();
			
			SCR_AIGroup builderPlayerGroup = groupManager.GetPlayerGroup(builderPlayerId);
			if (!thisPlayerGroup)
				return spawnPoints;
			
			// Remove spawn point if not owned by a group member
			if (thisPlayerGroup != builderPlayerGroup)
				spawnPoints.RemoveItem(spawnPoint);
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
