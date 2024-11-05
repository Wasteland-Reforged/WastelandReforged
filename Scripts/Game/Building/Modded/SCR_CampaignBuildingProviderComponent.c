modded class SCR_CampaignBuildingProviderComponentClass : SCR_MilitaryBaseLogicComponentClass
{
}

modded class SCR_CampaignBuildingProviderComponent : SCR_MilitaryBaseLogicComponent
{
	override SCR_CampaignBuildingBudgetToEvaluateData GetBudgetData(EEditableEntityBudget budget)
	{
		/*
		foreach (SCR_CampaignBuildingBudgetToEvaluateData budgetData : m_aBudgetsToEvaluate)
		{
			if (budgetData.GetBudget() == budget)
				return budgetData;
		}
		*/
		return null;
	}
	
	
	override bool IsCharacterFactionSame(notnull IEntity character)
	{
		return true;
	}
	
	override bool IsEnemyFaction(notnull ChimeraCharacter char)
	{
		FactionAffiliationComponent factionComponent = FactionAffiliationComponent.Cast(char.FindComponent(FactionAffiliationComponent));
		if (!factionComponent)
			return false;
		
		Faction faction = factionComponent.GetAffiliatedFaction();
		if (!faction)
			return false;
		
		SCR_CampaignFactionManager factionManager = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return false;
		
		Faction playerFaction = factionManager.GetLocalPlayerFaction();
		if (!playerFaction)
			return false;
		
		// Does the entity have a group? If so, are we in the same group?
		int localPlayerId = SCR_PlayerController.GetLocalPlayerId();
		int entityControllerPlayerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(char);
		
		if (entityControllerPlayerId != 0)
		{
			SCR_AIGroup localPlayerGroup = SCR_GroupsManagerComponent.GetInstance().GetPlayerGroup(localPlayerId);
			SCR_AIGroup entityGroup = SCR_GroupsManagerComponent.GetInstance().GetPlayerGroup(entityControllerPlayerId);
			
			bool membersOfSameGroup = localPlayerGroup == entityGroup;
			if (membersOfSameGroup)
				return false;
		}
		
		return playerFaction.IsFactionEnemy(faction);
	}
}