//------------------------------------------------------------------------------------------------
modded class SCR_CloseVehicleDoorUserAction : VehicleDoorUserAction
{	
	//------------------------------------------------------------------------------------------------
	// Override to allow players to close doors on vehicles occupied by enemies
	override bool CanBePerformedScript(IEntity user)
	{
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(GetOwner().FindComponent(SCR_VehicleDamageManagerComponent));
		if (damageManager && damageManager.GetState() == EDamageState.DESTROYED)
			return false;
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;

		CompartmentAccessComponent compartmentAccess = character.GetCompartmentAccessComponent();
		if (!compartmentAccess)
			return false;
		
		BaseCompartmentManagerComponent managerComponent = BaseCompartmentManagerComponent.Cast(GetOwner().FindComponent(BaseCompartmentManagerComponent));
		if (!managerComponent)
			return false;
		
		Vehicle vehicle = Vehicle.Cast(SCR_EntityHelper.GetMainParent(GetOwner(), true));
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
		
		if (managerComponent.GetDoorUser(GetDoorIndex()) && managerComponent.GetDoorUser(GetDoorIndex()) != user || !managerComponent.AreDoorOpen(GetDoorIndex()))
		{
			SetCannotPerformReason("#AR-UserAction_SeatOccupied");
			return false;
		}
		
		if (!compartmentAccess.CanAccessDoor(vehicle, managerComponent, GetDoorIndex()))
		{
			SetCannotPerformReason("#AR-UserAction_SeatObstructed");
			return false;
		}
		
		return true;
	}	
}