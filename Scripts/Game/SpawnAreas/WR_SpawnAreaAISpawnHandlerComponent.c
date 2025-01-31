[ComponentEditorProps(category: "Town AI Spawning", description: "Handles the random spawning/respawning of roaming bots in Towns")]
class WR_SpawnAreaAISpawnHandlerComponentClass : ScriptComponentClass
{
	
}

class WR_SpawnAreaAISpawnHandlerComponent : ScriptComponent
{
	ref WR_Logger<WR_SpawnAreaAISpawnHandlerComponent> logger = new WR_Logger<WR_SpawnAreaAISpawnHandlerComponent>(this);
	
	private WR_SpawnAreaEntity _parent;
	ref array<SCR_AIGroup> _aiGroups = {};
	static ref array<WR_SpawnAreaAISpawnHandlerComponent> m_aBotSpawnHandlers;
	
	[Attribute(defvalue: "50", desc: "Number of roaming bot groups to spawn per square kilometer of surface area inside this spawn area.")]
	protected int botsPerSqKm;
	
	[Attribute(defvalue: "2", desc: "Minimum number of roaming bot groups in this spawn area")]
	protected int AIGroupsFlatRate;
	
	override void OnPostInit(IEntity owner)
	{
		_parent = WR_SpawnAreaEntity.Cast(owner);
		if (!_parent)
		{
			logger.LogError("Parent entity of WR_SpawnAreaAISpawnHandlerComponent must be a WR_SpawnAreaEntity!");
			return;
		}
		
		if (!m_aBotSpawnHandlers)
		{
			m_aBotSpawnHandlers = {};
			logger.LogNormal("Initialized bot spawn handler array");
		}
		
		m_aBotSpawnHandlers.Insert(this);
		logger.LogDebug("Inserted " + GetSpawnAreaName() + " into the bot spawn handler component list");
		
		logger.LogNormal("Initialized.");
	}
	
	protected bool SpawnAIGroup()
	{
		// Set ResourceName for the AI Group Prefab
		ResourceName aiGroupResource = "{EE18BCEA7A5F399D}Prefabs/Groups/WR/WR_ScavGroup1.et";
		
		// Configure spawn position parameters
		float areaToCheck = 20; 		// Radius that will be checked if the initially passed pos is not safe
		float xzPaddingRadius = 3;		// Minimum radius of empty space to have around the chosen position
		float yPaddingDistance = 10; 	// Minimum distance of empty space to have above and below the chosen position 
			
		vector spawnPos;
		bool foundSafePos = WR_Utils.TryGetRandomSafePosWithinRadius(spawnPos, _parent.GetOrigin(), _parent.GetSphereRadius()
																	, areaToCheck, xzPaddingRadius, yPaddingDistance);
		
		if (!foundSafePos)
		{
			logger.LogWarning("Could not find safe position for roaming AI group!");
			return false;
		}
		
		// Spawn the group prefab
		IEntity aiGroupEntity = WR_Utils.SpawnPrefabInWorld(aiGroupResource, spawnPos);
		SCR_AIGroup aiGroup = SCR_AIGroup.Cast(aiGroupEntity);
	
		if (!aiGroup)
		{
			logger.LogWarning("Roaming AI Group Failed to Spawn!");
			return false;
		}
		
		// Guarantee this group becomes null when last member dies
		aiGroup.SetDeleteWhenEmpty(true);
		
		// All NPC fighters should be on civilian faction
		aiGroup.SetFaction(GetGame().GetFactionManager().GetFactionByKey("CIV"));	
		
		// Command the group to roam around the GetOwner		
		ResourceName waypointResource = "{A33AF7FC5004F294}Prefabs/AI/Waypoints/AIWaypoint_Defend_Large_CO.et";
		IEntity waypointEntity = WR_Utils.SpawnPrefabInWorld(waypointResource, _parent.GetOrigin());
		SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(waypointEntity);
		aiGroup.AddWaypoint(waypoint);
		
		_aiGroups.Insert(aiGroup);
		return true;
	}
	
	void CheckGroups(bool doRespawn) 
	{
		foreach(SCR_AIGroup group : _aiGroups)
		{
			if (!group)
			{
				logger.LogDebug("Found dead bot group at " + GetSpawnAreaName());
				_aiGroups.RemoveItem(group);
				CheckGroups(false);
				return;
			}
		}
		
		//If any of the bot groups are found to be empty, doRespawn is set to false, and we will not enter this block
		if (doRespawn) {
			TryRespawnGroup();
		}
	}
	
	private int GetAICountPerSqKm()
	{
		// Calculate the surface area of the spawn
		float spawnAreaRadius = _parent.GetSphereRadius();
		float areaSqKm = (Math.PI * spawnAreaRadius * spawnAreaRadius) / (1000 * 1000);

		return Math.Floor(areaSqKm * botsPerSqKm);
	}
	
	protected int GetDesiredGroups()
	{
		int groupsToSpawn = GetAICountPerSqKm();
		if (groupsToSpawn < AIGroupsFlatRate) groupsToSpawn = AIGroupsFlatRate;
		return groupsToSpawn;
	}
	
	protected void TryRespawnGroup()
	{
		const int MAX_ATTEMPTS = 10;
		int currentAttempt = 1;
		
		for (int i = _aiGroups.Count(); i < GetDesiredGroups(); i++)
		{
			if (!SpawnAIGroup())
			{
				i--;
			}
			else
			{
				logger.LogDebug("Spawned a new bot group at " + GetSpawnAreaName());
				return;
			}
			
			currentAttempt++;
			if (currentAttempt >= MAX_ATTEMPTS)
			{
				logger.LogDebug("Could not spawn roaming bot group in a reasonable number of attempts. Giving up until next pass.");
				return;
			}
			
		}
	}
	
	string GetSpawnAreaName()
	{
		return _parent.GetSpawnAreaName();
	}
	
	void ~WR_SpawnAreaAISpawnHandlerComponent()
	{
		delete _aiGroups;
	}
}