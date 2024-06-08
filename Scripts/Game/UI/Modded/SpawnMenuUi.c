class SPHCU_SpawnMenuUiClass : ChimeraMenuBase
{
	protected static const string TEXT_TITLE = "TextTitle";
	protected static const string BUTTON_CLOSE = "ButtonClose";
	protected static const string BUTTON_CHANGE = "ButtonChange";
	protected static const string BUTTON_RESPAWN = "ButtonRespawn";

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

		/*
			Close button
		*/

		SCR_ButtonTextComponent buttonClose = SCR_ButtonTextComponent.GetButtonText(BUTTON_CLOSE, rootWidget);
		if (buttonClose)
			buttonClose.m_OnClicked.Insert(Close);
		else
			Print("Button Close not found - won't be able to exit by button", LogLevel.WARNING);

		/*
			Change button
		*/

		SCR_ButtonTextComponent buttonChange = SCR_ButtonTextComponent.GetButtonText(BUTTON_CHANGE, rootWidget);
		if (buttonChange)
			buttonChange.m_OnClicked.Insert(ChangeText);
		else
			Print("Button Change not found", LogLevel.WARNING); // the button can be missing without putting the layout in jeopardy
		
		/*
			Respawn button
		*/

		SCR_ButtonTextComponent buttonRespawn = SCR_ButtonTextComponent.GetButtonText(BUTTON_RESPAWN, rootWidget);
		if (buttonRespawn)
			buttonRespawn.m_OnClicked.Insert(RespawnPlayer);
		else
			Print("Button Respawn not found", LogLevel.WARNING); // the button can be missing without putting the layout in jeopardy

		/*
			ESC/Start listener
		*/

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
	protected void ChangeText()
	{
		Widget rootWidget = GetRootWidget();
		if (!rootWidget)
			return;

		TextWidget textTitle = TextWidget.Cast(rootWidget.FindWidget(TEXT_TITLE));
		if (!textTitle)
		{
			Print("Title as TextWidget could not be found", LogLevel.WARNING);
			return;
		}

		string result;
		switch (Math.RandomInt(1, 6))
		{
			case 1: result = "This is a title"; break;
			case 2: result = "Random text"; break;
			case 3: result = "Third text, actually"; break;
			case 4: result = "Bonjour"; break;
			case 5: result = "I like trains"; break;
		}

		textTitle.SetText(result);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void RespawnPlayer()
	{
		vector respawnPos;
		bool respawnSuccessful = SPHCU_SpawnAreaPlayerSpawnHandlerComponent.TryRespawnPlayer(respawnPos);
		
		if (respawnSuccessful) Close();
//		PlayerController playerController = GetGame().GetPlayerController();
//		IEntity originalPlayerEntity = playerController.GetControlledEntity();
//		
//		SCR_RespawnComponent respawnComponent = SCR_RespawnComponent.Cast(playerController.GetRespawnComponent());
//		if (!respawnComponent) return;
//		
//		ResourceName prefab = "{84B40583F4D1B7A3}Prefabs/Characters/Factions/INDFOR/FIA/Character_FIA_Rifleman.et";
//		
//		// Select a random spawn area
//		// todo: check to make sure spawn areas is not null or empty
//		array<SPHCU_SpawnAreaEntity> spawnAreas = SPHCU_SpawnAreaEntity.SpawnAreas;
//		if (!spawnAreas)
//		{
//			Print("The spawn area list is null!");
//			return;
//		}
//		else if (spawnAreas.Count() < 1)
//		{
//			Print("The spawn area list is empty! Place one or more spawn areas in the world.");
//			return;
//		}
//		
//		SPHCU_SpawnAreaEntity spawnArea = SPHCU_SpawnAreaEntity.SpawnAreas.GetRandomElement();
//		
//		// Find a random safe position within the spawn area to spawn the player at
//		vector spawnAreaCenterPos = spawnArea.GetOrigin();
//		float spawnAreaRadius = spawnArea.GetSphereRadius();
//		
//		vector randomDir = Vector(Math.RandomFloat(0, 360), 0, 0).AnglesToVector();
//		float randomDist = Math.RandomFloat(0, spawnAreaRadius);
//		
//		float areaToCheck = 25;
//		vector outPos;
//		vector randomSpawnPos = spawnAreaCenterPos + randomDir * randomDist;
//		bool validPosFound = SCR_WorldTools.FindEmptyTerrainPosition(outPos, randomSpawnPos, areaToCheck);
//		
//		// Instantiate a FreeSpawnData object with the safe position we found
//		vector respawnPos;
//		
//		if (validPosFound)
//		{
//			respawnPos = outPos;
//		}
//		else
//		{
//			respawnPos = spawnAreaCenterPos;
//			// todo: probably want to fail here instead of setting the respawn pos to the center pos
//		}
//			
//		SCR_FreeSpawnData data = new SCR_FreeSpawnData(prefab, respawnPos);
//		
//		// Spawn the player
//		Print("Attempting to spawn player in " + spawnArea.GetSpawnAreaName());
//		bool isSpawnSuccessful = respawnComponent.RequestSpawn(data);
//		
//		if (isSpawnSuccessful)
//		{
//			if (originalPlayerEntity)
//				delete originalPlayerEntity; // Delete player's original body if this is not the player's initial spawn (i.e. a respawn)
//			
//			Close(); // Close menu
//			Print("Player successfully spawned in " + spawnArea.GetSpawnAreaName());
//		}
//			
//		// TODO: handle the case where no valid spawn points are found anywhere in the world
	}
}

modded enum ChimeraMenuPreset
{
	SPHCU_SpawnMenuUi_Id
}

