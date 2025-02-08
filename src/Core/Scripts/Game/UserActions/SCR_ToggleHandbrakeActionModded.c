modded class SCR_ToggleHandbrakeAction : SCR_ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		Vehicle vehicleEntity = Vehicle.Cast(GetOwner().GetRootParent());
		if (!vehicleEntity)
			return false;

		if (!vehicleEntity.IsOccupied())
			return true;

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;

		if (vehicleEntity)
		{
			SCR_BaseCompartmentManagerComponent baseCompMan = SCR_BaseCompartmentManagerComponent.Cast(vehicleEntity.FindComponent(SCR_BaseCompartmentManagerComponent));
			if (!baseCompMan)
				return false;
			
			if (baseCompMan.GetOccupantCount() > 0)
			{
				Faction characterFaction = character.GetFaction();
			
				// Factions not friendly to themselves (Independent, in this case)
				if (characterFaction.IsFactionEnemy(characterFaction))
				{
					array<IEntity> occupants = {};
					baseCompMan.GetOccupants(occupants);
					
					PlayerManager playerManager = GetGame().GetPlayerManager();
					SCR_GroupsManagerComponent groupManager = SCR_GroupsManagerComponent.GetInstance();
					
					int characterPlayerId = playerManager.GetPlayerIdFromControlledEntity(character);
					SCR_AIGroup characterGroup = groupManager.GetPlayerGroup(characterPlayerId);
					
					// Allow action to be performed if any occupant of the vehicle is in the same squad as the character
					// Begin by assuming that the vehicle holds no squad members of the player
					bool noOccupantsAreMemberOfSquad = true;
					foreach (IEntity occupant : occupants)
					{	
						int occupantPlayerId = playerManager.GetPlayerIdFromControlledEntity(occupant);
						SCR_AIGroup occupantGroup = groupManager.GetPlayerGroup(occupantPlayerId);
						
						// If this occupant is a member of the player's squad, the original assumption is wrong and we can stop
						if (characterGroup == occupantGroup && characterGroup != null && occupantGroup != null)
						{
							noOccupantsAreMemberOfSquad = false;
							break;
						}
					}
					
					// Vehicle is hostile if no squad members were found inside.
					if (noOccupantsAreMemberOfSquad)
					{
						SetCannotPerformReason("#AR-UserAction_SeatHostile");
						return false;
					}
				}
				// Any other faction
				else
				{
					Faction vehicleFaction = vehicleEntity.GetFaction();
					if (characterFaction && vehicleFaction && characterFaction.IsFactionEnemy(vehicleFaction))
					{
						SetCannotPerformReason("#AR-UserAction_SeatHostile");
						return false;
					}
				}	
			}
		}

		if (!SCR_InteractionHandlerComponent.CanBeShownInVehicle(
			character,
			this,
			true,
			m_eShownInVehicleState != EUserActionInVehicleState.IN_VEHICLE_PILOT,
			m_eShownInVehicleState != EUserActionInVehicleState.NOT_IN_VEHICLE,
			m_aDefinedCompartmentSectionsOnly,
			m_aExcludeDefinedCompartmentSections
		))
		{
			SetCannotPerformReason(CONTROLLED_BY_DRIVER);
			return false;
		}

		return true;
	}
}