modded class SCR_ArsenalManagerComponent
{
	//------------------------------------------------------------------------------------------------
	//! Get the cost of spawning as a player at given base
	//! \param[in] playerLoadout Loadout to check supply cost for
	//! \param[in] getLocalPlayer If true and the loadout is a SCR_PlayerArsenalLoadout then it will get the locally stored data for the arsenal loadout
	//! \param[in] playerID Player ID of player that want's to spawn, Required only if !getLocalPlayer
	//! \param[in] playerFaction (Optional) Faction of player to spawn to speed up getting the spawn cost data from Catalog
	//! \param[in] spawnTarget Spawnpoint of spawn location. Not required but without it, it cannot know if the spawnpoint has supplies enabled or other values if the spawnpoint is a base
	//! \param[out] spawnPointParentBase Returns the base if the spawnpoint is a base composition
	//! \param[out] spawnpointResourceComponent Returns the Resource component from the spawnpoint or base
	//! \return Returns the total cost of spawning with the given loadout
	override static float GetLoadoutCalculatedSupplyCost(notnull SCR_BasePlayerLoadout playerLoadout, bool getLocalPlayer, int playerID, SCR_Faction playerFaction, IEntity spawnTarget, out SCR_CampaignMilitaryBaseComponent spawnPointParentBase = null, out SCR_ResourceComponent spawnpointResourceComponent = null)
	{	
		// TODO: make it possible to turn off supply costs at base.
		// See the unmodded class for inspiration, but probably put parameters for this on the wasteland gamemode or something
		
		// Get resource component of command tent
		if (spawnTarget)
		{
			//~ Check if spawn target is an MHQ
			if (spawnTarget.FindComponent(SCR_CampaignMobileAssemblyStandaloneComponent))
				return 0;
			
			IEntity parent = spawnTarget;
			
			//~ Check if spawn target is a base
			while (parent)
			{
				spawnPointParentBase = SCR_CampaignMilitaryBaseComponent.Cast(parent.FindComponent(SCR_CampaignMilitaryBaseComponent));
	
				if (spawnPointParentBase)
					break;
	
				parent = parent.GetParent();
			}
			
			if (spawnPointParentBase)
				spawnpointResourceComponent = spawnPointParentBase.GetResourceComponent();
			else 
				spawnpointResourceComponent = SCR_ResourceComponent.FindResourceComponent(spawnTarget);
		}
		
		// Get supply cost parameter
		// TODO: parameterize this
		return 100.0;
	}
}