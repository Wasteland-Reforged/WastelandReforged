modded class SCR_DeployMenuMain
{
	override void MuteSounds(bool mute = true)
	{
		if (!IsOpen())
			return;

		WR_GameModeWasteland gameMode = WR_GameModeWasteland.Cast(GetGame().GetGameMode());
		if (!gameMode.IsSpawnLobbyPresent())
		{
			AudioSystem.SetMasterVolume(AudioSystem.SFX, !mute);
			AudioSystem.SetMasterVolume(AudioSystem.VoiceChat, !mute);
			AudioSystem.SetMasterVolume(AudioSystem.Dialog, !mute);
		}
		
		ArmaReforgerLoadingAnim.m_onExitLoadingScreen.Remove(MuteSounds);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpened()
	{
		WR_GameModeWasteland gameMode = WR_GameModeWasteland.Cast(GetGame().GetGameMode());
		if (gameMode.IsSpawnLobbyPresent())
		{
			SCR_UISoundEntity uiSound = SCR_UISoundEntity.GetInstance();
			uiSound.SoundEvent("WR_SOUND_MAP_DROP_3", force: true);
		}
		
		super.OnMenuOpened();
	}
	
	//------------------------------------------------------------------------------------------------
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
	
	//------------------------------------------------------------------------------------------------
	//! Centers map to a specific spawn point.
	protected void FocusOnPoint(vector point, bool smooth = true)
	{
		if (!m_MapEntity || !m_MapEntity.IsOpen())
			return;

		float x, y;
		m_MapEntity.WorldToScreen(point[0], point[2], x, y);

		float xScaled = GetGame().GetWorkspace().DPIUnscale(x);
		float yScaled = GetGame().GetWorkspace().DPIUnscale(y);

		if (smooth)
			m_MapEntity.PanSmooth(x, y);
		else
			m_MapEntity.SetPan(xScaled, yScaled, true, true);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void OnMapOpen(MapConfiguration config)
	{
		m_MapEntity.SetZoom(0.1);
		
		// note@lk: temporary hotfix for duplicite journal entries, better solution is on the way
		Widget toolMenu = m_wMenuFrame.FindAnyWidget("ToolMenuVert");
		Widget child = toolMenu.GetChildren();
		while (child)
		{
			Widget sibling = child.GetSibling();
			child.RemoveFromHierarchy();
			child = sibling;
		}		
		
		m_UIElementContainer = SCR_MapUIElementContainer.Cast(m_MapEntity.GetMapUIComponent(SCR_MapUIElementContainer));
		if (m_UIElementContainer)
			m_UIElementContainer.GetOnSpawnPointSelected().Insert(SetSpawnPointExt);

		GetGame().GetCallqueue().CallLater(SetInitialMapFocus);
	}
	
	protected void SetInitialMapFocus()
	{
		m_MapEntity.CenterMap();
	}
	
	override protected void RequestRespawn()
	{
		UpdateRespawnButton();
		
		if (!m_RespawnButton.IsEnabled())
			return;

		if (!m_iSelectedSpawnPointId.IsValid())
		{
			Debug.Error("Selected SpawnPointId is invalid!");
			return;
		}

		SCR_Faction playerFaction = SCR_Faction.Cast(m_PlyFactionAffilComp.GetAffiliatedFaction());
		if (!playerFaction)
			return;
		
		ResourceName characterPrefab = playerFaction.GetDefaultCharacterPrefab();

		m_fCurrentDeployTimeOut = DEPLOY_TIME_OUT;
		
		// Get spawn data
		SCR_SpawnData rspData;
		
		WR_SpawnPoint wrSpawnPoint = WR_SpawnPoint.Cast(SCR_SpawnPoint.GetSpawnPointByRplId(m_iSelectedSpawnPointId));
		if (wrSpawnPoint)
			rspData = WR_SpawnAreaPlayerSpawnHandlerComponent.GetSpawnData(characterPrefab, wrSpawnPoint.GetSpawnRegion());
		else
			rspData = new SCR_SpawnPointSpawnData(characterPrefab, m_iSelectedSpawnPointId);
		
		rspData.SetSkipPreload(false);
		m_SpawnRequestManager.RequestSpawn(rspData);
	}
	
	//! Opens pause menu.
	override protected void OnPauseMenu()
	{
		WR_GameModeWasteland gameMode = WR_GameModeWasteland.Cast(GetGame().GetGameMode());
		if (gameMode.IsSpawnLobbyPresent())
		{
			SCR_DeployMenuMain.CloseDeployMenu();
			return;
		}
		
		MenuBase menu = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.PauseMenu, 0, true, false);

		PauseMenuUI pauseMenu = PauseMenuUI.Cast(menu);
		if (pauseMenu)
		{
			pauseMenu.FadeBackground(true, true);
			pauseMenu.DisableSettings();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		TextWidget pauseBtnText = TextWidget.Cast(GetRootWidget().FindWidget("Overlay0.MenuFrame.Menu.Overlay0.Navigation.PauseButton.HorizontalLayout1.TextHint"));
		if (!pauseBtnText)
			return;
		
		WR_GameModeWasteland gameMode = WR_GameModeWasteland.Cast(GetGame().GetGameMode());
		if (gameMode.IsSpawnLobbyPresent())
		{
			pauseBtnText.SetText("Close Deployment Menu");
		}
	}
}

