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
			buttonChange.m_OnClicked.Insert(AddItemToVehicle);
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
	}
	
	//------------------------------------------------------------------------------------------------
	protected void AddItemToVehicle()
	{
		string entityToFind = "PalletJack_Debug";
		//string entityToFind = "Humvee";
		IEntity storageEntity = GetGame().GetWorld().FindEntityByName(entityToFind);
		SCR_UniversalInventoryStorageComponent storage = SCR_UniversalInventoryStorageComponent.Cast(storageEntity.FindComponent(SCR_UniversalInventoryStorageComponent));
		
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(storageEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		//SCR_VehicleInventoryStorageManagerComponent inventoryManager = SCR_VehicleInventoryStorageManagerComponent.Cast(storageEntity.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
		
		string prefab = "{3E413771E1834D2F}Prefabs/Weapons/Rifles/M16/Rifle_M16A2.et";
		
		//Resource resource = Resource.Load(prefab);
		//ResourceName resourceName = new ResourceName(prefab);
		
		//IEntity item = GetGame().SpawnEntityPrefab(resource);
		//bool successfullyInsertedItem = inventoryManager.TryInsertItem(item);
		//bool successfullyInsertedItem = inventoryManager.TryInsertItemInStorage(item, storage);
		bool successfullyInsertedItem = inventoryManager.TrySpawnPrefabToStorage(prefab, storage);
		
		Print(successfullyInsertedItem);
	}
}

modded enum ChimeraMenuPreset
{
	SPHCU_SpawnMenuUi_Id
}

