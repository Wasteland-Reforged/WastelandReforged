class WR_Mission
{
	// Replicated properties
	protected int m_iMissionId;
	protected WR_MissionType m_eType = WR_MissionType.EMPTY;
	protected WR_MissionStatus m_eStatus;
	protected vector m_vPosition;
	
	// Other properties
	protected ref WR_MissionDefinition m_Definition; // Contains info about mission reward, NPCs, props, etc.
	protected ref SCR_MapMarkerBase m_Marker; // Map marker indicating location and status
	protected WR_MissionLocationEntity m_MissionLocation;
	
	// Entities - the entities for which this mission is responsible (props, AI, etc.) are tracked for clean-up purposes
	protected IEntity m_aRewards;
	protected IEntity m_Prop;
	protected ref array<SCR_AIGroup> m_aAiGroups = {};

	static WR_Mission CreateMission(int id, WR_MissionType type)
	{
		WR_Mission mission = new WR_Mission();
		
		mission.m_iMissionId = id;
		mission.m_eType = type;
		mission.m_eStatus = WR_MissionStatus.InProgress;
		
		return mission;
	}
	
	int GetMissionId()
	{
		return m_iMissionId;
	}
	
	void SetMissionId(int missionId)
	{
		m_iMissionId = missionId;
	}
	
	WR_MissionType GetType()
	{
		return m_eType;
	}
	
	void SetType(WR_MissionType type)
	{
		m_eType = type;
	}
	
	WR_MissionStatus GetStatus()
	{
		return m_eStatus;
	}
	
	void SetStatus(WR_MissionStatus status)
	{
		m_eStatus = status;
	}
	
	vector GetPosition()
	{
		return m_vPosition;
	}
	
	void SetPosition(vector position)
	{
		m_vPosition = position;
	}
	
	string GetName()
	{
		if (!m_Definition)
		{
			Print("[WASTELAND] WR_Mission: Cannot retrieve mission name! Mission definition is missing", LogLevel.ERROR);
			return "";
		}
		
		return m_Definition.m_sName;
	}
	
	bool IsEmptyMission()
	{
		return m_eType == WR_MissionType.EMPTY;
	}
	
	string GetDescription()
	{
		if (!m_Definition)
		{
			Print("[WASTELAND] WR_Mission: Cannot retrieve mission description! Mission definition is missing", LogLevel.ERROR);
			return "";
		}
		
		return m_Definition.m_sDescription;
	}
	
	WR_MissionDefinition GetDefinition()
	{
		return m_Definition;	
	}
	
	void SetDefinition(WR_MissionDefinition definition)
	{
		m_Definition = definition;
	}
	
	SCR_MapMarkerBase GetMarker()
	{
		return m_Marker;
	}
	
	void SetMarker(SCR_MapMarkerBase marker)
	{
		m_Marker = marker;
	}
	
	array<IEntity> GetMissionEntities()
	{
		array<IEntity> entities = {};
		
		entities.Insert(m_aRewards);
		entities.Insert(m_Prop);
		
		// Have to insert one by one because array does not auto upcast when using InsertAll
		foreach (SCR_AIGroup group : m_aAiGroups)
			entities.Insert(group);
		
		return entities;
	}
	
	void SetMissionEntities(IEntity rewards, IEntity prop, array<SCR_AIGroup> aiGroups)
	{
		m_aRewards = rewards;
		m_Prop = prop;
		m_aAiGroups = aiGroups;
	}
	
	ref array<SCR_AIGroup> GetAiGroups()
	{
		return m_aAiGroups;
	}
	
	WR_MissionLocationEntity GetMissionLocation()
	{
		return m_MissionLocation;
	}
	
	void SetMissionLocation(WR_MissionLocationEntity location)
	{
		m_MissionLocation = location;
		m_vPosition = location.GetOrigin();
	}
	
	/////////////////////////////
	/* Replication codec below */
	/////////////////////////////
	
	static bool Extract(WR_Mission instance, ScriptCtx ctx, SSnapSerializerBase snapshot)
    {
		// Convert instance to snapshot
		snapshot.SerializeInt(instance.m_iMissionId);
		snapshot.SerializeInt(instance.m_eType);
		snapshot.SerializeInt(instance.m_eStatus);
		snapshot.SerializeVector(instance.m_vPosition);

		return true;
    }
 
    static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, WR_Mission instance)
    {
		// Convert snapshot to instance
        snapshot.SerializeInt(instance.m_iMissionId);
        snapshot.SerializeInt(instance.m_eType);
        snapshot.SerializeInt(instance.m_eStatus);
        snapshot.SerializeVector(instance.m_vPosition);
        
		return true;
    }
 
    static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet)
    {
		// Compress snapshot into packet
        snapshot.EncodeInt(packet);
        snapshot.EncodeInt(packet);
        snapshot.EncodeInt(packet);
        snapshot.EncodeVector(packet);
    }
 
    static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot)
    {
		// Decompress packet into snapshot
        snapshot.DecodeInt(packet);
        snapshot.DecodeInt(packet);
        snapshot.DecodeInt(packet);
        snapshot.DecodeVector(packet);
        
		return true;
    }
 
    static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx)
    {	
		// Compare two snapshots
		return lhs.CompareSnapshots(rhs, 4+4+4+12);   // int32, int32, int32, vector
    }
 
    static bool PropCompare(WR_Mission instance, SSnapSerializerBase snapshot, ScriptCtx ctx)
    {
		// Compare instance and snapshot
        bool result =
			snapshot.CompareInt(instance.m_iMissionId)
			&& snapshot.CompareInt(instance.m_eType)
			&& snapshot.CompareInt(instance.m_eStatus)
			&& snapshot.CompareVector(instance.m_vPosition);
		
		return result;
    }
}