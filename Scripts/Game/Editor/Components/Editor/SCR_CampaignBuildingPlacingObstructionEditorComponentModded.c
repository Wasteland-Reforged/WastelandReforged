modded class SCR_CampaignBuildingPlacingObstructionEditorComponentClass : SCR_BaseEditorComponentClass
{

}

modded class SCR_CampaignBuildingPlacingObstructionEditorComponent : SCR_BaseEditorComponent
{
	override bool CanCreate(out ENotification outNotification = -1, out SCR_EPreviewState previewStateToShow = SCR_EPreviewState.PLACEABLE)
	{
		
		// superior can be created was set to false which means some external condition block placing the composition. No need to continue with any evaluation.
		if (m_bSuperiorCanBeCreated)
		{
			outNotification = ENotification.EDITOR_PLACING_BLOCKED;
			previewStateToShow = SCR_EPreviewState.BLOCKED;
			return false;
		}
			
		m_bCanBeCreated = true;
		m_fRollMin = 0;
		m_fRollMax = 0;
		m_fPitchMin = 0;
		m_fPitchMax = 0;

		if (m_bIsVehicle)
		{
			if (!CheckVehiclePosition())
			{
				m_bCanBeCreated = false;
				outNotification = ENotification.EDITOR_PLACING_BLOCKED;
				previewStateToShow = SCR_EPreviewState.BLOCKED;
			}
		
			return m_bCanBeCreated;
		}

		foreach (Tuple3<IEntity, float, vector> compositionEntity : m_aCompositionEntities)
		{
			if (compositionEntity.param1 && !CheckEntityPosition(compositionEntity.param1.GetOrigin(), compositionEntity.param2))
			{
				previewStateToShow = SCR_EPreviewState.WARNING;
				break;
			}

			if (compositionEntity.param2 < MINIMAL_ENTITY_RADIUS_TO_EVALUATE_TILT)
				continue;

			if (IsCompositionTilted(compositionEntity.param1, compositionEntity.param2, compositionEntity.param3, previewStateToShow))
				break;
		}

		if (Math.AbsFloat(m_fRollMax - m_fRollMin) > m_fTiltDifferenceWarning)
			previewStateToShow = SCR_EPreviewState.WARNING;

		if (Math.AbsFloat(m_fPitchMax - m_fPitchMin) > m_fTiltDifferenceWarning)
			previewStateToShow = SCR_EPreviewState.WARNING;

		return m_bCanBeCreated;
	}
}