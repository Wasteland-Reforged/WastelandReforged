modded class SCR_HintSettings
{
	// TODO: Verify that this actually works and is necessary beyond the Workbench environment
    override void SaveShownHints(BaseContainer container)
	{
		if (!container)
			return;
		
		container.Set("m_aShownHints", m_aShownHints);
		container.Set("m_aShownHintCounts", m_aShownHintCounts);
		GetGame().UserSettingsChanged();
	}
}