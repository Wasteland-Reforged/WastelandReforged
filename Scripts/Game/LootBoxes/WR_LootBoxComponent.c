[ComponentEditorProps(category: "Spawning", description: "Populate crates with random loot items")]
class WR_LootBoxComponentClass : ScriptComponentClass
{
	
}

class WR_LootBoxComponent : ScriptComponent
{
	ref static array<WR_LootBoxComponent> LootBoxComponents;

	override void OnPostInit(IEntity owner)
	{
		if (!LootBoxComponents)
		{
			LootBoxComponents = {};
			Print("[WASTELAND] Initialized loot box component list.");
		}
		
		LootBoxComponents.Insert(this);
	}
	
	void ~WR_LootBoxComponent()
	{
		LootBoxComponents.RemoveItem(this);
	}
}