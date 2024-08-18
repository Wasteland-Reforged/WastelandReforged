


//------------------------------------------------------------------------------------------------
modded class SCR_GetInUserAction : SCR_CompartmentUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!pOwnerEntity || !pUserEntity)
			return;
		
		ChimeraCharacter character = ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;
		
		BaseCompartmentSlot targetCompartment = GetCompartmentSlot();
		if (!targetCompartment)
			return;
		
		CompartmentAccessComponent compartmentAccess = character.GetCompartmentAccessComponent();
		if (!compartmentAccess)
			return;
		Vehicle vehicle = Vehicle.Cast(SCR_EntityHelper.GetMainParent(pOwnerEntity, true));
		FactionKey origFaction; 
		if (!vehicle)
			return;
			
		SCR_VehicleFactionAffiliationComponent affiliation = vehicle.GetFactionAffiliation();
		origFaction = affiliation.GetAffiliatedFactionKey();
		affiliation.SetAffiliatedFactionByKey("friendly");
		
		if (!compartmentAccess.GetInVehicle(pOwnerEntity, targetCompartment, false, GetRelevantDoorIndex(pUserEntity), ECloseDoorAfterActions.RETURN_TO_PREVIOUS_STATE, false))
			return;
		
		affiliation.SetAffiliatedFactionByKey(origFaction);
		
		super.PerformAction(pOwnerEntity, pUserEntity);
	}
	
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
		
//		Vehicle vehicle = Vehicle.Cast(SCR_EntityHelper.GetMainParent(owner, true));
//		if (vehicle)
//		{
//			Faction characterFaction = character.GetFaction();
//			Faction vehicleFaction = vehicle.GetFaction();
//			if (characterFaction && vehicleFaction && characterFaction.IsFactionEnemy(vehicleFaction))
//			{
//				SetCannotPerformReason("#AR-UserAction_SeatHostile");
//				return false;
//			}
//		}
		
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