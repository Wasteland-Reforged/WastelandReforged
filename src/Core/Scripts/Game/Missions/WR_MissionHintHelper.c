class WR_MissionHintHelper
{
	static void ShowHintWithSound(string title, string description, string sound)
	{
		int durationS = WR_Utils.MillisecondsToSeconds(WR_Constants.s_iMissionHintDisplayDurationMs);
		
		SCR_HintUIInfo hintInfo = SCR_HintUIInfo.CreateInfo(
			description,
			title,
			durationS,
			EHint.UNDEFINED,
			EFieldManualEntryId.NONE,
			true
		);
		
		SCR_HintManagerComponent.ShowHint(hintInfo, isSilent: true, ignoreShown: true);
		PlayNotificationSound(sound);
	}
	
	private static void PlayNotificationSound(string sound)
	{
		SCR_UISoundEntity uiSound = SCR_UISoundEntity.GetInstance();
		uiSound.SoundEvent(sound, force: true);
	}
}