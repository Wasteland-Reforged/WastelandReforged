class WR_SpawnPointClass : SCR_SpawnPointClass {}
class WR_SpawnPoint : SCR_SpawnPoint
{
	protected ref WR_Logger<WR_SpawnPoint> logger = new WR_Logger<WR_SpawnPoint>(this);
	
	[Attribute("", UIWidgets.CheckBox)]
	private bool m_bIsSpawnLobbySpawnPoint;
	
	[Attribute("", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(WR_SpawnRegion))]
	private WR_SpawnRegion m_eSpawnRegion;
	
	bool IsSpawnLobbySpawnPoint()
	{
		return m_bIsSpawnLobbySpawnPoint;
	}
	
	WR_SpawnRegion GetSpawnRegion()
	{
		return m_eSpawnRegion;
	}
	
	override string GetFactionKey()
	{
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController)
			return m_sFaction;
		
		SCR_PlayerFactionAffiliationComponent playerFactionAffil = SCR_PlayerFactionAffiliationComponent.Cast(playerController.FindComponent(SCR_PlayerFactionAffiliationComponent));
		Faction playerFaction = playerFactionAffil.GetAffiliatedFaction();
		
		if (playerFaction)
			return playerFaction.GetFactionKey();
		
		return m_sFaction;
	}
}

class WR_ControllableSpawnPointClass : WR_SpawnPointClass {}
class WR_ControllableSpawnPoint : WR_SpawnPoint
{
	//private WR_SpawnAreaEntity parent;
	
//	override void EOnInit(IEntity owner)
//	{
//		super.EOnInit(owner);
//		
//		WR_SpawnAreaEntity parent = WR_SpawnAreaEntity.Cast(GetParent());
//		if (!parent)
//		{
//			logger.LogError("WR_ControllableSpawnPoint must be a child of WR_SpawnAreaEntity!");
//			return;
//		}
//		
//		parent = parent;
//	}
	
	bool IsParentSpawnAreaControlledByFriendlies(int playerId)
	{
		WR_SpawnAreaEntity parent = WR_SpawnAreaEntity.Cast(GetParent());
		array<IEntity> playerEntities = parent.GetPlayerControlledEntitiesInsideTrigger();
		if (playerEntities.Count() == 0)
			return false;
		
		PlayerManager playerManager = GetGame().GetPlayerManager();
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		SCR_GroupsManagerComponent groupManager = SCR_GroupsManagerComponent.GetInstance();
			
		Faction thisPlayerFaction = factionManager.GetPlayerFaction(playerId);
		if (!thisPlayerFaction)
			return false;
		
		SCR_AIGroup thisPlayerGroup = groupManager.GetPlayerGroup(playerId);
		if (!thisPlayerGroup)
			return false;
		
		int friendlyCount = 0;
		int enemyCount = 0;
		
		foreach (IEntity player : playerEntities)
		{
			int occupantPlayerId = playerManager.GetPlayerIdFromControlledEntity(player);
			
			Faction occupantPlayerFaction = factionManager.GetPlayerFaction(occupantPlayerId);
			if (!occupantPlayerFaction)
				continue;
			
			// Handle case when the player in question's faction is friendly to itself (BLUFOR, OPFOR)
			if (thisPlayerFaction.IsFactionFriendly(thisPlayerFaction))
			{
				if (thisPlayerFaction != occupantPlayerFaction)
				{
					enemyCount += 1;
					continue;
				}
				
				friendlyCount += 1;
				continue;
			}
			// Handle case when the player in question's faction is a free-for-all faction (Independent)
			else
			{
				SCR_AIGroup occupantPlayerGroup = groupManager.GetPlayerGroup(playerId);
				if (!occupantPlayerGroup)
					continue;
				
				if (thisPlayerGroup != occupantPlayerGroup)
				{
					enemyCount += 1;
					continue;
				}
				
				friendlyCount += 1;
			}
		}
		
		if (friendlyCount + enemyCount == 0)
			return false;
		
		WR_GameModeWasteland gamemode = WR_GameModeWasteland.Cast(GetGame().GetGameMode());
		if (!gamemode)
			return false;
		
		float currentFriendlyToEnemyRatio = friendlyCount / (friendlyCount + enemyCount);
		
		return currentFriendlyToEnemyRatio >= gamemode.GetFriendlyPlayerTownSpawnRatio();
	}
}