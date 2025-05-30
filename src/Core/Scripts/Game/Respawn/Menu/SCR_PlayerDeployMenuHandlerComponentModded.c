modded class SCR_PlayerDeployMenuHandlerComponent
{
	private ref map<string, SCR_SpawnPoint> m_mSpawnBunkerSpawnPoints = new map<string, SCR_SpawnPoint>();
	private ref WR_Logger<SCR_PlayerDeployMenuHandlerComponent> logger = new WR_Logger<SCR_PlayerDeployMenuHandlerComponent>(this);
	
	override void Update(float timeSlice)
	{
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
					SAttemptSpawnInLobby();
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
	
	static bool SAttemptSpawnInLobby()
	{	
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController)
			return false;
		
		SCR_PlayerFactionAffiliationComponent playerFactionAffil = SCR_PlayerFactionAffiliationComponent.Cast(playerController.FindComponent(SCR_PlayerFactionAffiliationComponent));
		SCR_Faction playerFaction = SCR_Faction.Cast(playerFactionAffil.GetAffiliatedFaction()); // Compiler bug. This member variable usage should not be allowed.
		if (!playerFaction)
		{
			return false;
		}
		
		// Get spawn point for this player's faction
		string factionKey = playerFaction.GetFactionKey(); 
		string spawnPointName = string.Format("WR_SpawnPoint_Lobby_%1", factionKey);
		SCR_SpawnPoint spawnPoint = SCR_SpawnPoint.Cast(GetGame().GetWorld().FindEntityByName(spawnPointName));
		if (!spawnPoint)
		{
			Print(string.Format("Spawn point '%1' should be present in the world if a spawn lobby is also present!", spawnPointName), LogLevel.ERROR);
			return false;
		}
		
		// Respawn player
		SCR_RespawnComponent spawnRequestManager = SCR_RespawnComponent.Cast(playerController.GetRespawnComponent());

//		ResourceName charPrefab = playerFaction.GetDefaultCharacterPrefab();
//		vector pos = spawnPoint.GetOrigin();
//		vector heading = spawnPoint.GetYawPitchRoll();
//		SCR_FreeSpawnData respawnData = new SCR_FreeSpawnData(charPrefab, pos, heading);
//		respawnData.SetSkipPreload(false);

		ResourceName charPrefab = playerFaction.GetDefaultCharacterPrefab();
		int spawnPointRplId = spawnPoint.GetRplId();
		SCR_SpawnPointSpawnData spawnData = new SCR_SpawnPointSpawnData(charPrefab, spawnPointRplId);
		spawnData.SetSkipPreload(false);
		
		spawnRequestManager.RequestSpawn(spawnData);	
		return true;
	}
}
