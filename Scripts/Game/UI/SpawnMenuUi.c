class WR_SpawnMenuUiClass : ChimeraMenuBase
{
	protected static const string TEXT_TITLE = "TextTitle";
	protected static const string BUTTON_CLOSE = "ButtonClose";
	protected static const string BUTTON_RESPAWN_NORTH = "ButtonRespawnNorth";
	protected static const string BUTTON_RESPAWN_CENTRAL = "ButtonRespawnCentral";
	protected static const string BUTTON_RESPAWN_SOUTH = "ButtonRespawnSouth";

	//------------------------------------------------------------------------------------------------
	protected override void OnMenuOpen()
	{
		Print("OnMenuOpen: menu/dialog opened!", LogLevel.NORMAL);

		Widget rootWidget = GetRootWidget();
		if (!rootWidget)
		{
			Print("Error in Layout Tutorial layout creation", LogLevel.ERROR);
			return;
		}

		//Close Menu
		SCR_ButtonTextComponent buttonClose = SCR_ButtonTextComponent.GetButtonText(BUTTON_CLOSE, rootWidget);
		if (buttonClose)
			buttonClose.m_OnClicked.Insert(Close);
		else
			Print("Button Close not found - won't be able to exit by button", LogLevel.WARNING);
		
		//RespawnNorth button
		SCR_ButtonTextComponent buttonRespawnNorth = SCR_ButtonTextComponent.GetButtonText(BUTTON_RESPAWN_NORTH, rootWidget);
		if (buttonRespawnNorth)
			buttonRespawnNorth.m_OnClicked.Insert(RespawnPlayerNorth);
		else
			Print("Button RespawnNorth not found", LogLevel.WARNING); // the button can be missing without putting the layout in jeopardy
		
		//RespawnCentral button
		SCR_ButtonTextComponent buttonRespawnCentral = SCR_ButtonTextComponent.GetButtonText(BUTTON_RESPAWN_CENTRAL, rootWidget);
		if (buttonRespawnCentral)
			buttonRespawnCentral.m_OnClicked.Insert(RespawnPlayerCentral);
		else
			Print("Button RespawnCentral not found", LogLevel.WARNING); // the button can be missing without putting the layout in jeopardy
		
		//RespawnSouth button
		SCR_ButtonTextComponent buttonRespawnSouth = SCR_ButtonTextComponent.GetButtonText(BUTTON_RESPAWN_SOUTH, rootWidget);
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
		Print("[Wasteland]: Entered RespawnPlayerNorth()");
		
		vector respawnPos;
		//bool respawnSuccessful = WR_SpawnAreaPlayerSpawnHandlerComponent.TryRespawnPlayer(respawnPos);
	}
	
	protected void RespawnPlayerCentral()
	{
		Print("[Wasteland]: Entered RespawnPlayerCentral()");
		
		vector respawnPos;
		//bool respawnSuccessful = WR_SpawnAreaPlayerSpawnHandlerComponent.TryRespawnPlayer(respawnPos);
	}
	
	protected void RespawnPlayerSouth()
	{
		Print("[Wasteland]: Entered RespawnPlayerSouth()");
		
		vector respawnPos;
		//bool respawnSuccessful = WR_SpawnAreaPlayerSpawnHandlerComponent.TryRespawnPlayer(respawnPos);
	}
	
}

modded enum ChimeraMenuPreset
{
	WR_SpawnMenuUi_Id
}

