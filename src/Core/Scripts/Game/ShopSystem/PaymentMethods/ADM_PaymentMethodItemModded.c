modded class ADM_PaymentMethodItem : ADM_PaymentMethodBase
{
	override bool DistributePayment(IEntity player, int quantity = 1)
	{
		if (!Replication.IsServer()) 
			return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) 
			return false;
		
		bool returned = true;
		for (int i = 0; i < m_ItemQuantity * quantity; i++)
		{
			IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(m_ItemPrefab));
			bool insertedItem = inventory.TryInsertItem(item, EStoragePurpose.PURPOSE_ANY, null);
			if (!insertedItem) 
				returned = false;
		}
		
		InventoryStorageManagerComponent invMan = EL_InventoryUtils.GetResponsibleStorageManager(player);
		int cashAdded = EL_MoneyUtils.AddCash(invMan, 10000);
		
		return returned;
	}
}