class WR_SpawnAreaControllerComponentClass : SCR_BaseGameModeComponentClass
{
	
}

class WR_SpawnAreaControllerComponent : SCR_BaseGameModeComponent
{
	protected IEntity _parent;
	
	override void OnPostInit(IEntity owner)
	{
		_parent = owner;
	}
	
	override void OnGameModeStart()
	{

	}


}