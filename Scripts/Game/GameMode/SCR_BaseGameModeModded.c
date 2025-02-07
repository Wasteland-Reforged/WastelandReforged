//------------------------------------------------------------------------------------------------
modded class SCR_BaseGameMode
{
	//------------------------------------------------------------------------------------------------
	/*!
		Modded to not delete player character upon disconnect if character is dead
	*/
	protected override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		// Set player controlled entity to null if it is dead or uncon to avoid the body being deleted upon disconnect
		if (IsMaster())
			NullifyPlayerControlledEntityIfDeadOrUncon(playerId);
		
		super.OnPlayerDisconnected(playerId, cause, timeout);
	}
	
	private void NullifyPlayerControlledEntityIfDeadOrUncon(int playerId)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!playerController)
			return;
		
		IEntity controlledEntity = playerController.GetControlledEntity();
		if (!controlledEntity)
			return;
		
		CharacterControllerComponent charController = CharacterControllerComponent.Cast(controlledEntity.FindComponent(CharacterControllerComponent));
		if (!charController)
			return;
		
		if (
			charController.GetLifeState() == ECharacterLifeState.DEAD 
			|| charController.GetLifeState() == ECharacterLifeState.INCAPACITATED
		)
		{
			playerController.SetControlledEntity(null);
			
			// TODO: Anti-combat logging measures can be taken here once persistence is implemented.
			// Instead of only setting the controlled entity to null if the player is dead/uncon,
			// we can instead check to see if the player was alive when he disconnected.
			// Then, simply do a CallLater to delete the body.
		}
	}
}
