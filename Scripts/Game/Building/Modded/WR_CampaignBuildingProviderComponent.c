modded class SCR_CampaignBuildingProviderComponentClass : SCR_MilitaryBaseLogicComponentClass
{
}

modded class SCR_CampaignBuildingProviderComponent : SCR_MilitaryBaseLogicComponent
{
	override bool IsCharacterFactionSame(notnull IEntity character)
	{
		return true;
	}
}