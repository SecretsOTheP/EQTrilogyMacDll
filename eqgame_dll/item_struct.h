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
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 04111-1307 USA
*/

#ifndef ITEM_STRUCT_H
#define ITEM_STRUCT_H

/*
 * Note: (Doodman)
 *	This structure has field names that match the DB name exactly.
 *	Please take care as to not mess this up as it should make
 *	everyones life (i.e. mine) much easier. And the DB names
 *	match the field name from the 13th floor (SEQ) item collectors,
 *	so please maintain that as well.
 *
 * Note #2: (Doodman)
 *	UnkXXX fields are left in here for completeness but commented
 *	out since they are really unknown and since the items are now
 *	preserialized they should not be needed. Conversly if they
 *	-are- needed, then they shouldn't be unkown.
 *
 * Note #3: (Doodman)
 *	Please take care when adding new found data fields to add them
 *	to the appropriate structure. Item_Struct has elements that are
 *	global to all types of items only.
 *
 * Note #4: (Doodman)
 *	Made ya look! Ha!
 */

//#include "eq_constants.h"
#include "eq_dictionary.h"

/*
** Child struct of Item_Struct:
**	Effect data: Click, Proc, Focus, Worn, Scroll
**
*/
struct ItemEffect_Struct {
	int16	Effect;
	uint8	Type;
	uint8	Level;
	uint8	Level2;
	//MaxCharges
	//CastTime
	//RecastDelay
	//RecastType
	//ProcRate
};

class ItemInst;

struct InternalSerializedItem_Struct {
	int16 slot_id;
	const void * inst;
};

// use EmuConstants::ITEM_COMMON_SIZE
//#define MAX_AUGMENT_SLOTS 5

#endif
