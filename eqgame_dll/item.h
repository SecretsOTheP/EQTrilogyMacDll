/*	EQEMu: Everquest Server Emulator
	Copyright (C) 2001-2003 EQEMu Development Team (http://eqemulator.net)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY except by those people which sell it, which
	are required to give you total support for your newly bought product;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

// @merth notes:
// These classes could be optimized with database reads/writes by storing
// a status flag indicating how object needs to interact with database

#ifndef __ITEM_H
#define __ITEM_H

class ItemInst;				// Item belonging to a client (contains info on item, dye, augments, charges, etc)
class ItemInstQueue;		// Queue of ItemInst objects (i.e., cursor)
class Inventory;			// Character inventory
class ItemParse;			// Parses item packets
class EvolveInfo;			// Stores information about an evolving item family

#include <string>
#include <vector>
#include <map>
#include <list>
#include "eq_packet_structs.h"
#include "eq_constants.h"
#include "item_struct.h"
#include "timer.h"

namespace ItemField
{
	enum
	{
		source = 0,
#define F(x) x,
#include "item_fieldlist.h"
#undef F
		updated
	};
};

// Indexing positions to the beginning slot_id's for a bucket of slots
#define IDX_CURSOR_BAG	330
#define IDX_INV			22
#define IDX_INV_BAG		250
#define IDX_BANK		2000
#define IDX_BANK_BAG	2030
#define IDX_TRADE		3000
#define IDX_TRADE_BAG	3030
#define IDX_TRADESKILL	4000
#define MAX_ITEMS_PER_BAG 10

// Specifies usage type for item inside ItemInst
enum ItemInstTypes
{
	ItemInstNormal = 0,
	ItemInstWorldContainer
};

typedef enum {
	byFlagIgnore,	//do not consider this flag
	byFlagSet,		//apply action if the flag is set
	byFlagNotSet	//apply action if the flag is NOT set
} byFlagSetting;


//FatherNitwit: location bits for searching specific
//places with HasItem() and HasItemByUse()
enum {
	invWhereWorn		= 0x01,
	invWherePersonal	= 0x02,	//in the character's inventory
	invWhereBank		= 0x04,
	invWhereUnused	    = 0x08,
	invWhereTrading		= 0x10,
	invWhereCursor		= 0x20
};


// ########################################
// Class: Queue
//	Queue that allows a read-only iterator
class ItemInstQueue
{
public:
	~ItemInstQueue();
	/////////////////////////
	// Public Methods
	/////////////////////////

	inline std::list<ItemInst*>::const_iterator cbegin() { return m_list.cbegin(); }
	inline std::list<ItemInst*>::const_iterator cend() { return m_list.cend(); }

	inline int size() { return static_cast<int>(m_list.size()); } // TODO: change to size_t
	inline bool empty() { return m_list.empty(); }

	void push(ItemInst* inst);
	void push_front(ItemInst* inst);
	ItemInst* pop();
	ItemInst* peek_front() const;

protected:
	/////////////////////////
	// Protected Members
	/////////////////////////

	std::list<ItemInst*> m_list;

};

// ########################################
// Class: Inventory
//	Character inventory
class Inventory
{
	friend class ItemInst;
public:
	///////////////////////////////
	// Public Methods
	///////////////////////////////

	Inventory() { m_version = EQClientUnknown; m_versionset = false; }
	~Inventory();

	// Inventory v2 creep
	bool SetInventoryVersion(EQClientVersion version) {
		if (!m_versionset) {
			m_version = version;
			m_versionset = true;
			return (m_versionset);
		}
		else {
			return false;
		}
	}

	EQClientVersion GetInventoryVersion() { return m_version; }

	static void CleanDirty();
	static void MarkDirty(ItemInst *inst);

	// Retrieve a writable item at specified slot
	ItemInst* GetItem(int16 slot_id) const;
	ItemInst* GetItem(int16 slot_id, uint8 bagidx) const;

	inline std::list<ItemInst*>::const_iterator cursor_cbegin() { return m_cursor.cbegin(); }
	inline std::list<ItemInst*>::const_iterator cursor_cend() { return m_cursor.cend(); }

	inline int CursorSize() { return m_cursor.size(); }
	inline bool CursorEmpty()			{ return (m_cursor.size() == 0); }

	// Retrieve a read-only item from inventory
	inline const ItemInst* operator[](int16 slot_id) const { return GetItem(slot_id); }

	// Add item to inventory
	int16 PutItem(int16 slot_id, const ItemInst& inst);
	int16 RefPutItem(int16 slot_id, ItemInst* inst);

	// Add item to cursor queue
	int16 PushCursor(const ItemInst& inst);

	// Swap items in inventory
	bool SwapItem(int16 slot_a, int16 slot_b);

	// Remove item from inventory
	bool DeleteItem(int16 slot_id, uint8 quantity=0);

	// Checks All items in a bag for No Drop
	bool CheckNoDrop(int16 slot_id);

	// Remove item from inventory (and take control of memory)
	ItemInst* PopItem(int16 slot_id);

	// Check whether there is space for the specified number of the specified item.
	bool HasSpaceForItem(const Item_Struct *ItemToTry, int16 Quantity);

	// Check whether item exists in inventory
	// where argument specifies OR'd list of invWhere constants to look
	int16 HasItem(int16 item_id, uint8 quantity = 0, uint8 where = 0xFF);
	int16 HasArtifactItem();

	// Check whether item exists in inventory
	// where argument specifies OR'd list of invWhere constants to look
	int16 HasItemByUse(uint8 use, uint8 quantity=0, uint8 where=0xFF);

	// Locate an available inventory slot
	int16 FindFreeSlot(bool for_bag, bool try_cursor, uint8 min_size = 0, bool is_arrow = false);
	int16 FindFreeSlotForTradeItem(const ItemInst* inst);

	// Calculate slot_id for an item within a bag
	static int16 CalcSlotId(int16 slot_id); // Calc parent bag's slot_id
	static int16 CalcSlotId(int16 bagslot_id, uint8 bagidx); // Calc slot_id for item inside bag
	static uint8 CalcBagIdx(int16 slot_id); // Calc bagidx for slot_id
	static int16 CalcSlotFromMaterial(uint8 material);
	static uint8 CalcMaterialFromSlot(int16 equipslot);

	static bool CanItemFitInContainer(const Item_Struct *ItemToTry, const Item_Struct *Container);

	//  Test for valid inventory casting slot
	bool SupportsClickCasting(int16 slot_id);

	// Test whether a given slot can support a container item
	static bool SupportsContainers(int16 slot_id);

	int GetSlotByItemInst(ItemInst *inst);

	uint8 FindBrightestLightType();

	void dumpEntireInventory();
	void dumpWornItems();
	void dumpInventory();
	void dumpBankItems();

	void SetCustomItemData(uint32 character_id, int16 slot_id, std::string identifier, std::string value);
	void SetCustomItemData(uint32 character_id, int16 slot_id, std::string identifier, int value);
	void SetCustomItemData(uint32 character_id, int16 slot_id, std::string identifier, float value);
	void SetCustomItemData(uint32 character_id, int16 slot_id, std::string identifier, bool value);
	std::string GetCustomItemData(int16 slot_id, std::string identifier);
protected:
	///////////////////////////////
	// Protected Methods
	///////////////////////////////

	int GetSlotByItemInstCollection(const std::map<int16, ItemInst*> &collection, ItemInst *inst);
	void dumpItemCollection(const std::map<int16, ItemInst*> &collection);
	void dumpBagContents(ItemInst *inst, std::map<int16, ItemInst*>::const_iterator *it);

	// Retrieves item within an inventory bucket
	ItemInst* _GetItem(const std::map<int16, ItemInst*>& bucket, int16 slot_id) const;

	// Private "put" item into bucket, without regard for what is currently in bucket
	int16 _PutItem(int16 slot_id, ItemInst* inst);

	// Checks an inventory bucket for a particular item
	int16 _HasItem(std::map<int16, ItemInst*>& bucket, int16 item_id, uint8 quantity);
	int16 _HasItem(ItemInstQueue& iqueue, int16 item_id, uint8 quantity);
	int16 _HasItemByUse(std::map<int16, ItemInst*>& bucket, uint8 use, uint8 quantity);
	int16 _HasItemByUse(ItemInstQueue& iqueue, uint8 use, uint8 quantity);


	// Player inventory
	std::map<int16, ItemInst*>	m_worn;		// Items worn by character
	std::map<int16, ItemInst*>	m_inv;		// Items in character personal inventory
	std::map<int16, ItemInst*>	m_bank;		// Items in character bank
	std::map<int16, ItemInst*>	m_trade;	// Items in a trade session
	ItemInstQueue				m_cursor;	// Items on cursor: FIFO

private:
	// Active inventory version
	EQClientVersion m_version;
	bool m_versionset;
};

class SharedDatabase;

// ########################################
// Class: ItemInst
//	Base class for an instance of an item
//	An item instance encapsulates item data + data specific
//	to an item instance (includes dye, augments, charges, etc)
class ItemInst
{
public:
	/////////////////////////
	// Methods
	/////////////////////////

	// Constructors/Destructor
	ItemInst(const Item_Struct* item = nullptr, int8 charges = 0);

	ItemInst(SharedDatabase *db, int16 item_id, int8 charges = 0);

	ItemInst(ItemInstTypes use_type);

	ItemInst(const ItemInst& copy);

	~ItemInst();

	// Query item type
	bool IsType(ItemClassTypes item_class) const;

	// Can item be stacked?
	bool IsStackable() const;

	// Can item be equipped by/at?
	bool IsEquipable(uint16 race, uint16 class_) const;
	bool IsEquipable(int16 slot_id) const;

	//
	// Contents
	//
	ItemInst* GetItem(uint8 slot) const;
	int16 GetItemID(uint8 slot) const;
	inline const ItemInst* operator[](uint8 slot) const { return GetItem(slot); }
	void PutItem(uint8 slot, const ItemInst& inst);
	void PutItem(SharedDatabase *db, uint8 slot, int16 item_id) { return; } // not defined anywhere...
	void DeleteItem(uint8 slot);
	ItemInst* PopItem(uint8 index);
	void Clear();
	void ClearByFlags(byFlagSetting is_nodrop, byFlagSetting is_norent);
	uint8 FirstOpenSlot() const;
	uint8 GetTotalItemCount() const;
	bool IsNoneEmptyContainer();
	std::map<uint8, ItemInst*>* GetContents() { return &m_contents; }

	// Has attack/delay?
	bool IsWeapon() const;
	bool IsAmmo() const;

	// Accessors
	const int16 GetID() const { return ((m_item) ? m_item->ID : NO_ITEM); }
	const Item_Struct* GetItem() const;
	const Item_Struct* GetUnscaledItem() const;

	int8 GetCharges() const				{ return m_charges; }
	void SetCharges(int8 charges)			{ m_charges = charges; }

	uint32 GetPrice() const					{ return m_price; }
	void SetPrice(uint32 price)				{ m_price = price; }

	void SetColor(uint32 color)				{ m_color = color; }
	uint32 GetColor() const					{ return m_color; }

	uint32 GetMerchantSlot() const				{ return m_merchantslot; }
	void SetMerchantSlot(uint32 slot)		{ m_merchantslot = slot; }

	int32 GetMerchantCount() const				{ return m_merchantcount; }
	void SetMerchantCount(int32 count)		{ m_merchantcount = count; }

	int16 GetCurrentSlot() const			{ return m_currentslot; }
	void SetCurrentSlot(int16 curr_slot)	{ m_currentslot = curr_slot; }

	// Is this item already attuned?
	bool IsInstNoDrop() const { return m_instnodrop; }
	void SetInstNoDrop(bool flag) { m_instnodrop=flag; }

	std::string GetCustomDataString() const;
	std::string GetCustomData(std::string identifier);
	void SetCustomData(std::string identifier, std::string value);
	void SetCustomData(std::string identifier, int value);
	void SetCustomData(std::string identifier, float value);
	void SetCustomData(std::string identifier, bool value);
	void DeleteCustomData(std::string identifier);

	// Allows treatment of this object as though it were a pointer to m_item
	operator bool() const { return (m_item != nullptr); }

	// Compare inner Item_Struct of two ItemInst objects
	bool operator==(const ItemInst& right) const { return (this->m_item == right.m_item); }
	bool operator!=(const ItemInst& right) const { return (this->m_item != right.m_item); }

	// Clone current item
	ItemInst* Clone() const;

	bool IsSlotAllowed(int16 slot_id) const;

	bool IsScaling() const				{ return m_scaling; }
	bool IsEvolving() const				{ return (m_evolveLvl >= 1); }
	uint32 GetExp() const				{ return m_exp; }
	void SetExp(uint32 exp)				{ m_exp = exp; }
	void AddExp(uint32 exp)				{ m_exp += exp; }
	bool IsActivated()					{ return m_activated; }
	void SetActivated(bool activated)	{ m_activated = activated; }
	int8 GetEvolveLvl() const			{ return m_evolveLvl; }
	void SetScaling(bool v)				{ m_scaling = v; }

	void Initialize(SharedDatabase *db = nullptr);
	void ScaleItem();
	bool EvolveOnAllKills() const;
	int8 GetMaxEvolveLvl() const;
	uint32 GetKillsNeeded(uint8 currentlevel);

	std::string Serialize(int16 slot_id) const { InternalSerializedItem_Struct s; s.slot_id=slot_id; s.inst=(const void *)this; std::string ser; ser.assign((char *)&s,sizeof(InternalSerializedItem_Struct)); return ser; }
	inline int32 GetSerialNumber() const { return m_SerialNumber; }
	inline void SetSerialNumber(int32 id) { m_SerialNumber = id; }

	std::map<std::string, Timer>& GetTimers() { return m_timers; }
	void SetTimer(std::string name, uint32 time);
	void StopTimer(std::string name);
	void ClearTimers();

protected:
	//////////////////////////
	// Protected Members
	//////////////////////////
	std::map<uint8, ItemInst*>::const_iterator _cbegin() { return m_contents.cbegin(); }
	std::map<uint8, ItemInst*>::const_iterator _cend() { return m_contents.cend(); }

	friend class Inventory;


	void _PutItem(uint8 index, ItemInst* inst) { m_contents[index] = inst; }

	ItemInstTypes		m_use_type;	// Usage type for item
	const Item_Struct*	m_item;		// Ptr to item data
	int8				m_charges;	// # of charges for chargeable items
	uint32				m_price;	// Bazaar /trader price
	uint32				m_color;
	uint32				m_merchantslot;
	int16				m_currentslot;
	bool				m_instnodrop;
	int32				m_merchantcount;		//number avaliable on the merchant, -1=unlimited
	int32				m_SerialNumber;	// Unique identifier for this instance of an item. Needed for Bazaar.
	uint32				m_exp;
	int8				m_evolveLvl;
	bool				m_activated;
	Item_Struct*		m_scaledItem;
	EvolveInfo*			m_evolveInfo;
	bool				m_scaling;

	//
	// Items inside of this item (augs or contents);
	std::map<uint8, ItemInst*>			m_contents; // Zero-based index: min=0, max=9
	std::map<std::string, std::string>	m_custom_data;
	std::map<std::string, Timer>		m_timers;
};

class EvolveInfo {
public:
	friend class ItemInst;
	//temporary
	uint16				LvlKills[9];
	uint32				FirstItem;
	uint8				MaxLvl;
	bool				AllKills;

	EvolveInfo();
	EvolveInfo(uint32 first, uint8 max, bool allkills, uint32 L2, uint32 L3, uint32 L4, uint32 L5, uint32 L6, uint32 L7, uint32 L8, uint32 L9, uint32 L10);
	~EvolveInfo();
};

struct LightProfile_Struct
{
	/*
	Current criteria (light types):
	Equipment:	{ 0 .. 15 }
	General:	{ 9 .. 13 }

	Notes:
	- Initial character load and item movement updates use different light source update behaviors
	-- Server procedure matches the item movement behavior since most updates occur post-character load
	- MainAmmo is not considered when determining light sources
	- No 'Sub' or 'Aug' items are recognized as light sources
	- Light types '< 9' and '> 13' are not considered for general (carried) light sources
	- If values > 0x0F are valid, then assignment limiters will need to be removed
	- MainCursor 'appears' to be a valid light source update slot..but, have not experienced updates during debug sessions
	- All clients have a bug regarding stackable items (light and sound updates are not processed when picking up an item)
	-- The timer-based update cancels out the invalid light source
	*/

	static uint8 TypeToLevel(uint8 lightType);
	static bool IsLevelGreater(uint8 leftType, uint8 rightType);

	// Light types (classifications)
	struct {
		uint8 Innate;		// Defined by db field `npc_types`.`light` - where appropriate
		uint8 Equipment;	// Item_Struct::light value of worn/carried equipment
		uint8 Spell;		// Set value of any light-producing spell (can be modded to mimic equip_light behavior)
		uint8 Active;		// Highest value of all light sources
	} Type;

	// Light levels (intensities) - used to determine which light source should be active
	struct {
		uint8 Innate;
		uint8 Equipment;
		uint8 Spell;
		uint8 Active;
	} Level;
};

#endif // #define __ITEM_H
