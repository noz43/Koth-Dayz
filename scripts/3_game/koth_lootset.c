class KOTH_LootSet
{
	string name;
	ref array<ref KOTH_LootItem> items = new array<ref KOTH_LootItem>;

	void KOTH_LootSet(string setName)
	{
		name = setName;
	}

	string GetName()
	{
		return name;
	}
	
	array<ref KOTH_LootItem> GetItems()
	{
		return items;
	}

	KOTH_LootItem RegisterItem(string className, int amount = -1)
	{
		KOTH_LootItem item = new KOTH_LootItem(className, amount);
		items.Insert(item);

		return item;
	}
}