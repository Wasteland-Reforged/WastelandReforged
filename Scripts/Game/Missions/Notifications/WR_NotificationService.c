[ComponentEditorProps(category: "Game Mode", description: "Handles notifications sent by the Mission System.")]
class WR_MissionNotificationComponentClass : ScriptComponentClass {}

class WR_MissionNotificationComponent : ScriptComponent
{
	ref WR_Logger<WR_MissionNotificationComponent> logger = new WR_Logger<WR_MissionNotificationComponent>(this);
	RplComponent m_RplComponent;
	
	WR_MissionSystemConfig m_MissionSystemConfig;
	
	override void OnPostInit(IEntity owner)
	{
		m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		
		WR_GameModeWasteland gameMode = WR_GameModeWasteland.Cast(owner);
		if (!gameMode)
		{
			logger.LogError("The mission notification component must be a child of the Wasteland game mode!");
			return;
		}
		
		m_MissionSystemConfig = gameMode.GetMissionSystemConfig();
	}
	
	void SendNotification(WR_Mission mission)
	{
		if (m_RplComponent.Role() != RplRole.Authority)
		{
			logger.LogError("Only the authority may send mission notifications!");
			return;
		}
		
		WR_MissionNotification notif = WR_MissionNotification.CreateMissionNotification(
			mission.GetType(),
			mission.GetStatus(),
			mission.GetCompletionType(),
			mission.GetCompletingPlayerId()
		);
		
		// Broadcast hint notification to all connected clients
		Rpc(RpcDo_Broadcast_ShowHintFromNotification, notif);
		
		// Show hint to host (for player-hosted servers)
		DisplayHint(notif);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	private void RpcDo_Broadcast_ShowHintFromNotification(WR_MissionNotification notif)
	{
		if (m_RplComponent.Role() != RplRole.Proxy)
		{
			logger.LogError("Only a proxy may receive mission notifications!");
			return;
		}
		
		logger.LogNormal("Received mission notification!");
		logger.LogNormal("Type:              " + notif.m_eMissionType);
		logger.LogNormal("Status:            " + notif.m_eMissionStatus);
		logger.LogNormal("Completion type:   " + notif.m_eCompletionType);
		logger.LogNormal("Completing player: " + notif.m_iCompletingPlayerId);
		
		DisplayHint(notif);
	}
	
	private void DisplayHint(WR_MissionNotification notif)
	{
		WR_MissionDefinition definition = GetMissionDefinitionByType(notif.GetMissionType());
		
		string title;
		string description;
		string sound;
		
		switch (notif.GetMissionStatus())
		{
			case WR_MissionStatus.Pending:
			{
				float delayM = m_MissionSystemConfig.m_fNewMissionNotificationDelayM;
				
				title = string.Format("Upcoming Objective: %1", definition.m_sName);
				if (delayM == 1)
				{
					description = string.Format("New objective starting in %1 minute!", delayM);
				}
				else if (delayM < 1)
				{
					float delayS = Math.Round(WR_Utils.MinutesToSeconds(delayM));
					if (delayS == 1)
						description = string.Format("New objective starting in %1 second!", delayS);
					else
						description = string.Format("New objective starting in %1 seconds!", delayS);
				}
				else
				{
					description = string.Format("New objective starting in %1 minutes!", delayM);
				}
				
				sound = SCR_SoundEvent.TASK_CREATED;
				
				break;
			}
			case WR_MissionStatus.InProgress:
			{
				title = string.Format("Objective Started: %1", definition.m_sName);
				description = definition.m_sDescription;
				sound = SCR_SoundEvent.TASK_CREATED;
				
				break;
			}
			case WR_MissionStatus.Complete:
			{
				WR_MissionCompletionType compType = notif.GetMissionCompletionType();
				int completingPlayerId = notif.GetCompletingPlayerId();

				Faction playerFaction = SCR_FactionManager.SGetPlayerFaction(completingPlayerId);
				SCR_GroupsManagerComponent groupManager = SCR_GroupsManagerComponent.GetInstance();
				SCR_AIGroup playerGroup = groupManager.GetPlayerGroup(completingPlayerId);
				
				switch (compType)
				{
					case WR_MissionCompletionType.Success:
					{
						title = string.Format("Objective Complete: %1", definition.m_sName);
						
						if (playerFaction && playerGroup)
							description = string.Format("%1 group %2 completed the objective successfully.", playerFaction.GetFactionName(), playerGroup.GetDisplayName());
						else
							description = "The objective was completed successfully.";
						
						sound = SCR_SoundEvent.TASK_SUCCEED;
						
						break;
					}
					case WR_MissionCompletionType.Destroyed:
					{
						title = string.Format("Objective Failed: %1", definition.m_sName);
						
						if (playerFaction && playerGroup)
							description = string.Format("%1 group %2 destroyed the objective.", playerFaction.GetFactionName(), playerGroup.GetDisplayName());
						else
							description = "The objective was destroyed.";
						
						sound = SCR_SoundEvent.TASK_FAILED;

						break;
					}
					case WR_MissionCompletionType.TimedOut:
					{
						title = string.Format("Objective Failed: %1", definition.m_sName);
						description = "The objective was not completed in the allotted time.";
						sound = SCR_SoundEvent.TASK_FAILED;
						break;
					}
					default:
					{
						title = string.Format("Objective Ended: %1", definition.m_sName);
						description = "The objective has ended.";
						sound = SCR_SoundEvent.TASK_FAILED;
						break;
					}
				}
				
				break;
			}
		}
		
		WR_MissionHintHelper.ShowHintWithSound(title, description, sound);
	}
	
	private WR_MissionDefinition GetMissionDefinitionByType(WR_MissionType type)
	{
		foreach (WR_MissionDefinition def : m_MissionSystemConfig.m_aMissionDefinitions)
		{
			if (def.m_eType == type)
				return def;
		}
		
		return new WR_MissionDefinition();
	}
}