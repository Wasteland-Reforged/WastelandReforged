
modded class SCR_SpawnHandlerComponent : ScriptComponent
{
	
	override protected bool AssignEntity_S(SCR_SpawnRequestComponent requestComponent, IEntity entity, SCR_SpawnData data)
	{
		int playerId = requestComponent.GetPlayerController().GetPlayerId();
		#ifdef _ENABLE_RESPAWN_LOGS
		Print(string.Format("%1::AssignEntity_S(playerId: %2, entity: %3, data: %4)", Type().ToString(),
					playerId,
					entity,
					data), LogLevel.NORMAL);
		#endif

		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));

		// Already controlling!
		IEntity previous = playerController.GetControlledEntity();
		if (previous == entity)
			return false;

		playerController.SetInitialMainEntity(entity);
		
		delete previous;

		// Notify the system of change
		HandleEntityChange_S(requestComponent, previous, entity, data);
		
		return true;
	}
}
