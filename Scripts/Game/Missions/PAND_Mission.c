class PAND_Mission
{
	// Replicated properties
	protected int m_iMissionId;
	protected PAND_MissionType m_eType = PAND_MissionType.Empty;
	protected PAND_MissionStatus m_eStatus;
	protected vector m_vPosition;
	protected WorldTimestamp m_CreatedAtTime;
	protected bool m_bIsSuccessful;
	
	// Other properties
	protected SCR_MapMarkerBase m_Marker;

	static PAND_Mission CreateMission(int id, PAND_MissionType type, vector position)
	{
		PAND_Mission mission = new PAND_Mission();
		
		mission.m_iMissionId = id;
		mission.m_eType = type;
		mission.m_vPosition = position;
		mission.m_CreatedAtTime = WR_Utils.TimestampNow(); // Is this the best way of tracking when a mission was started? Will this be prone to bugs if server/client are out of sync regarding world time?
		mission.m_eStatus = PAND_MissionStatus.InProgress;
		mission.m_bIsSuccessful = false;

		// Should we also trigger mission setup (spawning NPCs and loot) here? Not sure yet.
		
		return mission;
	}
	
	int GetMissionId()
	{
		return m_iMissionId;
	}
	
	PAND_MissionType GetType()
	{
		return m_eType;
	}
	
	PAND_MissionStatus GetStatus()
	{
		return m_eStatus;
	}
	
	vector GetPosition()
	{
		return m_vPosition;
	}
	
	WorldTimestamp GetCreatedAtTime()
	{
		return m_CreatedAtTime;
	}
	
	bool GetIsSuccessful()
	{
		return m_bIsSuccessful;
	}
	
	string GetName()
	{
		// TODO: make this reference a config file or something
		switch (m_eType)
		{
			case PAND_MissionType.CaptureWeapons:
				return "Capture Weapons";
			case PAND_MissionType.CaptureVehicle:
				return "Capture Vehicle";
			default:
			{
				Print("[WASTELAND] PAND_Mission: Invalid mission type!", LogLevel.ERROR);
				return "";	
			}
		}
		return "";
	}
	
	bool IsEmptyMission()
	{
		return m_eType == PAND_MissionType.Empty;
	}
	
	string GetDescription()
	{
		// TODO: read these from a config somewhere
		switch (m_eType)
		{
			case PAND_MissionType.CaptureWeapons:
				return "A weapon cache has been spotted and is available for capture!";
			case PAND_MissionType.CaptureVehicle:
				return "A vehicle has been disabled and is available for capture!";
			default:
			{
				Print("[WASTELAND] PAND_Mission: Invalid mission type!", LogLevel.ERROR);
				return "";	
			}
		}
		return "";
	}
	
	SCR_MapMarkerBase GetMarker()
	{
		return m_Marker;
	}
	
	void SetMarker(SCR_MapMarkerBase marker)
	{
		m_Marker = marker;
	}
	
	/////////////////////////////
	/* Replication codec below */
	/////////////////////////////
	
	static bool Extract(PAND_Mission instance, ScriptCtx ctx, SSnapSerializerBase snapshot)
    {
		// Convert instance to snapshot
		snapshot.SerializeInt(instance.m_iMissionId);
		snapshot.SerializeInt(instance.m_eType);
		snapshot.SerializeInt(instance.m_eStatus);
		snapshot.SerializeVector(instance.m_vPosition);
		snapshot.SerializeBytes(instance.m_CreatedAtTime, 8);
		snapshot.SerializeBool(instance.m_bIsSuccessful);

		return true;
    }
 
    static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, PAND_Mission instance)
    {
		// Convert snapshot to instance
        snapshot.SerializeInt(instance.m_iMissionId);
        snapshot.SerializeInt(instance.m_eType);
        snapshot.SerializeInt(instance.m_eStatus);
        snapshot.SerializeVector(instance.m_vPosition);
        snapshot.SerializeBytes(instance.m_CreatedAtTime, 8);
        snapshot.SerializeBool(instance.m_bIsSuccessful);
        
		return true;
    }
 
    static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet)
    {
		// Compress snapshot into packet
        snapshot.EncodeInt(packet);
        snapshot.EncodeInt(packet);
        snapshot.EncodeInt(packet);
        snapshot.EncodeVector(packet);
        snapshot.Serialize(packet, 8);
        snapshot.EncodeBool(packet);
    }
 
    static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot)
    {
		// Decompress packet into snapshot
        snapshot.DecodeInt(packet);
        snapshot.DecodeInt(packet);
        snapshot.DecodeInt(packet);
        snapshot.DecodeVector(packet);
        snapshot.Serialize(packet, 8);
        snapshot.DecodeBool(packet);
        
		return true;
    }
 
    static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx)
    {	
		// Compare two snapshots
		return lhs.CompareSnapshots(rhs, 4+4+4+12+4);   // int32, int32, int32, vector, bool
    }
 
    static bool PropCompare(PAND_Mission instance, SSnapSerializerBase snapshot, ScriptCtx ctx)
    {
		// Compare instance and snapshot
        bool result =
			snapshot.CompareInt(instance.m_iMissionId)
			&& snapshot.CompareInt(instance.m_eType)
			&& snapshot.CompareInt(instance.m_eStatus)
			&& snapshot.CompareVector(instance.m_vPosition)
			&& snapshot.Compare(instance.m_CreatedAtTime, 8)
			&& snapshot.CompareBool(instance.m_bIsSuccessful);
		
		return result;
    }
}