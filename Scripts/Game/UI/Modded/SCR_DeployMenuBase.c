modded class SCR_DeployMenuMain
{
	//------------------------------------------------------------------------------------------------
	//! Opens deploy menu.
	static override SCR_DeployMenuMain OpenDeployMenu()
	{
		GetGame().GetMenuManager().CloseAllMenus();
		if (!GetDeployMenu())
			GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.WR_RespawnMenu); // RespawnSuperMenu // WR_RespawnMenu
		
		return GetDeployMenu();
	}

	//------------------------------------------------------------------------------------------------
	//! As the name suggests, this method closes the deploy menu instance.
	static override void CloseDeployMenu()
	{
		GetGame().GetMenuManager().CloseMenuByPreset(ChimeraMenuPreset.WR_RespawnMenu);
	}

	//------------------------------------------------------------------------------------------------
	//! Returns the deploy menu instance.
	static override SCR_DeployMenuMain GetDeployMenu()
	{
		return SCR_DeployMenuMain.Cast(GetGame().GetMenuManager().FindMenuByPreset(ChimeraMenuPreset.WR_RespawnMenu));
	}
	
	//! Sends a respawn request based on assigned loadout and selected spawn point.
	protected override void RequestRespawn()
	{
		UpdateRespawnButton();
		
		if (!m_RespawnButton.IsEnabled())
			return;

//		if (!m_iSelectedSpawnPointId.IsValid())
//		{
//			Debug.Error("Selected SpawnPointId is invalid!");
//			return;
//		}

//		ResourceName resourcePrefab = ResourceName.Empty;
//		if (m_LoadoutRequestUIHandler.GetPlayerLoadout())
//			resourcePrefab = m_LoadoutRequestUIHandler.GetPlayerLoadout().GetLoadoutResource();
//		else
//		{
//			Debug.Error("No player loadout assigned!");
//			return;
//		}

		m_fCurrentDeployTimeOut = DEPLOY_TIME_OUT;
		
		//SCR_SpawnPointSpawnData rspData = new SCR_SpawnPointSpawnData(resourcePrefab, m_iSelectedSpawnPointId);
		SCR_FreeSpawnData rspData = WR_SpawnAreaPlayerSpawnHandlerComponent.GetSpawnData();
		if (rspData)
			m_SpawnRequestManager.RequestSpawn(rspData);
	}
	
	//! Sets respawn button enabled based on certain conditions.
	protected override void UpdateRespawnButton()
	{
		int remainingTime = -1;
		
		if (m_ActiveRespawnTimer)
			remainingTime = m_ActiveRespawnTimer.GetPlayerRemainingTime(m_iPlayerId);
		
		bool hasGroup = true;
		if (m_GroupRequestUIHandler && m_GroupRequestUIHandler.IsEnabled())
			hasGroup = m_GroupRequestUIHandler.GetPlayerGroup();
		m_bCanRespawnAtSpawnPoint = true;
		m_RespawnButton.SetEnabled(!m_bRespawnRequested && remainingTime <= 0 && m_fCurrentDeployTimeOut <= 0 && m_bCanRespawnAtSpawnPoint && m_PlyLoadoutComp.GetLoadout() != null && hasGroup);
	}
}