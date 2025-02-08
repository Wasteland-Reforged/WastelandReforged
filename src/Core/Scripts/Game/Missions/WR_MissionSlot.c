// TODO: get rid of this class. put m_LastStepCompletedTime in WR_Mission 
class WR_MissionSlot
{
	WR_Mission m_Mission;
//	WorldTimestamp m_CreatedAtTime;
//	WorldTimestamp m_CompletedAtTime;
	
	WorldTimestamp m_LastStepCompletedTime;
	
	void WR_MissionSlot(WR_Mission mission)
	{
		m_Mission = mission;
	}
	
	WR_Mission GetMission()
	{
		return m_Mission;
	}
	
	void SetMission(WR_Mission mission)
	{
		m_Mission = mission;
	}
	
	WorldTimestamp GetLastStepCompletedTime()
	{
		return m_LastStepCompletedTime;
	}
	
	void SetLastStepCompletedTime(WorldTimestamp time)
	{
		m_LastStepCompletedTime = time;
	}
	
	
//	WorldTimestamp GetCreatedAtTime()
//	{
//		return m_CreatedAtTime;
//	}
//	
//	void SetCreatedAtTime(WorldTimestamp time)
//	{
//		m_CreatedAtTime = time;
//	}
//	
//	WorldTimestamp GetCompletedAtTime()
//	{
//		return m_CompletedAtTime;
//	}
//	
//	void SetCompletedAtTime(WorldTimestamp time)
//	{
//		m_CompletedAtTime = time;
//	}
}