[BaseContainerProps()]
modded class SCR_NameTagRulesetFriendlies
{
	//------------------------------------------------------------------------------------------------
	//! Determine whether the tag passes basic conditions for visibility
	//! \param data is the subject nametag
	//! \param timeSlice is frame time slice
	//! \return Returns true if tag passed conditions for visibility

	override protected bool TestVisibility(SCR_NameTagData data, float timeSlice)
	{
		// current players tag, update tag specific vars and return
		if (data == m_CurrentPlayerTag)
		{
			data.UpdateTagPos();
			return false;
		}

		// out of zone range
		data.m_fTimeSliceUpdate += timeSlice;
		
		if (data.m_fTimeSliceUpdate >= POSITION_UPDATE) // avoid updating every frame
		{
			data.m_fTimeSliceUpdate = 0;
			
			data.m_fDistance = vector.DistanceSq(m_CurrentPlayerTag.m_vEntWorldPos, data.m_Entity.GetOrigin()); // checked through origin here to avoid more expensive pos check bottom
		}
		
		int distMax = m_ZoneCfg.m_fFarthestZoneRangePow2;
		Print("[SCR_NameTagRulesetBase.TestVisibility] debug line (" + __FILE__ + " L" + __LINE__ + ")", LogLevel.WARNING);		if (data.m_fDistance >= distMax) // distance of visible tag is updated per frame for scaling, which is why this check has its own scope
		{
			if (data.m_fDistance >= distMax + 100)		// cleanup tags which are more than distance + 10m (10^2) away from base zone query 
				data.m_fTimeSliceCleanup += timeSlice;			
			
			return false;
		}
		else
			data.m_fTimeSliceCleanup = 0;
		
		// vehicle visibility 
		if (data.m_Flags & ENameTagFlags.VEHICLE)	// in vehicle
		{
			if (!data.m_VehicleParent.m_bIsControlledPresent)	// dont show vehicle crew tags until inside the same vehicle
				return false;
			else if (m_CurrentPlayerTag.m_CharController.IsInThirdPersonView())	// dont show vehicle crew tags if in third person
				return false;
		}
		
		if (data.m_eType == ENameTagEntityType.VEHICLE)	// is vehicle
		{	
			if (data.m_Flags & ENameTagFlags.VEHICLE_DISABLE || SCR_VehicleTagData.Cast(data).m_bIsControlledPresent)
				return false;
		}
								
		// update tag position
		if (data.m_eAttachedTo == data.m_eAttachedToLast)
			data.UpdateTagPos();
		else
		{
			data.m_fTimeSlicePosChange += timeSlice * POS_TRANSITION_MULT;
			if ( data.m_fTimeSlicePosChange >= 1.0 )
			{
				data.UpdateTagPos();
				data.m_eAttachedToLast = data.m_eAttachedTo;
			}
			else 
			{
				data.UpdateTagPos();
				data.m_vTagWorldPos = vector.Lerp( data.m_vTagWorldPosLast, data.m_vTagWorldPos, data.m_fTimeSlicePosChange );
			}
		}
					
		CalculateScreenPos(data);
		if (m_iRefResolutionY * 1.1 < data.m_vTagScreenPos[1] || m_iRefResolutionX * 1.1 < data.m_vTagScreenPos[0] || 0 > data.m_vTagScreenPos[1] || 0 > data.m_vTagScreenPos[0])
			return false;	
			
		return true;	
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check if target is visible to the player
	//! \param data is the subject nametag
	//! \return Returns true if the tested target is visible/not obstructed in current players line of sight
	override protected bool TraceLOS(SCR_NameTagData data)
	{						
		TraceParam param = new TraceParam;
		param.Start = m_CurrentPlayerTag.m_vEntHeadPos;
		param.End = data.m_vEntHeadPos + HEAD_LOS_OFFSET;
		param.LayerMask = EPhysicsLayerDefs.Camera;
		param.Flags = TraceFlags.WORLD; // Only check for world obstruction
		param.Exclude = m_CurrentPlayerTag.m_Entity;
			
		float percent = GetGame().GetWorld().TraceMove(param, null);
		if (percent == 1 || param.TraceEnt == data.m_Entity)
			return true; // If trace travels the entire path, return true
		
		return false;
	}
}