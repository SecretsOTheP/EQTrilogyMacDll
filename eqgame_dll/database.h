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
#ifndef EQEMU_DATABASE_H
#define EQEMU_DATABASE_H

#define AUTHENTICATION_TIMEOUT	60
#define INVALID_ID				0xFFFFFFFF

#include "global_define.h"
#include "eqemu_logsys.h"
#include "types.h"
#include "dbcore.h"
#include "linked_list.h"
#include "eq_packet_structs.h"

#include <cmath>
#include <string>
#include <vector>
#include <map>

//atoi is not uint32 or uint32 safe!!!!
#define atoul(str) strtoul(str, nullptr, 10)

class Inventory;
class MySQLRequestResult;
class Client;

struct EventLogDetails_Struct {
	uint32	id;
	char	accountname[64];
	uint32	account_id;
	int16	status;
	char	charactername[64];
	char	targetname[64];
	char	timestamp[64];
	char	descriptiontype[64];
	char	details[128];
};

struct CharacterEventLog_Struct {
	uint32	count;
	uint8	eventid;
	EventLogDetails_Struct eld[255];
};

struct npcDecayTimes_Struct {
	uint16 minlvl;
	uint16 maxlvl;
	uint32 seconds;
};


struct VarCache_Struct {
	char varname[26];	
	char value[0];
};

class PTimerList;

#pragma pack(1)

/* Conversion Structs */

namespace Convert {
	struct BindStruct {
		/*000*/ uint32 zoneId;
		/*004*/ float x;
		/*008*/ float y;
		/*012*/ float z;
		/*016*/ float heading;
	};
	struct Color_Struct
	{
		union
		{
			struct
			{
				uint8	blue;
				uint8	green;
				uint8	red;
				uint8	use_tint;	// if there's a tint this is FF
			} rgb;
			uint32 color;
		};
	};
	struct AA_Array
	{
		uint32 AA;
		uint32 value;
	};
	struct SpellBuff_Struct
	{
		/*000*/	uint8	slotid;		//badly named... seems to be 2 for a real buff, 0 otherwise
		/*001*/ uint8	level;
		/*002*/	uint8	bard_modifier;
		/*003*/	uint8	effect;			//not real
		/*004*/	uint32	spellid;
		/*008*/ uint32	duration;
		/*012*/	uint32	counters;
		/*016*/	uint32	player_id;	//'global' ID of the caster, for wearoff messages
		/*020*/
	};

	struct SuspendedMinion_Struct
	{
		/*000*/	uint16 SpellID;
		/*002*/	uint32 HP;
		/*006*/	uint32 Mana;
		/*010*/	Convert::SpellBuff_Struct Buffs[BUFF_COUNT];
		/*510*/	uint32 Items[_MaterialCount];
		/*546*/	char Name[64];
		/*610*/
	};
	

	namespace player_lootitem_temp
	{
		struct ServerLootItem_Struct_temp {
			uint32	item_id;
			int16	equipSlot;
			uint8	charges;
			uint16	lootslot;
		};
	}

	struct DBPlayerCorpse_Struct_temp {
		uint32	crc;
		bool	locked;
		uint32	itemcount;
		uint32	exp;
		float	size;
		uint8	level;
		uint8	race;
		uint8	gender;
		uint8	class_;
		uint8	deity;
		uint8	texture;
		uint8	helmtexture;
		uint32	copper;
		uint32	silver;
		uint32	gold;
		uint32	plat;
		Color_Struct item_tint[9];
		uint8 haircolor;
		uint8 beardcolor;
		uint8 eyecolor1;
		uint8 eyecolor2;
		uint8 hairstyle;
		uint8 face;
		uint8 beard;
		player_lootitem_temp::ServerLootItem_Struct_temp	items[0];
	};

	namespace classic_db_temp {
		struct DBPlayerCorpse_Struct_temp {
			uint32	crc;
			bool	locked;
			uint32	itemcount;
			uint32	exp;
			float	size;
			uint8	level;
			uint8	race;
			uint8	gender;
			uint8	class_;
			uint8	deity;
			uint8	texture;
			uint8	helmtexture;
			uint32	copper;
			uint32	silver;
			uint32	gold;
			uint32	plat;
			Color_Struct item_tint[9];
			uint8 haircolor;
			uint8 beardcolor;
			uint8 eyecolor1;
			uint8 eyecolor2;
			uint8 hairstyle;
			uint8 face;
			uint8 beard;
			player_lootitem_temp::ServerLootItem_Struct_temp	items[0];
		};
	}
}

#pragma pack()

#endif
