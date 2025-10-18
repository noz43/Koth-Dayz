class KOTH_LootItem
{
	string name;
	int quantity;

	ref array<ref KOTH_LootItem> attachments = new array<ref KOTH_LootItem>;
	ref array<ref KOTH_LootItem> cargo = new array<ref KOTH_LootItem>;

	void KOTH_LootItem(string className, int amount = -1)
	{
		name = className;
		quantity = amount;
	}

	KOTH_LootItem AddAttachment(string className, int amount = -1)
	{
		KOTH_LootItem item = new KOTH_LootItem(className, amount);
		attachments.Insert(item);

		return item;
	}

	KOTH_LootItem AddCargo(string className, int amount = -1)
	{
		KOTH_LootItem item = new KOTH_LootItem(className, amount);
		cargo.Insert(item);

		return item;
	}
	
	array<ref KOTH_LootItem> GetAttachments()
	{
		return attachments;
	}
	
	array<ref KOTH_LootItem> GetCargo()
	{
		return cargo;
	}

	string GetName()
	{
		return name;
	}
	
	int GetQuantity()
	{
		return quantity;
	}

	void SetQuantity(int amount)
	{
		quantity = amount;
	}
}