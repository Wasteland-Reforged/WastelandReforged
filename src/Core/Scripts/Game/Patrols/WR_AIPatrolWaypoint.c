[EntityEditorProps(category: "WastelandReforged", description: "Marks waypoints for AI patrols to travel between.")]
class WR_AIPatrolWaypointClass : SCR_ScenarioFrameworkTriggerEntityClass
{
	
}

class WR_AIPatrolWaypoint : SCR_ScenarioFrameworkTriggerEntity
{
	ref WR_Logger<WR_AIPatrolWaypoint> logger = new WR_Logger<WR_AIPatrolWaypoint>(this);
	//private static ref array<WR_AIPatrolWaypoint> PatrolWaypoints;
	protected BaseRplComponent m_RplComponent;
	
	[Attribute(defvalue: "0", desc: "Determines the order in which waypoints are traversed by AI patrols", params: "0 100 1", category: "Waypoints")]
	protected int m_iWaypointOrderIndex;
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		m_RplComponent = BaseRplComponent.Cast(FindComponent(BaseRplComponent));
		if (!m_RplComponent)
		{
			logger.LogError("Parent entity must have RplComponent!");
			return;
		}
		
		SetActivationPresence(SCR_EScenarioFrameworkTriggerActivation.PLAYER);
		WR_AIPatrolSystem.InsertNewWaypoint(this);
	}
	
	int GetWaypointOrderIndex()
	{
		return m_iWaypointOrderIndex;
	}

}