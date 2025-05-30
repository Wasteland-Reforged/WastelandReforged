modded class SCR_AIGroup : ChimeraAIGroup
{
	ref WR_Logger<SCR_AIGroup> logger = new WR_Logger<SCR_AIGroup>(this);
	
	override bool SpawnGroupMember(bool snapToTerrain, int index, ResourceName res, bool editMode, bool isLast)
	{
		if (!GetGame().GetAIWorld().CanLimitedAIBeAdded())
		{
			if (isLast)
				Event_OnInit.Invoke(this);
			
			return true;
		}
		BaseWorld world = GetWorld();
		AIFormationDefinition formationDefinition;
		AIFormationComponent formationComponent = AIFormationComponent.Cast(this.FindComponent(AIFormationComponent));
		if (formationComponent)
			formationDefinition = formationComponent.GetFormation();
		EntitySpawnParams spawnParams = new EntitySpawnParams;
		spawnParams.TransformMode = ETransformMode.WORLD;
		GetWorldTransform(spawnParams.Transform);
		vector pos = spawnParams.Transform[3];
		
		if (formationDefinition)
			pos = CoordToParent(formationDefinition.GetOffsetPosition(index));
		else
			pos = CoordToParent(Vector(index, 0, 0));
		
		if (snapToTerrain)
		{
			float surfaceY = world.GetSurfaceY(pos[0], pos[2]);
			pos[1] = surfaceY;
		}
		
		
		// Snap to the nearest navmesh point
		AIPathfindingComponent pathFindindingComponent = AIPathfindingComponent.Cast(this.FindComponent(AIPathfindingComponent));
		
		if (!editMode)
		{
			NavmeshWorldComponent navmesh = pathFindindingComponent.GetNavmeshComponent();
			if (navmesh)
			{	 
				if (navmesh.IsTileRequested(pos))
				{
					return false;	
				}
				if (!navmesh.IsTileLoaded(pos))
				{
					navmesh.LoadTileIn(pos);
					return false;
				}
			}
		}
		
		if (pathFindindingComponent && pathFindindingComponent.GetClosestPositionOnNavmesh(pos, "10 10 10", pos))
		{
			float groundHeight = world.GetSurfaceY(pos[0], pos[2]);
			if (pos[1] < groundHeight)
				pos[1] = groundHeight;
			vector outWaterSurfacePoint;
			EWaterSurfaceType waterSurfaceType;
			vector transformWS[4];
			vector obbExtents;
			if (ChimeraWorldUtils.TryGetWaterSurface(GetWorld(), pos, outWaterSurfacePoint, waterSurfaceType, transformWS, obbExtents))
			{
				pos = outWaterSurfacePoint;
			}
		}
		
		spawnParams.Transform[3] = pos;
		
		IEntity member = GetGame().SpawnEntityPrefabEx(res, true, world, spawnParams);
		if (!member)
			return true;
		
		// Move in to vehicle 
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(member.FindComponent(SCR_EditableEntityComponent));
		
		
		if (editMode)
			m_aSceneGroupUnitInstances.Insert(member);
		
		// Even same null-check is above, in some situations, member can get deleted and it would result in VME
		if (!member)
			return true;
		
		// Add Money
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(member.FindComponent(SCR_InventoryStorageManagerComponent));
        if (inventory)
		{
			// Chance to carry cash
			// TODO: parameterize this probability
			if (Math.RandomFloat01() < 0.75)
			{
				int quantity = GetRandomMoneyQuantity(200, 10);
				EL_MoneyUtils.AddCash(inventory, quantity);
			}
		}
		else
		{
			logger.LogWarning("Could not find character's storage manager!");
		}

		// Add to group
		AddAIEntityToGroup(member);
		
		FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(member.FindComponent(FactionAffiliationComponent));
		
		if (factionAffiliation)
			factionAffiliation.SetAffiliatedFactionByKey(m_faction);
	
		if (isLast)
			Event_OnInit.Invoke(this);
		
		return true;
	}
	
	string GetDisplayName()
	{
		if (m_sCustomName && m_sCustomName.Length() > 0)
		{
			return m_sCustomName;
		}
		else
		{
			string company, platoon, squad, character, format;
			GetCallsigns(company, platoon, squad, character, format);
			string callsign = string.Format(format, WidgetManager.Translate(company), platoon, squad, character);
			
			return callsign;
		}
	}

	private int GetRandomMoneyQuantity(int max, int stepSize)
	{
		float x = Math.RandomFloat01();
		int amountToGive = ((1 - (x * x)) * max) + 1;
		return (amountToGive / stepSize) * stepSize;
	}
}