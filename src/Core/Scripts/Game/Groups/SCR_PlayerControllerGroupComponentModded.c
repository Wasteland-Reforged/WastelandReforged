modded class SCR_PlayerControllerGroupComponent : ScriptComponent
{
	override bool CanPlayerJoinGroup(int playerID, notnull SCR_AIGroup group)
	{
		// Groups manager doesn't exist, no point in continuing, cannot join
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		if (!groupsManager)
		{	
			#ifdef DEPLOY_MENU_DEBUG
			Print(string.Format("SCR_PlayerControllerGroupComponent.CanPlayerJoinGroup(%1, %2) - No SCR_GroupsManagerComponent!", playerID, group), LogLevel.ERROR);
			#endif
			return false;
		}

		// Cannot join a full group
		if (group.IsFull())
			return false;

		// Cannot join the group we are in already
		if (groupsManager.GetPlayerGroup(playerID) == group)
		{
			#ifdef DEPLOY_MENU_DEBUG
			Print(string.Format("SCR_PlayerControllerGroupComponent.CanPlayerJoinGroup(%1, %2) - Already in group!", playerID, group), LogLevel.ERROR);
			#endif
			return false;
		}	

		return true;
	}
}
