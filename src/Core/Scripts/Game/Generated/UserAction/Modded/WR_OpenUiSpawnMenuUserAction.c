class WR_OpenUiSpawnMenuUserAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		Print("WR_OpenUiSpawnMenuUserAction.PerformAction method reached", LogLevel.NORMAL);
		
		GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.WR_SpawnMenuUi_Id); 
		
		//SCR_HintManagerComponent.GetInstance().ShowCustomHint("Opened Spawn Menu layout", "Layout Opened", 0);
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
}