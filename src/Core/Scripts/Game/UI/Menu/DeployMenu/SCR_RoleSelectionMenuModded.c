modded class SCR_RoleSelectionMenu
{
	override static SCR_RoleSelectionMenu OpenRoleSelectionMenu()
	{
		if (!GetRoleSelectionMenu())
			GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RoleSelectionDialog);
		
		WR_GameModeWasteland gameMode = WR_GameModeWasteland.Cast(GetGame().GetGameMode());
		if (!gameMode.IsSpawnLobbyPresent())
		{
			SMuteSounds(true);
		}
		
		return GetRoleSelectionMenu();
	}
	
	override static void CloseRoleSelectionMenu()
	{
		GetGame().GetMenuManager().CloseMenuByPreset(ChimeraMenuPreset.RoleSelectionDialog);

		WR_GameModeWasteland gameMode = WR_GameModeWasteland.Cast(GetGame().GetGameMode());
		if (!gameMode.IsSpawnLobbyPresent())
		{
			SMuteSounds(false);
		}
	}
	
	private static void SMuteSounds(bool mute = true)
	{
		AudioSystem.SetMasterVolume(AudioSystem.SFX, !mute);
		AudioSystem.SetMasterVolume(AudioSystem.VoiceChat, !mute);
		AudioSystem.SetMasterVolume(AudioSystem.Dialog, !mute);
	}
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		WR_GameModeWasteland gameMode = WR_GameModeWasteland.Cast(GetGame().GetGameMode());
		if (gameMode.IsSpawnLobbyPresent())
		{
			SetUpContinueButton();
		}
	}
	
	private void SetUpContinueButton()
	{		
		m_ContinueButton.m_OnActivated.Remove(Close);
		m_ContinueButton.m_OnActivated.Insert(SetContinueButtonDisabled);
		m_ContinueButton.m_OnActivated.Insert(SCR_PlayerDeployMenuHandlerComponent.SAttemptSpawnInLobby);
	}
	
	private void SetContinueButtonDisabled()
	{
		m_ContinueButton.SetEnabled(false);
	}
}
