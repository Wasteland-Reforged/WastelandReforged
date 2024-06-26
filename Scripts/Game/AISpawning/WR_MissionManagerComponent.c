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
	
	[Attribute("5", UIWidgets.Slider, "Weighted Spawn Chance of all other missions", "0 10 1")]
	protected float regularMissionWeight;
	
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
		//Choose a random mission type, instantiate that mission object, and add it to the active mission list
		switch(getRandomMissionType()) {
			case "geocache":
				activeMissionList.Insert(new WR_GeocacheMission("Geocache", getRandomMissionLocation("AIWaypoint")));
				break;
			case "capturebase":
				activeMissionList.Insert(new WR_AIMission("CaptureBase", getRandomMissionLocation("AIWaypointBase")));
				break;
			case "convoy":
				activeMissionList.Insert(new WR_ConvoyMission("Convoy", getRandomMissionLocation("AIWaypoint")));	//Mission location get overriden immediately anyways
				break;
			default:
				activeMissionList.Insert(new WR_AIMission(getRandomRegularMission(), getRandomMissionLocation("AIWaypoint")));
		}
	}
	
	protected void startInitialMissions()
	{
		//Create a new mission every X milliseconds until we have reached the max number of missions
		int timeBetweenSpawns = 1000;
		for (int i = 0; i < maxActiveMissions; i++) {
			GetGame().GetCallqueue().CallLater(startRandomMission, timeBetweenSpawns*i, false);
		}
	}
	
	protected string getRandomMissionType()
	{
		WR_WeightedItemArray<string> missionWeights = new WR_WeightedItemArray<string>();
		missionWeights.AddItem(geocacheMissionWeight, "geocache");
		missionWeights.AddItem(captureBaseMissionWeight, "capturebase");
		missionWeights.AddItem(convoyMissionWeight, "convoy");
		missionWeights.AddItem(regularMissionWeight, "regular");
		return missionWeights.GetRandomItem();
	}
	
	protected string getRandomRegularMission()
	{
		WR_WeightedItemArray<string> missionWeights = new WR_WeightedItemArray<string>();
		missionWeights.AddItem(5, "VehicleWreck");
		missionWeights.AddItem(4, "CaptureVehicle");
		missionWeights.AddItem(4, "MedicalSupplies");
		missionWeights.AddItem(3, "SniperSquad");
		missionWeights.AddItem(3, "HeavyWeapons");
		return missionWeights.GetRandomItem();
	}
	
	protected vector getRandomMissionLocation(string waypointType)
	{
		array<vector> allLocations = {};
		string locationName;
		int locationNameCounter = 1;
		
		//Gather all locations by iterating over every comment entity with the name "AIWaypointX", and adding their origin location to the array
		while (true) {
			locationName = (waypointType + locationNameCounter);
			IEntity newLocation = GetGame().GetWorld().FindEntityByName(locationName);
			if (!newLocation) break;
			allLocations.Insert(newLocation.GetOrigin());
			locationNameCounter++;
		}
		
		//Remove all locations that belong to a mission that is currently active
		foreach (WR_Mission mission : activeMissionList) {
			allLocations.RemoveItem(mission.getMissionLocation());
		}
		
		//Return a random location from the remaining ones
		return allLocations.GetRandomElement();
	}
	
}











