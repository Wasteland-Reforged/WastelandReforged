[BaseContainerProps(configRoot: true)]
class WR_MissionSystemConfig
{
	[Attribute("", UIWidgets.Object, desc: "List of all mission definitions that can be used to create missions.")]
	ref array<ref WR_MissionDefinition> m_aMissionDefinitions;
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Slider, params: "0 60 0.01", desc: "(minutes) How long to wait after game mode start before creating missions.")]
	float m_fInitialMissionDelayM;
	
	[Attribute(defvalue: "5", uiwidget: UIWidgets.Slider, params: "0.01 60 0.01", desc: "(minutes) How long to wait to create a new mission after one ends.")]
	float m_fNewMissionDelayM;
	
	[Attribute(defvalue: "5", uiwidget: UIWidgets.Slider, params: "0 60 0.01", desc: "(minutes) How long to wait to start the mission after displaying the 'Upcoming Objective' notification. \n\nExample: 0 -> The 'Upcoming Objective' notifications will not be displayed.\nExample: 5 -> The 'Upcoming Objective' notifications will be displayed upon mission creation and the mission will start 5 minutes later.")]
	float m_fNewMissionNotificationDelayM;
	
	[Attribute(defvalue: "0.5", uiwidget: UIWidgets.Slider, params: "0 5 0.1", desc: "(minutes) How long to wait to delete a mission's map marker after it ends.")]
	float m_fMissionMapMarkerCleanupDelayM;
	
	[Attribute(defvalue: "5", uiwidget: UIWidgets.Slider, params: "0 60 0.01", desc: "(minutes) How long to wait to delete mission entities after it ends.")]
	float m_fMissionCleanupDelayM;
	
	[Attribute(defvalue: "60", uiwidget: UIWidgets.Slider, params: "0 120 1", desc: "(minutes) Time limit for missions. \n\n Example: 0 -> no time limit")]
	float m_fMissionTimeLimitM;
	
	[Attribute(defvalue: "10", uiwidget: UIWidgets.Slider, params: "0 128 1", desc: "Server must have at least this many players to spawn missions with 'free' difficulty")]
	int m_freeMissionThreshold;
	
	[Attribute(defvalue: "5", uiwidget: UIWidgets.Slider, params: "0 128 1", desc: "Server must have at least this many players to spawn missions of 'hard' difficulty")]
	int m_hardMissionThreshold;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.Slider, params: "0 128 1", desc: "Array of server player count thresholds that determine how many mission slots are open. Each number in the array represents a threshold at which another mission slot will open. This list will always automatically be sorted in ascending order.\n\nExample: [0, 20, 50] -> 1 mission slot at 0-19 players, 2 mission slots at 20-49 players, and 3 mission slots at 50+ players.\nExample: [0, 0] -> There are always 2 mission slots available, regardless of the number of players on the server.\n\n")]
	ref array<int> m_aPlayercountMissionThresholds;
}