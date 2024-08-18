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

//		Faction characterFaction = character.GetFaction();
//		Faction vehicleFaction = vehicleEntity.GetFaction();
//		if (characterFaction && vehicleFaction && characterFaction.IsFactionEnemy(vehicleFaction))
//		{
//			SetCannotPerformReason(OCCUPIED_BY_ENEMY);
//			return false;
//		}

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