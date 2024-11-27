class WR_MissionNotification
{
	WR_MissionType m_eMissionType;
	WR_MissionStatus m_eMissionStatus;
	WR_MissionCompletionType m_eCompletionType;
    int m_iCompletingPlayerId = -1;
	
	static WR_MissionNotification CreateMissionNotification(
		WR_MissionType missionType,
		WR_MissionStatus status,
		WR_MissionCompletionType completionType,
		int completingPlayerId = -1
	)
	{
		WR_MissionNotification notif = new WR_MissionNotification();
		
		notif.m_eMissionType = missionType;
		notif.m_eMissionStatus = status;
		notif.m_eCompletionType = completionType;
		notif.m_iCompletingPlayerId = completingPlayerId;
		
		return notif;
	}
	
	WR_MissionType GetMissionType()
	{
		return m_eMissionType;
	}
	
	WR_MissionStatus GetMissionStatus()
	{
		return m_eMissionStatus;
	}
	
	WR_MissionStatus GetMissionCompletionType()
	{
		return m_eCompletionType;
	}
	
	int GetCompletingPlayerId()
	{
		return m_iCompletingPlayerId;
	}
	
	///////////////////////
	/* Replication codec */
	///////////////////////
	
	static bool Extract(WR_MissionNotification instance, ScriptCtx ctx, SSnapSerializerBase snapshot)
    {
		// Convert instance to snapshot
		snapshot.SerializeInt(instance.m_eMissionType);
		snapshot.SerializeInt(instance.m_eMissionStatus);
		snapshot.SerializeInt(instance.m_eCompletionType);
		snapshot.SerializeInt(instance.m_iCompletingPlayerId);

		return true;
    }
 
    static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, WR_MissionNotification instance)
    {
		// Convert snapshot to instance
		snapshot.SerializeInt(instance.m_eMissionType);
		snapshot.SerializeInt(instance.m_eMissionStatus);
		snapshot.SerializeInt(instance.m_eCompletionType);
		snapshot.SerializeInt(instance.m_iCompletingPlayerId);
        
		return true;
    }
 
    static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet)
    {
		// Compress snapshot into packet
		snapshot.EncodeInt(packet);
        snapshot.EncodeInt(packet);
        snapshot.EncodeInt(packet);
        snapshot.EncodeInt(packet);
    }
 
    static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot)
    {
		// Decompress packet into snapshot
		snapshot.DecodeInt(packet);
        snapshot.DecodeInt(packet);
        snapshot.DecodeInt(packet);
        snapshot.DecodeInt(packet);
        
		return true;
    }
 
    static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx)
    {	
		// Compare two snapshots
		return lhs.CompareSnapshots(rhs, 4+4+4+4);   // int32, int32, int32, int32
    }
 
    static bool PropCompare(WR_MissionNotification instance, SSnapSerializerBase snapshot, ScriptCtx ctx)
    {
		// Compare instance and snapshot
        bool result =
			snapshot.CompareInt(instance.m_eMissionType)
			&& snapshot.CompareInt(instance.m_eMissionStatus)
			&& snapshot.CompareInt(instance.m_eCompletionType)
			&& snapshot.CompareInt(instance.m_iCompletingPlayerId);
		
		return result;
    }
}