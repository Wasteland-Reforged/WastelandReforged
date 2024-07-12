[ComponentEditorProps(category: "AI Spawning", description: "Create and manage active missions")]
class WR_MissionManagerComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class WR_MissionManagerComponent : SCR_BaseGameModeComponent
{	
	[Attribute("5", UIWidgets.Slider, "Max # of missions that can be active at once", "0 60 1")]
	protected int maxActiveMissions;
	
	//This only triggers AFTER the mission entities are cleaned up, which is already some minutes after a player completes the mission
	[Attribute("3", UIWidgets.Slider, "How long to wait after a mission ends before starting its replacement (in minutes)", "1 30 1")]
	protected float missionRespawnTimer;
	
	[Attribute("5", UIWidgets.Slider, "Weighted Spawn Chance of Geocache missions", "0 10 1")]
	protected float geocacheMissionWeight;
	
	[Attribute("5", UIWidgets.Slider, "Weighted Spawn Chance of Capture Base missions", "0 10 1")]
	protected float captureBaseMissionWeight;
	
	[Attribute("5", UIWidgets.Slider, "Weighted Spawn Chance of Convoy missions", "0 10 1")]
	protected float convoyMissionWeight;
	
	[Attribute("5", UIWidgets.Slider, "Weighted Spawn Chance of Abandoned Heli missions", "0 10 1")]
	protected float abandonedHeliMissionWeight;
	
	[Attribute("5", UIWidgets.Slider, "Weighted Spawn Chance of Capture Heli missions", "0 10 1")]
	protected float captureHeliMissionWeight;
	
	[Attribute("5", UIWidgets.Slider, "Weighted Spawn Chance of Capture Vehicle missions", "0 10 1")]
	protected float captureVehicleMissionWeight;
	
	[Attribute("5", UIWidgets.Slider, "Weighted Spawn Chance of Vehicle Wreck missions", "0 10 1")]
	protected float vehicleWreckMissionWeight;
	
	[Attribute("5", UIWidgets.Slider, "Weighted Spawn Chance of Heavy Weapons missions", "0 10 1")]
	protected float heavyWeaponsMissionWeight;
	
	[Attribute("5", UIWidgets.Slider, "Weighted Spawn Chance of Medical Supplies missions", "0 10 1")]
	protected float medicalSuppliesMissionWeight;
	
	[Attribute("5", UIWidgets.Slider, "Weighted Spawn Chance of Sniper Squad missions", "0 10 1")]
	protected float sniperSquadMissionWeight;
	
	protected int numActiveMissions, missionRespawnInMS, missionCheckInMS;
	protected ref array<ref WR_Mission> activeMissionList;
	
	override void OnGameModeStart()
	{
		numActiveMissions = 0;
		activeMissionList = {};
		startInitialMissions();
		
		missionRespawnInMS = missionRespawnTimer * 60 * 1000;
		missionCheckInMS = 5000;
		
		//Every X milliseconds, check the current mission list to see if any have been completed and need to be replaced (The "true" parameter makes this function repeat endlessly)
		GetGame().GetCallqueue().CallLater(checkMissions, missionCheckInMS, true);
		
		//startCastleMission();
	}
	
	protected void startInitialMissions()
	{
		//Create a new mission every X milliseconds until we have reached the max number of missions
		int timeBetweenSpawns = 3000;
		for (int i = 0; i < maxActiveMissions; i++) {
			GetGame().GetCallqueue().CallLater(startRandomMission, timeBetweenSpawns*i, false);
		}
	}
	
	protected void checkMissions()
	{
		//Check if any of the missions are Completed. If so, remove them from the list, and queue another one to spawn some time later
		foreach (WR_Mission mission : activeMissionList) {
			if (mission.isMissionComplete()) {
				Print("[WR_MissionManagerComponent] Cleaning up mission");
				activeMissionList.RemoveItem(mission);
				GetGame().GetCallqueue().CallLater(startRandomMission, missionRespawnInMS, false);
				return;
			}
		}
	}
	
	protected void startRandomMission()
	{
		WR_WeightedItemArray<string> missionWeights = new WR_WeightedItemArray<string>();
		missionWeights.AddItem(geocacheMissionWeight, "geocache");
		missionWeights.AddItem(captureBaseMissionWeight, "capturebase");
		missionWeights.AddItem(captureHeliMissionWeight, "captureheli");
		missionWeights.AddItem(convoyMissionWeight, "convoy");
		missionWeights.AddItem(abandonedHeliMissionWeight, "abandonedheli");
		missionWeights.AddItem(captureBaseMissionWeight, "capturevehicle");
		missionWeights.AddItem(captureHeliMissionWeight, "vehiclewreck");
		missionWeights.AddItem(convoyMissionWeight, "heavyweapons");
		missionWeights.AddItem(abandonedHeliMissionWeight, "medicalsupplies");
		missionWeights.AddItem(sniperSquadMissionWeight, "snipersquad");
		
		//Choose a random mission type, instantiate that mission object, and add it to the active mission list
		string missionName = missionWeights.GetRandomItem();
		switch(missionName) {
			case "capturebase":
				activeMissionList.Insert(new WR_Mission("capturebase", getRandomMissionLocation({"AIWaypointBase"})));
				break;
			case "captureheli":
				activeMissionList.Insert(new WR_Mission("captureheli", getRandomMissionLocation({"AIWaypointBase"})));
				break;
			case "abandonedheli":
				activeMissionList.Insert(new WR_Mission("abandonedheli", getRandomMissionLocation({"AIWaypointBase"})));
				break;
			case "convoy":
				activeMissionList.Insert(new WR_ConvoyMission("convoy", getRandomMissionLocation({"AIWaypointConvoy"})));
				break;
			default:
				activeMissionList.Insert(new WR_Mission(missionName, getRandomMissionLocation({"AIWaypoint"})));
		}
	}
	
	protected vector getRandomMissionLocation(array<string> waypointTypes)
	{
		array<vector> allLocations = {};
		string locationName;
		int locationNameCounter;
		
		foreach (string waypointType : waypointTypes) {
			locationNameCounter = 1;
			
			//Gather all locations by iterating over every comment entity with the name "AIWaypointX", and adding their origin location to the array
			while (true) {
				locationName = (waypointType + locationNameCounter);
				IEntity newLocation = GetGame().GetWorld().FindEntityByName(locationName);
				if (!newLocation) break;
				allLocations.Insert(newLocation.GetOrigin());
				locationNameCounter++;
			}
		}
		
		//Remove all locations that belong to a mission that is currently active
		foreach (WR_Mission mission : activeMissionList) {
			allLocations.RemoveItem(mission.getMissionLocation());
		}
		
		//Return a random location from the remaining ones
		return allLocations.GetRandomElement();
	}
	
}











