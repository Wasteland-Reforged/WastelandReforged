modded class ADM_ShopMerchandise: ScriptAndConfig
{
	override array<ref ADM_PaymentMethodBase> GetSellPayment()
	{
		float sellPriceCoef = 0.5;
		
		array<ref ADM_PaymentMethodElCash> sellPayments = {};
		
		foreach (ADM_PaymentMethodBase buyPaymentMethod : GetBuyPayment())
		{
			ADM_PaymentMethodElCash elCashMethod = ADM_PaymentMethodElCash.Cast(buyPaymentMethod);
			if (!elCashMethod)
				continue;
			
			ADM_PaymentMethodElCash sellPaymentMethod = new ADM_PaymentMethodElCash();
			
			int sellPrice = elCashMethod.GetQuantity() * sellPriceCoef;
			sellPaymentMethod.SetQuantity(sellPrice);
			
			sellPayments.Insert(sellPaymentMethod);
		}
		
		return sellPayments;
	}
}