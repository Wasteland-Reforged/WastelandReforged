modded class SCR_MapJournalUI : SCR_MapUIBaseComponent
{
	//------------------------------------------------------------------------------------------------
	override protected void OnPlayerFactionResponse(SCR_PlayerFactionAffiliationComponent component, int factionIndex, bool response)
	{
		if (response)
		{
			// Add null check to prevent exeception when leaving faction
			if (!m_wEntryLayout) return;
			
			m_wEntryLayout.SetVisible(false);
			GetJournalForPlayer();
		}
	}
}