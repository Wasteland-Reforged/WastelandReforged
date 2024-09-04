// Payment method using Everon Life cash
class ADM_PaymentMethodElCash : ADM_PaymentMethodCurrency
{
	static const ResourceName PREFAB_CASH = "{E2EEC5CBFF087DE3}Prefabs/EveronLife/Items/Currencies/MoneyStack.et";
	
	void SetQuantity(int quantity)
	{
		m_Quantity = quantity;
	}
	
	override bool CheckPayment(IEntity player, int quantity = 1)
	{
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) 
			return false;
		
		int totalCurrency = EL_MoneyUtils.GetCash(player);
		return totalCurrency >= m_Quantity * quantity;
	}
	
	override bool CollectPayment(IEntity player, int quantity = 1)
	{
		if (!Replication.IsServer() || !CheckPayment(player)) 
			return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		int transactionAmount = m_Quantity * quantity;
		if (transactionAmount == 0)
			return true;
		
		int currencyRemoved = EL_MoneyUtils.RemoveCash(player, transactionAmount);
		return currencyRemoved > 0;
	}
	
	override bool DistributePayment(IEntity player, int quantity = 1)
	{
		if (!Replication.IsServer()) 
			return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) 
			return false;
		
		int transactionAmount = m_Quantity * quantity;
		if (transactionAmount == 0)
			return true;
		
		int currencyAdded = EL_MoneyUtils.AddCash(player, transactionAmount);
		return currencyAdded > 0;
	}
	
	override string GetDisplayName(int quantity = 1)
	{
		int transactionAmount = m_Quantity * quantity;
		
		if (transactionAmount == 0)
			return "Free";
		
		return string.Format("$%1", Math.AbsInt(transactionAmount));
	}
	
//	override ResourceName GetDisplayEntity()
//	{
//		return PREFAB_CASH;
//	}
	
	override bool Equals(ADM_PaymentMethodBase other)
	{
		ADM_PaymentMethodElCash otherCurrency = ADM_PaymentMethodElCash.Cast(other);
		if (!otherCurrency)
			return false;
		
		return true;
	}
	
	override bool Add(ADM_PaymentMethodBase other, int quantity = 1)
	{
		if (!this.Equals(other))
			return false;
		
		ADM_PaymentMethodElCash otherCurrency = ADM_PaymentMethodElCash.Cast(other);
		if (!otherCurrency)
			return false;
				
		this.m_Quantity += otherCurrency.m_Quantity * quantity;
		
		return true;
	}
}