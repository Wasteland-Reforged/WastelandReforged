//------------------------------------------------------------------------------------------------
modded class SCR_NameTagDisplay : SCR_InfoDisplayExtended
{	
	protected SCR_AIGroup m_CurrentGroup;
	
	//------------------------------------------------------------------------------------------------
	override protected void InitializeTag(IEntity entity, bool friendlyOnly = true)
	{
		if (!m_wRoot)
			return;
		
		// if this is a current controlled entity
		if (entity == SCR_PlayerController.GetLocalControlledEntity())
		{
			if (m_aUninitializedTags.IsEmpty())
				CreateTagWidget();
			
			// init tag & insert into arrays
			SCR_NameTagData tag = m_aUninitializedTags[0];
			m_CurrentPlayerTag = tag;
			tag.InitTag(this, entity, s_NametagCfg, true);
			
			m_aNameTags.Insert(tag);
			m_aNameTagEntities.Insert(entity, tag);
			m_aUninitializedTags.Remove(0);
			
			// Init FactionAffiliationComponent, required to detect factions
			FactionManager factionMgr = GetGame().GetFactionManager();
			if (factionMgr)
			{
				FactionAffiliationComponent factionComponent = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
				if (factionComponent)
					m_CurrentFaction = factionComponent.GetAffiliatedFaction();
			}
			
			m_CurrentGroup = SCR_GroupsManagerComponent.GetInstance().GetPlayerGroup(SCR_PlayerController.GetLocalPlayerId());
		
			return;
		}
		
		if (friendlyOnly)
		{
			// Do not create tags for enemy players
			if (!IsEntityFriendly(entity))
				return;
		}
			
		// Init tag and insert into arrays			
		if (m_aUninitializedTags.IsEmpty())
			CreateTagWidget();
		
		SCR_NameTagData tag = m_aUninitializedTags[0];
		if (tag.InitTag(this, entity, s_NametagCfg))
		{	
			m_aNameTags.Insert(tag);
			m_aNameTagEntities.Insert(entity, tag);
			m_aUninitializedTags.Remove(0);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	// Modified version of IsFactionFriendly.
	// If the entity and the local player are in the same faction and the faction is friendly to itself, the entity is friendly.
	// Or, if the entity and local player are the in same group, the entity is friendly, regardless of if the faction is friendly to itself.
	protected bool IsEntityFriendly(IEntity entity)
	{
		FactionAffiliationComponent factionComponent = FactionAffiliationComponent.Cast( entity.FindComponent( FactionAffiliationComponent ) );			
		
		if (!factionComponent)
			return false;
		
		if (m_CurrentFaction)
		{
			Faction faction = factionComponent.GetAffiliatedFaction();
			if (!faction)
				return false;
			
			// Is faction friendly to us?
			if (faction.IsFactionFriendly(m_CurrentFaction))
				return true;
			
			// Does the entity have a group? If so, are we in the same group?
			int entityControllerPlayerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(entity);
			if (entityControllerPlayerId != 0)
			{
				bool membersOfSameGroup = m_CurrentGroup == SCR_GroupsManagerComponent.GetInstance().GetPlayerGroup(entityControllerPlayerId);
				if (membersOfSameGroup)
					return true;
			}
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnGroupJoined(SCR_AIGroup group, int playerID)
	{
		IEntity ent = m_PlayerManager.GetPlayerControlledEntity(playerID);
		if (ent)
		{
			SCR_NameTagData tagData = m_aNameTagEntities.Get(ent);
			if (tagData)
				tagData.SetGroup(group);
		}

		SCR_NameTagData data = m_aNameTagEntities.Get(ent);
		if (data)
			CleanupTag(data);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnGroupLeft(SCR_AIGroup group, int playerID)
	{
		IEntity ent = m_PlayerManager.GetPlayerControlledEntity(playerID);
		if (ent)
		{
			SCR_NameTagData tagData = m_aNameTagEntities.Get(ent);
			if (tagData)
				tagData.SetGroup(null);
		}
		
		SCR_NameTagData data = m_aNameTagEntities.Get(ent);
		if (data)
			CleanupTag(data);
	}
}