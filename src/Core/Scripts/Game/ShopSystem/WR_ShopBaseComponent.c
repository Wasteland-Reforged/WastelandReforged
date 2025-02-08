modded class ADM_ShopBaseComponent : ScriptComponent
{
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
//		super.super.OnPostInit(owner);
//		SetEventMask(owner, EntityEvent.INIT);
//		owner.SetFlags(EntityFlags.ACTIVE, true);
//		
//		//LoadMerchandise();
//		
//		RplComponent rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
//		if (rpl) 
//			rpl.InsertToReplication();
	}
	
	void LoadMerchandise()
	{
		if (!m_Merchandise) 
			m_Merchandise = new array<ref ADM_ShopMerchandise>();
			
		// Copy config into merchandise array
		if (m_ShopConfig != string.Empty) 
		{
			ADM_ShopConfig shopConfig = ADM_ShopConfig.GetConfig(m_ShopConfig);
			if (shopConfig && shopConfig.m_Merchandise) 
			{
				foreach (ADM_ShopMerchandise merch : shopConfig.m_Merchandise) 
				{
					m_Merchandise.Insert(merch);
				}
			}
		}
	}	
}