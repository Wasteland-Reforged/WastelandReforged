modded class SCR_Faction
{
	[Attribute()]
	private ResourceName m_DefaultCharacterPrefab;
	
	ResourceName GetDefaultCharacterPrefab()
	{
		return m_DefaultCharacterPrefab;
	}
}