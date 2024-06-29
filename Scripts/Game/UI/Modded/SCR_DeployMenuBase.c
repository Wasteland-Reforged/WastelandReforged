modded class SCR_DeployMenuMain
{
	//------------------------------------------------------------------------------------------------
	//! Opens deploy menu.
	static override SCR_DeployMenuMain OpenDeployMenu()
	{
		GetGame().GetMenuManager().CloseAllMenus();
		if (!GetDeployMenu())
			GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.WR_RespawnMenu);
		
		return GetDeployMenu();
	}
}