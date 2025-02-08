[ComponentEditorProps(category: "Spawning", description: "Populate crates with random loot items")]
class WR_LootBoxComponentClass : ScriptComponentClass {}

class WR_LootBoxComponent : ScriptComponent
{
	ref WR_Logger<WR_LootBoxComponent> logger = new WR_Logger<WR_LootBoxComponent>(this);
	ref static array<WR_LootBoxComponent> s_aLootBoxComponents;

	override void OnPostInit(IEntity owner)
	{
		if (!s_aLootBoxComponents)
		{
			s_aLootBoxComponents = {};
			logger.LogNormal("Initialized loot box component list.");
		}
		
		s_aLootBoxComponents.Insert(this);
	}
	
	static array<WR_LootBoxComponent> GetLootBoxComponents()
	{
		return s_aLootBoxComponents;
	}
	
	void ~WR_LootBoxComponent()
	{
		s_aLootBoxComponents.RemoveItem(this);
	}
}