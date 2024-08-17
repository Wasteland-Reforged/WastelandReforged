//------------------------------------------------------------------------------------------------
modded class SCR_MapMarkerManagerComponent : SCR_BaseGameModeComponent
{
	//------------------------------------------------------------------------------------------------
	//! Similar to vanilla SetStreamRulesForPlayer, but added exception handling when marker is null and more advanced logic to determine whether to stream a marker
	override void SetStreamRulesForPlayer(int playerID)
	{
		foreach (SCR_MapMarkerEntity dynamicMarker : m_aDynamicMarkers)
		{
			// FPM: Added exception handling
			if (!dynamicMarker)
				continue;
			
			FPM_MapMarkerPlayer fpmMarker = FPM_MapMarkerPlayer.Cast(dynamicMarker);
			if (fpmMarker != null)
			{
				// If SCR_MapMarkerEntity is successfully cast to FPM_MapMarkerPlayer, handle it as such.
				PlayerController playerController = GetGame().GetPlayerManager().GetPlayerController(playerID);
				bool shouldStreamOut = !ShouldRenderMapMarkerForPlayer(playerID, fpmMarker.GetPlayerId());
				
				HandleStreamOut(fpmMarker, playerController, shouldStreamOut);
			}
			else
			{
 				// Otherwise, handle the entity as a SCR_MapMarkerEntity as it is in vanilla.
				Faction markerFaction = dynamicMarker.GetFaction();
				
				if (!markerFaction || markerFaction == SCR_FactionManager.SGetPlayerFaction(playerID))
					HandleStreamOut(dynamicMarker, GetGame().GetPlayerManager().GetPlayerController(playerID), false);
				else
					HandleStreamOut(dynamicMarker, GetGame().GetPlayerManager().GetPlayerController(playerID), true);
			}
		};
	}
	
	//------------------------------------------------------------------------------------------------
	//! Similar to vanilla SetMarkerStreamRules, but with more advanced logic to determine whether to stream a marker
	override void SetMarkerStreamRules(notnull SCR_MapMarkerEntity marker)
	{
		array<int> players = {};
		GetGame().GetPlayerManager().GetPlayers(players);
		
		// Cast to FPM so we can get the player this marker is associated with
		FPM_MapMarkerPlayer fpmMarker = FPM_MapMarkerPlayer.Cast(marker);
		if (!fpmMarker)
			return;
		
		foreach (int playerId : players)
		{
			PlayerController playerController = GetGame().GetPlayerManager().GetPlayerController(playerId);
			bool shouldStreamOut = !ShouldRenderMapMarkerForPlayer(playerId, fpmMarker.GetPlayerId());
			
			HandleStreamOut(fpmMarker, playerController, shouldStreamOut);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	// Decides if a marker should be shown to a given player based on factors such as faction, group, and ownership of the marker.
	static bool ShouldRenderMapMarkerForPlayer(int playerId, int markerOwnerId)
	{
		Faction playerFaction = SCR_FactionManager.SGetPlayerFaction(playerId);
		Faction markerOwnerFaction = SCR_FactionManager.SGetPlayerFaction(markerOwnerId);
		
		if (!playerFaction || !markerOwnerFaction) return false;

		bool isPlayerMemberOfMarkerOwnerFaction = playerFaction == markerOwnerFaction;
		bool isPlayerFactionFriendlyToSelf = playerFaction.IsFactionFriendly(playerFaction);
		
		SCR_GroupsManagerComponent groupManager = SCR_GroupsManagerComponent.GetInstance();
		SCR_AIGroup playerGroup = groupManager.GetPlayerGroup(playerId)	;
		SCR_AIGroup markerOwnerGroup = groupManager.GetPlayerGroup(markerOwnerId);
		
		bool isPlayerMemberOfMarkerOwnerGroup = playerGroup == markerOwnerGroup && playerGroup != null && markerOwnerGroup != null;
		
		bool isPlayerOwnerOfMarker = playerId == markerOwnerId;
		
		bool shouldRenderMapMarker =
			isPlayerOwnerOfMarker 												 	  // Always show the player their own position 
			|| isPlayerMemberOfMarkerOwnerGroup  									  // On free-for-all factions, only show group members
			|| (isPlayerFactionFriendlyToSelf && isPlayerMemberOfMarkerOwnerFaction); // On team factions, always show teammates
		
		return shouldRenderMapMarker;
	}
}
