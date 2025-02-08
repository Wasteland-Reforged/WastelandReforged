modded class SCR_CampaignBuildingCompositionComponent : ScriptComponent
{
	SCR_FactionManager m_FactionManager;
	
	//------------------------------------------------------------------------------------------------
	// Called when the composition is spawned (gradual building)
	override protected void SetIsCompositionSpawned()
	{
		m_bCompositionIsSpawned = true;
		Replication.BumpMe();

		if (m_iBuilderId != INVALID_PLAYER_ID)
			CompositionBuildSound();

		if (!IsProxy())
		{
			SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.Cast(GetOwner().FindComponent(SCR_EditableEntityComponent));
			if (editable)
				editable.SetEntityFlag(EEditableEntityFlag.SPAWN_UNFINISHED, false);
			
			SetDestroyEvents();
		}

		if (m_OnCompositionSpawned)
			m_OnCompositionSpawned.Invoke(true);
		
		m_FactionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		int playerId = GetBuilderId(); // Get ID of player who built this composition
		Faction builderFaction = m_FactionManager.GetPlayerFaction(playerId);
		if (!builderFaction)
			return;
	
		IEntity owner = GetOwner();
		if (owner)
			SetFactionRecursively(owner, builderFaction);
	}
	
	private void SetFactionRecursively(IEntity entity, Faction builderFaction)
	{
		if (!entity)
			return;
		
		if (!m_FactionManager)
			m_FactionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
	
		// Set faction for the current entity if it has a FactionAffiliationComponent
		FactionAffiliationComponent factionComponent = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
		if (factionComponent)
			factionComponent.SetAffiliatedFaction(builderFaction);
	
		// Recursively process all children
		IEntity child = entity.GetChildren(); // Get first child
		while (child)
		{
			SetFactionRecursively(child, builderFaction); // Recursive call for child
			child = child.GetSibling(); // Move to next sibling
		}
	}
}
