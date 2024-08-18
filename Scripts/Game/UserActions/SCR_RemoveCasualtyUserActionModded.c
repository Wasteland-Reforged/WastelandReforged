modded class SCR_RemoveCasualtyUserAction : SCR_CompartmentUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment)
			return false;

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;

		CompartmentAccessComponent compartmentAccess = character.GetCompartmentAccessComponent();
		if (!compartmentAccess)
			return false;

		// Restrict removing casualty from another compartment in case the section does not match
		BaseCompartmentSlot characterCompartment = compartmentAccess.GetCompartment();
		if (characterCompartment && characterCompartment.GetCompartmentSection() != compartment.GetCompartmentSection())
			return false;

		IEntity owner = compartment.GetOwner();

		Vehicle vehicle = Vehicle.Cast(owner.GetRootParent());
		if (vehicle)
		{
			SCR_BaseCompartmentManagerComponent baseCompMan = SCR_BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
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
					Faction vehicleFaction = vehicle.GetFaction();
					if (characterFaction && vehicleFaction && characterFaction.IsFactionEnemy(vehicleFaction))
					{
						SetCannotPerformReason("#AR-UserAction_SeatHostile");
						return false;
					}
				}	
			}
		}

		BaseCompartmentManagerComponent managerComponent = compartment.GetManager();
		if (!managerComponent)
			return false;

		// Make sure vehicle can be enter via provided door, if not, set reason.
		if (!character.IsInVehicle() && !compartmentAccess.CanGetInVehicleViaDoor(owner, managerComponent, GetRelevantDoorIndex(user)))
		{
			SetCannotPerformReason(m_sCannotPerformObstructed);
			return false;
		}

		return true;
	}
}