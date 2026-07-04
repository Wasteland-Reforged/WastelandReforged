modded class SCR_MapJournalUI : SCR_MapUIBaseComponent
{
	//------------------------------------------------------------------------------------------------
	override protected void OnPlayerFactionResponse(SCR_PlayerFactionAffiliationComponent component, int factionIndex, bool response)
	{
		if (response)
		{
			SetJournalVisibility(false);
			GetJournalForPlayer();
		}
	}
}