[ComponentEditorProps(category: "Spawning", description: "Populate crates with random loot items")]
class WR_LootBoxComponentClass : ScriptComponentClass
{
	
}

class WR_LootBoxComponent : ScriptComponent
{
	ref static array<WR_LootBoxComponent> LootBoxComponents;

	override void OnPostInit(IEntity owner)
	{
		LootBoxComponents.Insert(this);
	}
}