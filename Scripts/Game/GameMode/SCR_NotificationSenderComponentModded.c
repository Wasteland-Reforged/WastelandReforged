//[ComponentEditorProps(category: "GameScripted/GameMode/Components", description: "Base for gamemode scripted component.")]
//class SCR_NotificationSenderComponentModdedClass : SCR_BaseGameModeComponentClass
//{
//}

modded class SCR_NotificationSenderComponent
{		
	//------------------------------------------------------------------------------------------------
	override void OnPlayerSpawned(int playerId, IEntity controlledEntity)
	{
		// Disable "<player-name> joined the <faction-name>" notifications for game masters.
		
//		//Check if faction changed and send notification if diffrent
//		if (m_FactionManager) 
//		{
//			//Get factions using ID
//			Faction playerFaction = m_FactionManager.GetPlayerFaction(playerId);
//		
//			//Faction is diffrent
//			if (m_FactionOnSpawn != playerFaction)
//			{
//				//Send player joined faction notification
//				SCR_NotificationsComponent.SendToUnlimitedEditorPlayers(ENotification.PLAYER_JOINED_FACTION, playerId);
//				m_FactionOnSpawn = playerFaction;
//			}
//		}
	}	
}
