class WR_WeightedItem<Class T>
{
    float m_fWeight;
	T m_tItem;

    void WR_WeightedItem(float weight, T item)
    {
		m_fWeight = weight;
		m_tItem = item;
    }
}

class WR_WeightedItemArray<Class T>
{
	private ref array<ref WR_WeightedItem<T>> m_items;
	private float m_totalItemWeight = 0;
	
	void AddItem(float weight, T item)
	{
		if (!m_items) m_items = {};
		
		WR_WeightedItem<T> weightedItem = new WR_WeightedItem<T>(weight, item);
		
		m_items.Insert(weightedItem);
		m_totalItemWeight += weightedItem.m_fWeight;
	}
	
	void AddItemsDistributedWeight(float totalWeight, notnull array<T> items)
	{
		int totalItems = items;
		float itemWeight = totalWeight / totalItems;
		
		foreach(T item : items)
			AddItem(itemWeight, item);
	}
	
	WR_WeightedItem<T> GetRandomWeightedItem()
    {
        float randomValue = Math.RandomFloat(0, m_totalItemWeight);
		T selectedItem;
		
        float cumulativeWeight = 0;
        foreach (auto weightedItem : m_items)
        {
            cumulativeWeight += weightedItem.m_fWeight;
            if (randomValue < cumulativeWeight)
            {
                return weightedItem;
            }
        }

        return null; // In case something goes wrong
    }
	
	T GetRandomItem()
    {
		WR_WeightedItem<T> weightedItem = GetRandomWeightedItem();
		
		// TODO: handle this potentially null value!
		// if (!weightedItem) return null; // Can't return null for type T. Need to find the right way to handle this

		return weightedItem.m_tItem;
    }
	
	int Count()
	{
		if (!m_items) return 0;
		
		return m_items.Count();
	}
	
	private void RecalculateTotalWeight()
	{
		float m_totalItemWeight = 0;
		
		foreach (auto item : m_items)
			m_totalItemWeight += item.m_fWeight;
	}
}