modded class SCR_CampaignBuildingEditorComponentClass : SCR_BaseEditorComponentClass
{
}

modded class SCR_CampaignBuildingEditorComponent : SCR_BaseEditorComponent
{
	override SCR_FactionAffiliationComponent GetProviderFactionComponent()
	{
		return null;
	}
}
