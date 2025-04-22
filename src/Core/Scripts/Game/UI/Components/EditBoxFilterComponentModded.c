modded class EditBoxFilterComponent : ScriptedWidgetComponent
{
	//------------------------------------------------------------------------------------------------
	override bool OnChange(Widget w, bool finished)
	{
		Print("Entered EditBoxFilterComponentModded.OnChange");
		
		return false;
		
		bool validInput = true;
		if (!m_wEditBox)
		{
			if (m_wMultilineEditBox)
				return OnChangeMultiline();
			
			return false;
		}
		
		string text = m_wEditBox.GetText();
		int length = text.Length();
		string shortText = text;
		
		if (length > 0)
		{
			if (length > m_iCharacterLimit) 
			{
				shortText = text.Substring(0, m_iCharacterLimit);
				m_wEditBox.SetText(shortText);
				
				m_OnTextTooLong.Invoke();
				validInput = false;
			}
			else
			{
				// Find invalid characters 
				shortText = FilterSymbolsFromText(text);
				
				// Invoke wrong input if texts are not same
				if (shortText != text)
				{
					m_wEditBox.SetText(shortText);
					m_OnInvalidInput.Invoke();
					validInput = false;
				}
			}
		}
		
		m_iCharacterCount = length;
		
		if (validInput)
			m_OnValidInput.Invoke();
		
		return false;
	}
};