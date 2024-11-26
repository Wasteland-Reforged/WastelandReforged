
modded class ADM_CurrencyComponent
{
	[Attribute(uiwidget: UIWidgets.EditBox, params: "0 inf"), RplProp()]
	protected int m_Value;
	
	override static int FindTotalCurrencyInInventory(SCR_InventoryStorageManagerComponent inventoryManager)
	{
		if (!inventoryManager) 
			return 0;
		
		return EL_MoneyUtils.GetCash(inventoryManager);
	}

}