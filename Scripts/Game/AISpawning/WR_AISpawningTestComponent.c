[ComponentEditorProps(category: "AI Spawning", description: "Test spawning groups of AI that roam towns")]
class WR_AISpawningTestComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class WR_AISpawningTestComponent : SCR_BaseGameModeComponent
{	
	protected void AISpawner(string spawnGroup, vector spawnPosition, string waypointType, vector waypointPosition)
	{
		//Generate Resource for spawn group
		Resource groupResource = Resource.Load(spawnGroup);
		
		if (!groupResource || !groupResource.IsValid()) {
			Print(("[AI Spawner] Unable to load resource for spawn group" + spawnGroup), LogLevel.ERROR);
			return;
		}
		
		//Generate the spawn parameters and spawn the spawnGroup
		SCR_AIGroup group = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(groupResource, GetGame().GetWorld(), GenerateSpawnParameters(spawnPosition)));
		
		if (!group) {
			Print("[AI Spawner] Unable to spawn group!", LogLevel.ERROR);
			return;
		}

		//Generate Resource for waypoint
		Resource waypointResource = Resource.Load(waypointType);
		
		if (!waypointResource || !waypointResource.IsValid()) {
			Print(("[AI Spawner] Unable to load resource for waypoint" + waypointType), LogLevel.ERROR);
			return;
		}
		
		//Generate spawn parameters and create the AIWaypoint
		SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(waypointResource, GetGame().GetWorld(), GenerateSpawnParameters(waypointPosition)));
		
		if (!waypoint) {
			Print("[AI Spawner] Unable to create waypoint!", LogLevel.ERROR);
			return;
		}
		
		//Assign the waypoint to the group
		group.AddWaypoint(waypoint);
	}
	
	protected EntitySpawnParams GenerateSpawnParameters(vector position)
	{
		//Create a new set of spawn parameters
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		
		//Assign the position to those parameters
		params.Transform[3] = position;
		
		//Return this set of spawn parameters
		return params;
	}
	
	override void OnGameModeStart()
	{
		//String variables for resources
		string spawnGroup = "{E552DABF3636C2AD}Prefabs/Groups/OPFOR/Group_USSR_RifleSquad.et";
		string waypointType = "{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et";
		
		//Entity variables for positions
		IEntity spawnPosition = GetGame().GetWorld().FindEntityByName("AISpawnPoint1");
		IEntity waypointPosition = GetGame().GetWorld().FindEntityByName("AIWaypoint1");
		
		//Execute the AI spawning using a delayed call
		GetGame().GetCallqueue().CallLater(AISpawner, 5000, false, spawnGroup, spawnPosition.GetOrigin(), waypointType, waypointPosition.GetOrigin());
	
	}
}