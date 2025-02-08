class WR_MissionNotificationService
{
	[RplProp(onRplName: "OnNotificationReceived")]
	WR_MissionType m_MissionType;
	
	[RplProp(onRplName: "OnNotificationReceived")]
	vector m_vMissionPosition;
	
	[RplProp(onRplName: "OnNotificationReceived")]
	int m_iCompletingPlayerId;
	
	void OnNotificationReceived()
	{
		//WR_MissionMarkerHelper.SendMissionNotificationByStatus
	}
}