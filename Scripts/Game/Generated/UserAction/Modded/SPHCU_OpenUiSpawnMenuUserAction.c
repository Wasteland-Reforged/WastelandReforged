class SPHCU_OpenUiSpawnMenuUserAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		Print("SPHCU_OpenUiSpawnMenuUserAction.PerformAction method reached", LogLevel.NORMAL);
		
		GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.SPHCU_SpawnMenuUi_Id); 
		
		SCR_HintManagerComponent.GetInstance().ShowCustomHint("Opened Spawn Menu layout", "Layout Opened", 0);
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