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