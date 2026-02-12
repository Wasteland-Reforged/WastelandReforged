[EntityEditorProps(category: "WastelandReforged", description: "Marks a spherical area players can spawn inside.")]
class WR_SpawnAreaEntityClass : BaseGameTriggerEntityClass
{
	
}

class WR_SpawnAreaEntity : BaseGameTriggerEntity
{
	// Categories
	private const string CATEGORY_SPAWNAREA = "Spawn Area";
	
	ref WR_Logger<WR_SpawnAreaEntity> logger = new WR_Logger<WR_SpawnAreaEntity>(this);
	
	[Attribute(defvalue: "", desc: "Name of the spawn area", category: CATEGORY_SPAWNAREA)]
	protected string m_Name;
	
	[Attribute("", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(WR_SpawnRegion), category: CATEGORY_SPAWNAREA)]
	WR_SpawnRegion m_eSpawnRegion;
	
	[Attribute(defvalue: "true", desc: "Determines if players should spawn here", category: CATEGORY_SPAWNAREA)]
	protected bool m_IsActive;

	override void EOnInit(IEntity owner)
	{
		logger.LogDebug("Initializing " + GetSpawnAreaName() + " spawn area entity...");
	}
	
	string GetSpawnAreaName()
	{
		return m_Name;
	}
	
	WR_SpawnRegion GetSpawnRegion()
	{
		return m_eSpawnRegion;
	}
	
	//! Returns all player entities within the trigger.
	notnull array<IEntity> GetPlayerControlledEntitiesInsideTrigger()
	{
		array<IEntity> entitiesInside = {};
		GetEntitiesInside(entitiesInside);
		
		array<IEntity> result = {};
		foreach (IEntity entity : entitiesInside)
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
			if (!character)
				continue;
			
			PlayerManager playerManager = GetGame().GetPlayerManager();
			int playerId = playerManager.GetPlayerIdFromControlledEntity(entity);
			if (playerId == 0)
				continue;
			
			result.Insert(entity);
		}
		
		return result;
	}
	
//	//! Returns all the players in the trigger that are members of the given faction
//	array<IEntity> GetPlayersByFactionInsideTrigger(Faction faction)
//	{
//		array<IEntity> entitiesInside = {};
//		GetEntitiesInside(entitiesInside);
//		
//		array<IEntity> entitiesOut = {};
//		foreach (IEntity entity : entitiesInside)
//		{
//			if (!entity)
//				continue;
//
//			SCR_ChimeraCharacter chimeraCharacter = SCR_ChimeraCharacter.Cast(entity);
//			if (!chimeraCharacter)
//				entitiesOut.Insert(chimeraCharacter);
//
//			if (chimeraCharacter.GetFaction() != m_OwnerFaction)
//				continue;
//
//			if (EntityUtils.IsPlayer(entity))
//				entitiesOut.Insert(chimeraCharacter);
//		}
//	}
//	
//	//! Returns all the players in the trigger that are members of the given squad
//	array<IEntity> GetPlayersByGroupInsideTrigger(SCR_AIGroup group)
//	{
//		array<IEntity> entitiesInside = {};
//		GetEntitiesInside(entitiesInside);
//	
//		SCR_GroupsManagerComponent groupManager = SCR_GroupsManagerComponent.GetInstance();
//			
//		array<IEntity> entitiesOut = {};
//		foreach (IEntity entity : entitiesInside)
//		{
//			if (!entity)
//				continue;
//
//			SCR_ChimeraCharacter chimeraCharacter = SCR_ChimeraCharacter.Cast(entity);
//			if (!chimeraCharacter)
//				entitiesOut.Insert(chimeraCharacter);
//
//			// Compare group
//			chimeraCharacter
//			groupManager.GetPlayerGroup();
//			if (chimeraCharacter != m_OwnerFaction)
//				continue;
//
//			if (EntityUtils.IsPlayer(entity))
//				entitiesOut.Insert(chimeraCharacter);
//		}
//	}
}