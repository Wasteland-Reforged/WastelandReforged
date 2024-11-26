class WR_SpawnMenuUiClass : ChimeraMenuBase
{
	protected static const string TEXT_TITLE = "TextTitle";
	protected static const string BUTTON_CLOSE = "ButtonClose";
	protected static const string BUTTON_RESPAWN_NORTH = "ButtonRespawnNorth";
	protected static const string BUTTON_RESPAWN_CENTRAL = "ButtonRespawnCentral";
	protected static const string BUTTON_RESPAWN_SOUTH = "ButtonRespawnSouth";
	
	SCR_ButtonTextComponent buttonClose;
	SCR_ButtonTextComponent buttonRespawnNorth;
	SCR_ButtonTextComponent buttonRespawnCentral;
	SCR_ButtonTextComponent buttonRespawnSouth;
	PlayerController pc;
	IEntity oldChar;

	//------------------------------------------------------------------------------------------------
	protected override void OnMenuOpen()
	{
		Print("OnMenuOpen: menu/dialog opened!", LogLevel.NORMAL);

		Widget rootWidget = GetRootWidget();
		if (!rootWidget) return;

		//Close Menu
		buttonClose = SCR_ButtonTextComponent.GetButtonText(BUTTON_CLOSE, rootWidget);
		if (buttonClose)
			buttonClose.m_OnClicked.Insert(Close);
		else
			Print("Button Close not found - won't be able to exit by button", LogLevel.WARNING);
		
		//RespawnNorth button
		buttonRespawnNorth = SCR_ButtonTextComponent.GetButtonText(BUTTON_RESPAWN_NORTH, rootWidget);
		if (buttonRespawnNorth)
			buttonRespawnNorth.m_OnClicked.Insert(RespawnPlayerNorth);
		else
			Print("Button RespawnNorth not found", LogLevel.WARNING); // the button can be missing without putting the layout in jeopardy
		
		//RespawnCentral button
		buttonRespawnCentral = SCR_ButtonTextComponent.GetButtonText(BUTTON_RESPAWN_CENTRAL, rootWidget);
		if (buttonRespawnCentral)
			buttonRespawnCentral.m_OnClicked.Insert(RespawnPlayerCentral);
		else
			Print("Button RespawnCentral not found", LogLevel.WARNING); // the button can be missing without putting the layout in jeopardy
		
		//RespawnSouth button
		buttonRespawnSouth = SCR_ButtonTextComponent.GetButtonText(BUTTON_RESPAWN_SOUTH, rootWidget);
		if (buttonRespawnSouth)
			buttonRespawnSouth.m_OnClicked.Insert(RespawnPlayerSouth);
		else
			Print("Button RespawnSouth not found", LogLevel.WARNING); // the button can be missing without putting the layout in jeopardy


		//ESC/Start listener
		InputManager inputManager = GetGame().GetInputManager();
		if (inputManager)
		{
			// this is for the menu/dialog to close when pressing ESC
			// an alternative is to have a button with the SCR_NavigationButtonComponent component
			// and its Action Name field set to MenuBack - this would activate the button on ESC press
			inputManager.AddActionListener("MenuOpen", EActionTrigger.DOWN, Close);
			inputManager.AddActionListener("MenuBack", EActionTrigger.DOWN, Close);
#ifdef WORKBENCH // in Workbench, F10 is used because ESC closes the preview
			inputManager.AddActionListener("MenuOpenWB", EActionTrigger.DOWN, Close);
			inputManager.AddActionListener("MenuBackWB", EActionTrigger.DOWN, Close);
#endif
		}
		else if (!buttonClose)
		{
			Print("Auto-closing the menu that has no exit path", LogLevel.WARNING);
			Close();
			return;
		}
	}

	//------------------------------------------------------------------------------------------------
	protected override void OnMenuClose()
	{
		// here we clean action listeners added above as the good practice wants it
		InputManager inputManager = GetGame().GetInputManager();
		if (inputManager)
		{
			inputManager.RemoveActionListener("MenuOpen", EActionTrigger.DOWN, Close);
			inputManager.RemoveActionListener("MenuBack", EActionTrigger.DOWN, Close);
#ifdef WORKBENCH
			inputManager.RemoveActionListener("MenuOpenWB", EActionTrigger.DOWN, Close);
			inputManager.RemoveActionListener("MenuBackWB", EActionTrigger.DOWN, Close);
#endif
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void RespawnPlayerNorth()
	{
		RequestCustomRespawn(SpawnAreaCategory.SPAWN_NORTH);
	}
	
	protected void RespawnPlayerCentral()
	{
		RequestCustomRespawn(SpawnAreaCategory.SPAWN_CENTRAL);
	}
	
	protected void RespawnPlayerSouth()
	{
		RequestCustomRespawn(SpawnAreaCategory.SPAWN_SOUTH);
	}
	
	protected void RequestCustomRespawn(SpawnAreaCategory spawnCategory)
	{
		Widget rootWidget = GetRootWidget();
		if (!rootWidget) return;
		
		//Change Title Text
		TextWidget textTitle = TextWidget.Cast(rootWidget.FindWidget(TEXT_TITLE));
		textTitle.SetText("Waiting for respawn...");
		
		//Disable buttons while waiting for respawn
		buttonRespawnNorth.m_OnClicked.Clear();
		buttonRespawnCentral.m_OnClicked.Clear();
		buttonRespawnSouth.m_OnClicked.Clear();
		buttonClose.m_OnClicked.Clear();
		
		//Respawn Player
		pc = GetGame().GetPlayerController();
		SCR_RespawnComponent m_SpawnRequestManager = SCR_RespawnComponent.Cast(pc.GetRespawnComponent());
		
		SCR_PlayerFactionAffiliationComponent m_PlyFactionAffilComp = SCR_PlayerFactionAffiliationComponent.Cast(pc.FindComponent(SCR_PlayerFactionAffiliationComponent));
		string factionKey = m_PlyFactionAffilComp.GetAffiliatedFaction().GetFactionKey();
		
		m_SpawnRequestManager.SGetOnLocalPlayerSpawned().Insert(Close);
		//oldChar = pc.GetControlledEntity();
		//m_SpawnRequestManager.SGetOnLocalPlayerSpawned().Insert(DeleteOldChar);

		SCR_FreeSpawnData rspData = WR_SpawnAreaPlayerSpawnHandlerComponent.GetSpawnData(factionKey, spawnCategory);
		if (rspData)
			m_SpawnRequestManager.RequestSpawn(rspData);	
		
	}
	
	protected void DeleteOldChar() {
		delete oldChar;
	}
	
}

modded enum ChimeraMenuPreset
{
	WR_SpawnMenuUi_Id
}

