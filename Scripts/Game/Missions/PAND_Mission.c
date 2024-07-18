class PAND_Mission
{
	protected bool m_bIsNull;
	protected int m_iMissionId = -1; // -1 indicates this object is a dummy value
	protected PAND_MissionType m_eType;
	protected PAND_MissionStatus m_eStatus;
	protected vector m_vPosition;
	protected WorldTimestamp m_CreatedAtTime;
	protected bool m_bIsSuccessful;

	//void PAND_Mission(int id, PAND_MissionType type, vector position)
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
	
	static PAND_Mission GetInitialMission()
	{
		return CreateMission(-1, PAND_MissionType.CaptureWeapons, "69 69 69");
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
			case PAND_MissionType.PaulTest:
				return "Paul's Test Mission";
			default:
			{
				Print("[WASTELAND] PAND_Mission: Invalid mission type!", LogLevel.ERROR);
				return "";	
			}
		}
		return "";
	}
	
	static bool Extract(PAND_Mission instance, ScriptCtx ctx, SSnapSerializerBase snapshot)
    {
//		// If instance is null, encode null flag and exit
//		bool instanceHasValue = instance != null;
//		snapshot.SerializeBool(instanceHasValue);
//		if (!instanceHasValue) return true;
//			
//		// Convert instance to snapshot
//        snapshot.SerializeInt(instance.m_iMissionId);
//        snapshot.SerializeInt(instance.m_eType);
//        snapshot.SerializeInt(instance.m_eStatus);
//        snapshot.SerializeVector(instance.m_vPosition);
//        //snapshot.SerializeBytes(instance.m_CreatedAtTime, 8); // Make sure it's correct to use SerializeBytes. The reason I initially used it is because WorldTimestamp has a 64-bit int behind it.
//        snapshot.SerializeBool(instance.m_bIsSuccessful);
		
//		bool instanceHasValue = instance != null;
//		snapshot.SerializeBool(instanceHasValue);
//		
//		if (instance)
//		{
	        snapshot.SerializeInt(instance.m_iMissionId);
	        snapshot.SerializeInt(instance.m_eType);
	        snapshot.SerializeInt(instance.m_eStatus);
	        snapshot.SerializeVector(instance.m_vPosition);
	        snapshot.SerializeBytes(instance.m_CreatedAtTime, 8); // Make sure it's correct to use SerializeBytes. The reason I initially used it is because WorldTimestamp has a 64-bit int behind it.
	        snapshot.SerializeBool(instance.m_bIsSuccessful);
//		}
//        
		return true;
    }
 
    static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, PAND_Mission instance)
    {
//		// If snapshot is null, decode null flag and exit
//		bool instanceHasValue;
//		snapshot.SerializeBool(instanceHasValue);
//		if (!instanceHasValue) return true;
		
		// Convert snapshot to instance
        snapshot.SerializeInt(instance.m_iMissionId);
        snapshot.SerializeInt(instance.m_eType);
        snapshot.SerializeInt(instance.m_eStatus);
        snapshot.SerializeVector(instance.m_vPosition);
        snapshot.SerializeBytes(instance.m_CreatedAtTime, 8); // Make sure it's correct to use SerializeBytes. The reason I initially used it is because WorldTimestamp has a 64-bit int behind it.
        snapshot.SerializeBool(instance.m_bIsSuccessful);
        
		return true;
    }
 
    static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet)
    {
		// Read values from snapshot, encode them into smaller representation, then
        // write them into packet.
        snapshot.EncodeInt(packet);
        snapshot.EncodeInt(packet);
        snapshot.EncodeInt(packet);
        snapshot.EncodeVector(packet);
        snapshot.Serialize(packet, 8); // Make sure it's correct to use Serialize. The reason I initially used it is because WorldTimestamp has a 64-bit int behind it.
        snapshot.EncodeBool(packet);
    }
 
    static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot)
    {
		//return true;
		
        // Read values from packet, decode them into their original representation,
        // then write them into snapshot.
        snapshot.DecodeInt(packet);
        snapshot.DecodeInt(packet);
        snapshot.DecodeInt(packet);
        snapshot.DecodeVector(packet);
        snapshot.Serialize(packet, 8); // Make sure it's correct to use Serialize. The reason I initially used it is because WorldTimestamp has a 64-bit int behind it.
        snapshot.DecodeBool(packet);
        
		return true;
    }
 
    static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx)
    {	
		// Compare two snapshots and determine whether they are the same.
        // We have to compare properties one-by-one, but for properties with known
        // length (such as primitive types bool, int, float and vector), we do multiple
        // comparisons in single call. However, because we do not know length of string,
        // we use provided function which will determine number of bytes that need
        // to be compared from serialized data.
        
		//return lhs.CompareSnapshots(rhs, 4+4+4+12+8+4);   // int32, int32, int32, vector, int64, bool
		return lhs.CompareSnapshots(rhs, 4+4+4+12+4);   // int32, int32, int32, vector, bool
    }
 
    static bool PropCompare(PAND_Mission instance, SSnapSerializerBase snapshot, ScriptCtx ctx)
    {
        if (!instance) return true; // Kill replication if instance is null
		
        // return snapshot.CompareInt(instance.m_iMissionId)
        //     && snapshot.CompareInt(instance.m_eType)
        //     && snapshot.CompareInt(instance.m_eStatus)
		//     && snapshot.CompareVector(instance.m_vPosition)
        //     //&& snapshot.Compare(instance.m_CreatedAt, 8)
		// 	&& snapshot.CompareBool(instance.m_bIsSuccessful);

        bool b1 = snapshot.CompareInt(instance.m_iMissionId);
        bool b2 = snapshot.CompareInt(instance.m_eType);
        bool b3 = snapshot.CompareInt(instance.m_eStatus);
		bool b4 = snapshot.CompareVector(instance.m_vPosition);
        bool b5 = snapshot.Compare(instance.m_CreatedAtTime, 8);
		bool b6 = snapshot.CompareBool(instance.m_bIsSuccessful);

		//return b1 && b2 && b3 && b4 && b5 && b6;
		return b1 && b2 && b3 && b4 && b6;
    }
}

//class PAND_MissionDto
//{
//	int m_iId;
//	PAND_MissionType m_eType;
//	PAND_MissionStatus m_eStatus;
//	vector m_vPosition;
//	WorldTimestamp m_CreatedAt;
//	bool b_IsSuccessful;
//	

//}