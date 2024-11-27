class WR_MissionHintHelper
{
	// TODO: make this configurable from world editor
	private static string m_sSoundEventNameNotification = "SOUND_HUD_TASK_CREATED";
	
	static void ShowHintWithSound(string title, string description)
	{
		SCR_HintUIInfo hintInfo = SCR_HintUIInfo.CreateInfo(
			description,
			title,
			10, // TODO: read from config
			EHint.UNDEFINED,
			EFieldManualEntryId.NONE,
			true // TODO: read from config
		);
		
		SCR_HintManagerComponent.ShowHint(hintInfo, isSilent: true, ignoreShown: true);
		PlayNotificationSound();
	}
	
	private static void PlayNotificationSound()
	{
		SCR_UISoundEntity uiSound = SCR_UISoundEntity.GetInstance();
		//uiSound.SetSignalValueStr("", 1.00); // TODO: figure out how to set volume properly
		uiSound.SoundEvent(m_sSoundEventNameNotification, force: true);
	}
}