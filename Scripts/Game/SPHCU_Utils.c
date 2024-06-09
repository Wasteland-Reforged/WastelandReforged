class SPHCU_Utils
{
	//------------------------------------------------------------------------------------------------
	static vector GetRandomPointWithinCircle(vector center, float radius)
	{
		float randomDisplacement = Math.RandomFloat(0, radius);
		vector randomDirection = GetRandomHorizontalDirectionAngles().AnglesToVector();
		
		return center + randomDisplacement * randomDirection;
	}
	
	//------------------------------------------------------------------------------------------------
	static vector GetRandomHorizontalDirectionAngles()
	{
		return Vector(Math.RandomFloat(0, 360), 0, 0);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find a safe position. Leverages SCR_WorldTools.FindEmptyTerrainPosition
	Generally used for spawning players and vehicles within spawn areas.
	\param[out] safePos Variable filled with found position.
	\param centerPos Center of queried area. Height is irrelevant, only terrain positions are queried.
	\param radiusToSelectPointsWithin Radius within which a point will be randomly selected.
	\param radiusToCheckAroundInitiallySelectedPos Radius within which a safe position will be searched for if the initially selected point is not safe.
	\param xzPaddingRadius Radius of the cylinder that we are attempting to find a safe position for.
	\param yPadding Height of the cylinder that we are attempting to find a safe position for. 
	\return True if position was found
	*/
	static bool TryGetSafePos(out vector safePos, vector centerPos, float radiusToSelectPointsWithin, float radiusToCheckAroundInitiallySelectedPos, float xzPaddingRadius = 0.5, float yPaddingDistance = 2)
	{			
		vector posToCheck = SPHCU_Utils.GetRandomPointWithinCircle(centerPos, radiusToSelectPointsWithin);
		
		// Calculate safe pos
		vector selectedPos;
		bool foundSafePos = SCR_WorldTools.FindEmptyTerrainPosition(selectedPos, posToCheck, radiusToCheckAroundInitiallySelectedPos, xzPaddingRadius, yPaddingDistance);

		// Check if selected position is underwater
		bool isPosUnderWater = SCR_TerrainHelper.GetTerrainY(selectedPos, GetGame().GetWorld(), true) == 0;
		
		// If it is, try to find another empty position.
		while (isPosUnderWater)
		{
			posToCheck = SPHCU_Utils.GetRandomPointWithinCircle(centerPos, radiusToSelectPointsWithin);
			foundSafePos = SCR_WorldTools.FindEmptyTerrainPosition(selectedPos, posToCheck, radiusToCheckAroundInitiallySelectedPos, xzPaddingRadius, yPaddingDistance);
			isPosUnderWater = SCR_TerrainHelper.GetTerrainY(selectedPos, GetGame().GetWorld(), true) == 0;
		}
		
		//selectedPos[1] = selectedPos[1] + 2; // Add a constant to y component to prevent vehicles spawning underground
		
		safePos = selectedPos;
		return foundSafePos;
	}
}