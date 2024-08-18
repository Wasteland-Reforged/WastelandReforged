


//------------------------------------------------------------------------------------------------
modded class SCR_GetInUserAction : SCR_CompartmentUserAction
{
	//------------------------------------------------------------------------------------------------
	// Override to allow players to enter vehicles occupied by enemies
	override bool CanBePerformedScript(IEntity user)
	{
		if (m_DamageManager && m_DamageManager.GetState() == EDamageState.DESTROYED)
			return false;

		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment)
			return false;
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;

		CompartmentAccessComponent compartmentAccess = character.GetCompartmentAccessComponent();
		if (!compartmentAccess)
			return false;
		
		//~ TODO: Hotfix until proper solution, only blocks player does not block AI or Editor actions
		float storedResources;
		if (m_ResourceComp && m_CompartmentManager && m_CompartmentManager.BlockSuppliesIfOccupied())
		{
			if (SCR_ResourceSystemHelper.GetStoredResources(m_ResourceComp, storedResources) && storedResources > 0)
			{
				SetCannotPerformReason(OCCUPIED_BY_SUPPLIES);
				return false;
			}
		}
		
		IEntity owner = compartment.GetOwner();
		Vehicle vehicle = Vehicle.Cast(SCR_EntityHelper.GetMainParent(owner, true));
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
		
		if (compartment.GetOccupant())
		{
			SetCannotPerformReason("#AR-UserAction_SeatOccupied");
			return false;
		}
		
		// Check if the position isn't lock.
		if (m_pLockComp && m_pLockComp.IsLocked(user, compartment))
		{
			SetCannotPerformReason(m_pLockComp.GetCannotPerformReason(user));
			return false;
		}
		
		// Make sure vehicle can be enter via provided door, if not, set reason.
		if (!compartmentAccess.CanGetInVehicleViaDoor(owner, m_CompartmentManager, GetRelevantDoorIndex(user)))
		{
			SetCannotPerformReason("#AR-UserAction_SeatObstructed");
			return false;
		}
		
		return true;
	}	
}