class WR_MissionSlot
{
	WR_Mission m_Mission;
	
	WorldTimestamp m_CreatedTime;
	WorldTimestamp m_StartTime;
	WorldTimestamp m_CompletedTime;
	
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

	WorldTimestamp GetCreatedTime()
	{
		return m_CreatedTime;
	}
	
	void SetCreatedTime(WorldTimestamp time)
	{
		m_CreatedTime = time;
	}
	
	WorldTimestamp GetStartTime()
	{
		return m_StartTime;
	}
	
	void SetCompletedTime(WorldTimestamp time)
	{
		m_CompletedTime = time;
	}

	WorldTimestamp GetCompletedTime()
	{
		return m_CompletedTime;
	}
	
	void SetCompletedTime(WorldTimestamp time)
	{
		m_CompletedTime = time;
	}
}