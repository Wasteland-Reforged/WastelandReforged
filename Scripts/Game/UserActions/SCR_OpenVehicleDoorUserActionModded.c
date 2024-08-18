//------------------------------------------------------------------------------------------------
modded class SCR_OpenVehicleDoorUserAction : VehicleDoorUserAction
{	
	//------------------------------------------------------------------------------------------------
	// Override to allow players to open doors on vehicles occupied by enemies
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
		
		if (managerComponent.GetDoorUser(GetDoorIndex()) && managerComponent.GetDoorUser(GetDoorIndex()) != user || managerComponent.AreDoorOpen(GetDoorIndex()))
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