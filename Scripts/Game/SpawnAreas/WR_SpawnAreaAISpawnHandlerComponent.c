[ComponentEditorProps(category: "Town AI Spawning", description: "Handles the random spawning/respawning of roaming bots in Towns")]
class WR_SpawnAreaAISpawnHandlerComponentClass : ScriptComponentClass
{
	
}

class WR_SpawnAreaAISpawnHandlerComponent : ScriptComponent
{
	private WR_SpawnAreaEntity _parent;
	ref static array<WR_SpawnAreaAISpawnHandlerComponent> AISpawnHandlerComponents;
	ref array<SCR_AIGroup> _aiGroups = {};
	
	[Attribute(defvalue: "75", desc: "Number of roaming bot groups to spawn per square kilometer of surface area inside this spawn area.")]
	protected int botsPerSqKm;
	
	[Attribute(defvalue: "2", desc: "Minimum number of roaming bot groups in this spawn area")]
	protected int AIGroupsFlatRate;
	
	[Attribute(defvalue: "0.25", desc: "Chance per group to spawn 2 scavs instead of 1")]
	protected float chanceOfLargerGroup;
	
	override void OnPostInit(IEntity owner)
	{
		_parent = WR_SpawnAreaEntity.Cast(owner);
		if (!_parent)
		{
			Print("[WASTELAND] WR_SpawnAreaAISpawnHandlerComponent: Parent entity of WR_SpawnAreaAISpawnHandlerComponent must be a WR_SpawnAreaEntity!", LogLevel.ERROR);
			return;
		}
		
		if (!AISpawnHandlerComponents)
		{
			AISpawnHandlerComponents = {};
			Print("[WASTELAND] WR_SpawnAreaAISpawnHandlerComponent: Initialized AI spawn area handler component list.", LogLevel.NORMAL);
		}
		
		AISpawnHandlerComponents.Insert(this);
		Print("[WASTELAND] WR_SpawnAreaAISpawnHandlerComponent: Inserted " + GetSpawnAreaName() + " into the AI spawn handler component list", LogLevel.SPAM);
	}
	
	void SpawnInitialAIGroups(out int successfulAIGroupSpawnCount)
	{
		int groupsToSpawn = GetAICountPerSqKm();
		if (groupsToSpawn < AIGroupsFlatRate) groupsToSpawn = AIGroupsFlatRate;
		Print("[WASTELAND] WR_SpawnAreaAISpawnHandlerComponent: Attempting to spawn " + groupsToSpawn + " AI groups in " + GetSpawnAreaName(), LogLevel.SPAM);
		
		// Generate max amount of AI groups at mission start
		successfulAIGroupSpawnCount = 0;	
		for (int i = 0; i < groupsToSpawn; i++) {
			if (spawnAIGroup()) {
				successfulAIGroupSpawnCount++;
			}
			else {
				i--;
			}		
		}
		
		// Call CheckRoamingAI() continuously every 1 minute to handle respawning of groups
		float checkTimerInMinutes = 0.5;
		GetGame().GetCallqueue().CallLater(CheckRoamingAI, checkTimerInMinutes * 60 * 1000, true);
	}
	
	bool spawnAIGroup()
	{
		// Set ResourceName for the AI Group Prefab
		ResourceName aiGroupResource = "{EE18BCEA7A5F399D}Prefabs/Groups/WR/WR_ScavGroup1.et";
		if (Math.RandomFloat01() < chanceOfLargerGroup) aiGroupResource = "{824C880AC69BB6B3}Prefabs/Groups/WR/WR_ScavGroup2.et";
		
		// Configure spawn position parameters
		float areaToCheck = 20; 		// Radius that will be checked if the initially passed pos is not safe
		float xzPaddingRadius = 3;		// Minimum radius of empty space to have around the chosen position
		float yPaddingDistance = 10; 	// Minimum distance of empty space to have above and below the chosen position 
			
		vector spawnPos;
		bool foundSafePos = WR_Utils.TryGetRandomSafePosWithinRadius(spawnPos, _parent.GetOrigin(), _parent.GetSphereRadius()
																	, areaToCheck, xzPaddingRadius, yPaddingDistance);
		
		if (!foundSafePos) {
			Print("[WASTELAND] WR_SpawnAreaAISpawnHandlerComponent: Could not find safe position for roaming AI group!", LogLevel.WARNING);
			return false;
		}
		
		// Spawn the group prefab
		IEntity aiGroupEntity = WR_Utils.SpawnPrefabInWorld(aiGroupResource, spawnPos);
		SCR_AIGroup aiGroup = SCR_AIGroup.Cast(aiGroupEntity);
	
		if (!aiGroup) {
			Print("[WASTELAND] WR_SpawnAreaAISpawnHandlerComponent: Roaming AI Group Failed to Spawn!", LogLevel.WARNING);
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
	
	void CheckRoamingAI()
	{
		foreach (auto aigroup : _aiGroups) {
			if (!aigroup) {
				Print("[WASTELAND] WR_SpawnAreaAISpawnHandlerComponent: Clearing dead AI Group..", LogLevel.SPAM);
				GetGame().GetCallqueue().CallLater(spawnAIGroup, 3 * 60 * 1000, false);
				_aiGroups.RemoveItemOrdered(aigroup);
			}
		}

	}
	
	private int GetAICountPerSqKm()
	{
		// Calculate the surface area of the spawn
		float spawnAreaRadius = _parent.GetSphereRadius();
		float areaSqKm = (Math.PI * spawnAreaRadius * spawnAreaRadius) / (1000 * 1000);

		return Math.Floor(areaSqKm * botsPerSqKm);
	}
	
	string GetSpawnAreaName()
	{
		return _parent.GetSpawnAreaName();
	}
	
	void ~WR_SpawnAreaAISpawnHandlerComponent()
	{
		if (!AISpawnHandlerComponents) return;
		
		if (AISpawnHandlerComponents.Count() == 0)
		{
			AISpawnHandlerComponents = null;
			return;
		}
		
		AISpawnHandlerComponents.RemoveItem(this);
	}
}