modded class SCR_PlayerDeployMenuHandlerComponent
{
	private ref map<string, SCR_SpawnPoint> m_mSpawnBunkerSpawnPoints = new map<string, SCR_SpawnPoint>();
	private ref WR_Logger<SCR_PlayerDeployMenuHandlerComponent> logger = new WR_Logger<SCR_PlayerDeployMenuHandlerComponent>(this);

	override void Update(float timeSlice)
	{
#ifdef ENABLE_DIAG
		if (SCR_RespawnComponent.Diag_IsCLISpawnEnabled())
			return;
#endif
		if (CanOpenWelcomeScreen())
		{
			if (!IsWelcomeScreenOpen())
				GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.WelcomeScreenMenu);

			return;

		}
		else if (IsWelcomeScreenOpen())
		{
			CloseWelcomeScreen();
			return;
		}

		if (CanOpenMenu())
		{
			if (HasPlayableFaction() && !SCR_RoleSelectionMenu.GetRoleSelectionMenu())
			{
				WR_GameModeWasteland gameMode = WR_GameModeWasteland.Cast(GetGame().GetGameMode());
				if (gameMode.IsSpawnLobbyPresent())
				{
					SCR_RoleSelectionMenu.CloseRoleSelectionMenu();
					AttemptSpawnInLobby();
				}
				else if (!SCR_DeployMenuMain.GetDeployMenu())
				{
					SCR_RoleSelectionMenu.CloseRoleSelectionMenu();
					SCR_DeployMenuMain.OpenDeployMenu();
				}
			}
			else
			{
				if (!SCR_RoleSelectionMenu.GetRoleSelectionMenu())
				{
					SCR_DeployMenuMain.CloseDeployMenu();
					SCR_RoleSelectionMenu.OpenRoleSelectionMenu();
				}
				
				m_bFirstOpen = false;
			}
		}
		else if (IsMenuOpen())
		{
			CloseMenu();
		}
	}
	
	private bool AttemptSpawnInLobby()
	{
		// Get spawn point for this player's faction
		string factionKey = m_PlyFactionAffil.GetAffiliatedFaction().GetFactionKey();
		SCR_SpawnPoint spawnPoint = m_mSpawnBunkerSpawnPoints.Get(factionKey);
		
		// Cache the spawn point if it is not already
		if (!spawnPoint)
		{
			string spawnPointName = string.Format("WR_SpawnPoint_Lobby_%1", factionKey);
			spawnPoint = SCR_SpawnPoint.Cast(GetGame().GetWorld().FindEntityByName(spawnPointName));
			if (!spawnPoint)
			{
				logger.LogError(string.Format("Spawn point '%1' should be present in the world if a spawn lobby is also present!", spawnPointName));
				return false;
			}
			m_mSpawnBunkerSpawnPoints.Set(factionKey, spawnPoint);
		}
		
		// Respawn player
		PlayerController pc = GetGame().GetPlayerController();
		SCR_RespawnComponent m_SpawnRequestManager = SCR_RespawnComponent.Cast(pc.GetRespawnComponent());

		// TODO: parameterize this. And make generic so we dont have two places in the spawning code where we're grabbing char prefabs
		string charPrefab;
		switch (factionKey)
		{
			case "US":
				charPrefab = "{B39F86B378284BFC}Prefabs/Characters/Factions/BLUFOR/US_Army/WR_Character_US_Base.et";
				break;
			case "USSR":
				charPrefab = "{8449DECFA1B5831F}Prefabs/Characters/Factions/OPFOR/USSR_Army/WR_Character_USSR_Base.et";
				break;
			case "FIA":
				charPrefab = "{CEE7531F4FBAEB38}Prefabs/Characters/Factions/CIV/Dockworker/WR_Character_CIV_Base.et";
				break;
			default:
				Print("[WASTELAND] WR_SpawnAreaPlayerSpawnHandlerComponent: Unknown faction key!", LogLevel.ERROR);
				return null;
		}
		
		vector pos = spawnPoint.GetOrigin();
		vector heading = spawnPoint.GetYawPitchRoll();
		
		SCR_FreeSpawnData respawnData = new SCR_FreeSpawnData(charPrefab, pos, heading);
		respawnData.SetSkipPreload(false);
		m_SpawnRequestManager.RequestSpawn(respawnData);	
		
		return true;
	}
}
