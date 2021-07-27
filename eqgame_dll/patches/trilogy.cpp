#include "../global_define.h"
#include "../eqemu_logsys.h"
#include "trilogy.h"
#include "../opcodemgr.h"
#include "../eq_stream_ident.h"
#include "../crc32.h"

#include "../eq_packet_structs.h"
#include "../packet_dump_file.h"
#include "../misc_functions.h"
#include "../packet_functions.h"
#include "../string_util.h"
#include "../item.h"
#include "trilogy_structs.h"

#include "../zone_numbers.h"


typedef DWORD(__stdcall* CEverQuest__HandleWorldMessage_t)(DWORD *, unsigned __int32, char *, unsigned __int32);
extern CEverQuest__HandleWorldMessage_t CEverQuest__HandleWorldMessage_Trampoline;

extern DWORD* conn_ptr;
namespace Trilogy {
	static const char *name = "Trilogy";
	static OpcodeManager *opcodes = nullptr;
	static Strategy struct_strategy;

	void Register(EQStreamIdentifier &into)
	{
		//create our opcode manager if we havent already
		if (opcodes == nullptr)
		{
			std::string opfile = "patch_";
			opfile += name;
			opfile += ".conf";
			//load up the opcode manager.
			//TODO: figure out how to support shared memory with multiple patches...
			opcodes = new RegularOpcodeManager();
			Log.Out(Logs::General, Logs::World_Server, "[OPCODES] Trilogy Register starting... %s | %s", opfile.c_str(), name);
			if (!opcodes->LoadOpcodes(opfile.c_str()))
			{
				Log.Out(Logs::General, Logs::World_Server, "[OPCODES] Error loading opcodes file %s. Not registering patch %s.", opfile.c_str(), name);
				return;
			}
		}

		//ok, now we have what we need to register.

		EQStream::Signature signature;
		std::string pname;

		pname = std::string(name) + "_world";
		//register our world signature.
		signature.first_length = sizeof(structs::LoginInfo_Struct);
		signature.first_eq_opcode = opcodes->EmuToEQ(OP_SendLoginInfo);
		into.RegisterOldPatch(signature, pname.c_str(), opcodes, &struct_strategy);

		pname = std::string(name) + "_zone";
		//register our zone signature.
		signature.first_length = sizeof(structs::SetDataRate_Struct);
		signature.first_eq_opcode = opcodes->EmuToEQ(OP_DataRate);
		into.RegisterOldPatch(signature, pname.c_str(), opcodes, &struct_strategy);

		Log.Out(Logs::General, Logs::Netcode, "[IDENTIFY] Registered patch %s", name);
	}

	void Reload()
	{

		//we have a big problem to solve here when we switch back to shared memory
		//opcode managers because we need to change the manager pointer, which means
		//we need to go to every stream and replace it's manager.

		if (opcodes != nullptr)
		{
			//TODO: get this file name from the config file
			std::string opfile = "patch_";
			opfile += name;
			opfile += ".conf";
			if (!opcodes->ReloadOpcodes(opfile.c_str()))
			{
				Log.Out(Logs::General, Logs::Netcode, "[OPCODES] Error reloading opcodes file %s for patch %s.", opfile.c_str(), name);
				return;
			}
			Log.Out(Logs::General, Logs::Netcode, "[OPCODES] Reloaded opcodes for patch %s", name);
		}
	}



	Strategy::Strategy()
		: StructStrategy()
	{
		//all opcodes default to passthrough.
#include "ss_register.h"
#include "trilogy_ops.h"
	}

	std::string Strategy::Describe() const
	{
		std::string r;
		r += "Patch ";
		r += name;
		return(r);
	}

#include "ss_define.h"

	const EQClientVersion Strategy::ClientVersion() const
	{
		return EQClientTrilogy;
	}
	ENCODE(OP_SendLoginInfo)
	{
		ENCODE_LENGTH_EXACT(structs::LoginInfo_Struct);
		SETUP_DIRECT_ENCODE(structs::LoginInfo_Struct, LoginInfo_Struct);
		memcpy(eq->AccountName, emu->AccountName, 30);
		memcpy(eq->Password, emu->Password, 30);
		IN(zoning);
		FINISH_ENCODE();
	}

	ENCODE(OP_EnterWorld)
	{
		SETUP_DIRECT_ENCODE(structs::EnterWorld_Struct, EnterWorld_Struct);
		strn0cpy(eq->charname, emu->charname, 30);
		FINISH_ENCODE();
	}

	/*

	ENCODE(OP_ZoneServerInfo)
	{
		SETUP_DIRECT_ENCODE(ZoneServerInfo_Struct, structs::ZoneServerInfo_Struct);
		strcpy(eq->ip, emu->ip);
		eq->port = ntohs(emu->port);

		FINISH_ENCODE();
	}
	ENCODE(OP_LogServer)
	{
		ENCODE_LENGTH_EXACT(LogServer_Struct);
		SETUP_DIRECT_ENCODE(LogServer_Struct, structs::LogServer_Struct);
		FINISH_ENCODE();
	}
	*/

	DECODE(OP_ApproveWorld)
	{
		SETUP_DIRECT_DECODE(structs::ApproveWorld_Struct, ApproveWorld_Struct);
		eq->response = 0;
		FINISH_DIRECT_DECODE();
	}

	DECODE(OP_EnterWorld)
	{
		SETUP_DIRECT_DECODE(structs::ApproveWorld_Struct, ApproveWorld_Struct);
		eq->response = 0;
		FINISH_DIRECT_DECODE();
	}
	DECODE(OP_ExpansionInfo)
	{
		SETUP_DIRECT_DECODE(structs::ExpansionInfo_Struct, ExpansionInfo_Struct);
		if (emu->Expansions > 3)
			eq->Expansions = 3;
		else
			OUT(Expansions);
		FINISH_DIRECT_DECODE();
	}
	DECODE(OP_SendCharInfo)
	{
		int r;
		DECODE_LENGTH_EXACT(CharacterSelect_Struct);
		SETUP_DIRECT_DECODE(structs::CharacterSelect_Struct, CharacterSelect_Struct);
		for (r = 0; r < 10; r++)
		{
			strncpy(emu->zone[r], StaticGetZoneName(eq->zone[r]), 20);
			IN(primary[r]);
			if (eq->race[r] > 300)
				emu->race[r] = 1;
			else
				emu->race[r] = eq->race[r];
			IN(class_[r]);
			strncpy(emu->name[r], eq->name[r], 30);
			IN(gender[r]);
			IN(level[r]);
			IN(secondary[r]);
			IN(face[r]);
			int k;
			for (k = 0; k < 9; k++)
			{
				IN(equip[r][k]);
				IN(cs_colors[r][k].color);
			}
			IN(deity[r]);
		}
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_ZoneEntry)
	{
		SETUP_DIRECT_ENCODE(structs::ClientZoneEntry_Struct, ClientZoneEntry_Struct)
		strn0cpy(eq->char_name, emu->char_name, 32);
		Log.Out(Logs::General, Logs::Netcode, "[STRUCTS] CZE is %i bytes outgoing", __packet->size);
		FINISH_ENCODE();
	}

	DECODE(OP_ZoneEntry)
	{
		SETUP_DIRECT_DECODE(structs::ServerZoneEntry_Struct, ServerZoneEntry_Struct);
		memset(emu, 0, sizeof(structs::ServerZoneEntry_Struct));
		
		int k = 0;
		strncpy(emu->zone, StaticGetZoneName(eq->zoneID), 15);
		IN(anon);
		strncpy(emu->name, eq->name, 30);
		emu->deity = eq->deity;
		emu->race = eq->race;
		IN(size);
		IN(NPC);

		IN(invis);
		IN(max_hp);
		IN(curHP);
		IN(x_pos);
		IN(y_pos);
		IN(z_pos);
		IN(heading);
		IN(face);
		IN(level);
		IN(prev);
		IN(next);
		IN(corpse);
		IN(LocalInfo);
		IN(My_Char);
		IN(view_height);
		IN(sprite_oheight);
		IN(sprite_oheights);
		emu->sze_unknown1 = eq->type;

		for (k = 0; k < 9; k++)
		{
			emu->equipment[k] = (uint8)eq->equipment[k];
			emu->equipcolors[k].color = eq->equipcolors[k].color;
		}
		IN(anim_type);
		IN(bodytexture);
		IN(helm);
		IN(race);
		IN(GM);
		if (eq->GuildID == 0 || eq->GuildID == 0xFFFFFFFF)
			emu->GuildID = 0xFFFF;
		else
			emu->GuildID = eq->GuildID;

		strn0cpy(emu->Surname, eq->Surname, 20);
		IN(walkspeed);
		IN(runspeed);
		IN(light);
		IN(class_);
		IN(gender);
		IN(flymode);
		IN(size);
		IN(petOwnerId);		
		CRC32::SetEQChecksum((uchar*)emu, sizeof(structs::ServerZoneEntry_Struct));

		FINISH_DIRECT_DECODE();
	}

	DECODE(OP_PlayerProfile)
	{
		DecryptProfilePacket(__packet->pBuffer, __packet->size);
		EQApplicationPacket* outapp = new EQApplicationPacket();
		outapp->SetOpcode(OP_PlayerProfile);
		outapp->pBuffer = new uchar[10000];
		outapp->size = InflatePacket((unsigned char*)__packet->pBuffer, sizeof(PlayerProfile_Struct), outapp->pBuffer, 10000);
		safe_delete(__packet);
		__packet = outapp;
		SETUP_DIRECT_DECODE(structs::PlayerProfile_Struct, PlayerProfile_Struct);
		memset(emu, 0, sizeof(structs::PlayerProfile_Struct));
		//Find these:
		//	eq->available_slots=0xffff;
		//eq->bind_point_zone = emu->binds[0].zoneId;
		//eq->bind_location[0].x = emu->binds[0].x;
		//eq->bind_location[0].y = emu->binds[0].y;
		//eq->bind_location[0].z = emu->binds[0].z;
		//OUT(birthday);
		//OUT(lastlogin);
		//OUT(timePlayedMin);
		//OUT_str(boat);
		//OUT(air_remaining);
		//OUT(level2);
		//for(r = 0; r < 9; r++)
		//{
		//OUT(item_material[r]);
		//}

		int r = 0;
		strn0cpy(emu->current_zone, StaticGetZoneName(eq->current_zone), 15);
		IN(gender);
		IN(race);
		IN(class_);
		IN(level);
		IN(deity);
		IN(intoxication);
		IN(points);
		IN(mana);
		IN(cur_hp);
		IN(STR);
		IN(STA);
		IN(CHA);
		IN(DEX);
		IN(INT);
		IN(AGI);
		IN(WIS);
		IN(face);
		for (r = 0; r < 256; r++)
		{
			emu->spell_book[r] = eq->spell_book[r];
			if (eq->spell_book[r] == 0 || eq->spell_book[r] > 2999)
				emu->spell_book[r] = 0xFFFF;
		}
		for (r = 0; r < 8; r++)
		{
			emu->mem_spells[r] = eq->mem_spells[r];
			if (eq->spell_book[r] == 0 || eq->spell_book[r] > 2999)
				emu->spell_book[r] = 0xFFFF;
		}
		IN(platinum);
		IN(gold);
		IN(silver);
		IN(copper);
		IN(platinum_cursor);
		IN(gold_cursor);
		IN(silver_cursor);
		IN(copper_cursor);
		for (r = 0; r < structs::MAX_PP_SKILL; r++)
		{
			emu->skills[r] = eq->skills[r];
		}

		for (r = 0; r < 15; r++)
		{
			if (eq->buffs[r].spellid == 0 || eq->buffs[r].spellid > 2999)
				emu->buffs[r].spellid = 0xFFFF;
			else
				emu->buffs[r].spellid = eq->buffs[r].spellid;
			if (emu->buffs[r].spellid == 0xFFFF)
			{
				emu->buffs[r].visable = 0;
				emu->buffs[r].level = 0;
				emu->buffs[r].bard_modifier = 0;
				emu->buffs[r].duration = 0;
				emu->buffs[r].activated = 0;
			}
			else {
				emu->buffs[r].visable = 2;
				emu->buffs[r].level = eq->buffs[r].level;
				emu->buffs[r].bard_modifier = eq->buffs[r].bard_modifier;
				emu->buffs[r].spellid = eq->buffs[r].spellid;
				emu->buffs[r].duration = eq->buffs[r].duration;
			}
		}
		strn0cpy(emu->name, eq->name, 30);
		strncpy(emu->Surname, eq->Surname, 20);
		if (eq->guild_id == 0 || eq->guild_id == 0xFFFFFFFF)
			emu->guild_id = 0xFFFF;
		else
			emu->guild_id = eq->guild_id;
		IN(pvp);
		IN(anon);
		IN(gm);
		IN(guildrank);
		IN(exp);
		for (r = 0; r < 24; r++)
		{
			emu->languages[r] = eq->languages[r];
		}
		IN(x);
		IN(y);
		IN(z);
		IN(heading);
		IN(platinum_bank);
		IN(gold_bank);
		IN(silver_bank);
		IN(copper_bank);
		IN(autosplit);
		if (eq->expansions > 3)
			emu->expansions = 3;
		else
			IN(expansions);
		for (r = 0; r < 6; r++)
		{
			strn0cpy(emu->groupMembers[r], eq->groupMembers[r], 48);
		}
		IN(abilitySlotRefresh);
		for (r = 0; r < 8; r++)
		{
			emu->spellSlotRefresh[r] = eq->spellSlotRefresh[r];
		}
		emu->eqbackground = 0;

		Log.Out(Logs::General, Logs::Netcode, "[STRUCTS] Player Profile Packet is %i bytes uncompressed", sizeof(structs::PlayerProfile_Struct));

		CRC32::SetEQChecksum(__packet->pBuffer, sizeof(structs::PlayerProfile_Struct));
		EQApplicationPacket* outapp2 = new EQApplicationPacket();
		outapp2->SetOpcode(OP_PlayerProfile);
		outapp2->pBuffer = new uchar[10000];
		outapp2->size = DeflatePacket((unsigned char*)__packet->pBuffer, sizeof(structs::PlayerProfile_Struct), outapp2->pBuffer, 10000);
		EncryptTrilogyProfilePacket(outapp2->pBuffer, outapp2->size);
		Log.Out(Logs::General, Logs::Netcode, "[STRUCTS] Player Profile Packet is %i bytes compressed", outapp2->size);
		safe_delete_array(__eq_buffer);
		safe_delete(outapp);
		return outapp2;
	}

	/*DECODE(OP_CharacterCreate)
	{
		DECODE_LENGTH_EXACT(structs::CharCreate_Struct);
		SETUP_DIRECT_DECODE(CharCreate_Struct, structs::CharCreate_Struct);
		IN(class_);
		IN(gender);
		IN(race);
		strncpy(eq->start_zone, eq->current_zone, 20);
		uint32 deity = (uint8)eq->deity;
		emu->deity = deity;
		IN(STR);
		IN(STA);
		IN(AGI);
		IN(DEX);
		IN(WIS);
		IN(INT);
		IN(CHA);
		IN(face);
		emu->beard = 0;
		emu->beardcolor = 0;
		emu->hairstyle = 0;
		emu->haircolor = 0;
		emu->eyecolor1 = 0;
		emu->eyecolor2 = 0;
		FINISH_DIRECT_DECODE();
	}

	DECODE(OP_SetGuildMOTD)
	{
		SETUP_DIRECT_DECODE(GuildMOTD_Struct, structs::GuildMOTD_Struct);
		strcpy(emu->name, eq->name);
		strcpy(emu->motd, eq->motd);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_GuildMOTD)
	{
		SETUP_DIRECT_ENCODE(GuildMOTD_Struct, structs::GuildMOTD_Struct);
		strcpy(eq->name, emu->name);
		strcpy(eq->motd, emu->motd);
		FINISH_ENCODE();
	}

	DECODE(OP_GuildInvite)
	{
		SETUP_DIRECT_DECODE(GuildCommand_Struct, structs::GuildCommand_Struct);
		strcpy(emu->myname, eq->myname);
		strcpy(emu->othername, eq->othername);
		IN(guildeqid);
		IN(officer);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_GuildInvite)
	{
		SETUP_DIRECT_ENCODE(GuildCommand_Struct, structs::GuildCommand_Struct);
		strn0cpy(eq->myname, emu->myname, 30);
		strn0cpy(eq->othername, emu->othername, 30);
		OUT(guildeqid);
		OUT(officer);
		FINISH_ENCODE();
	}

	DECODE(OP_GuildRemove)
	{
		SETUP_DIRECT_DECODE(GuildCommand_Struct, structs::GuildCommand_Struct);
		strcpy(emu->myname, eq->myname);
		strcpy(emu->othername, eq->othername);
		IN(guildeqid);
		IN(officer);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_GuildRemove)
	{
		SETUP_DIRECT_ENCODE(GuildCommand_Struct, structs::GuildCommand_Struct);
		strn0cpy(eq->myname, emu->myname, 30);
		strn0cpy(eq->othername, emu->othername, 30);
		OUT(guildeqid);
		OUT(officer);
		FINISH_ENCODE();
	}

	DECODE(OP_GuildInviteAccept)
	{
		SETUP_DIRECT_DECODE(GuildInviteAccept_Struct, structs::GuildInviteAccept_Struct);
		strcpy(emu->inviter, eq->inviter);
		strcpy(emu->newmember, eq->newmember);
		IN(response);
		emu->guildeqid = (int16)eq->guildeqid;
		FINISH_DIRECT_DECODE();
	}
	*/
	DECODE(OP_NewZone)
	{
		SETUP_DIRECT_DECODE(structs::NewZone_Struct, NewZone_Struct);
		memset(emu, 0, sizeof(structs::NewZone_Struct));
		strn0cpy(emu->char_name, eq->char_name, 30);
		strn0cpy(emu->zone_short_name, eq->zone_short_name, 20);
		strn0cpy(emu->zone_long_name, eq->zone_long_name, 180);
		IN(ztype);
		int r = 0;
		for (r = 0; r < 4; r++)
		{
			emu->fog_red[r] = eq->fog_red[r];
		}
		for (r = 0; r < 4; r++)
		{
			emu->fog_green[r] = eq->fog_green[r];
		}
		for (r = 0; r < 4; r++)
		{
			emu->fog_blue[r] = eq->fog_blue[r];
		}
		for (r = 0; r < 4; r++)
		{
			emu->fog_minclip[r] = eq->fog_minclip[r];
		}
		for (r = 0; r < 4; r++)
		{
			emu->fog_maxclip[r] = eq->fog_maxclip[r];
		}
		IN(gravity);
		IN(time_type);
		IN(sky);
		IN(safe_y);
		IN(safe_x);
		IN(safe_z);
		IN(max_z);
		emu->underworld = eq->underworld;
		IN(minclip);
		IN(maxclip);
		IN(timezone);
		FINISH_DIRECT_DECODE();
	}
	DECODE(OP_ChannelMessage)
	{
		
		unsigned char *__emu_buffer = __packet->pBuffer;
		ChannelMessage_Struct *emu = (ChannelMessage_Struct *)__emu_buffer;
		uint32 __i = 0;
		__i++;
		int msglen = __packet->size - sizeof(ChannelMessage_Struct);
		int len = sizeof(structs::ChannelMessage_Struct) + msglen;
		EQApplicationPacket* outapp = new EQApplicationPacket(OP_ChannelMessage, len);
		structs::ChannelMessage_Struct *eq = (structs::ChannelMessage_Struct *) outapp->pBuffer;
		strn0cpy(eq->targetname, emu->targetname, 32);
		strn0cpy(eq->sender, emu->sender, 32);
		eq->language = emu->language;
		eq->chan_num = emu->chan_num;
		eq->skill_in_language = emu->skill_in_language;
		strncpy(eq->message, emu->message, msglen);
		return outapp;
	}

	DECODE(OP_FormattedMessage)
	{		
		char *bufptr;
		unsigned char *__emu_buffer = __packet->pBuffer;
		OldFormattedMessage_Struct *emu = (OldFormattedMessage_Struct *)__emu_buffer;
		uint32 __i = 0;
		__i++;

		int msglen = __packet->size - sizeof(OldFormattedMessage_Struct);

		std::string message("");
		if (emu->string_id == 1032 || emu->string_id == 1034) {
			int len = msglen + sizeof(structs::ChannelMessage_Struct);
			EQApplicationPacket* outapp = new EQApplicationPacket(OP_ChannelMessage, len);
			structs::ChannelMessage_Struct *eq = (structs::ChannelMessage_Struct *) outapp->pBuffer;
			memset(outapp->pBuffer, 0, len);
			strn0cpy(eq->message, emu->message, 30);
			bufptr = emu->message + strlen(eq->sender) + 1;
			strn0cpy(eq->message, bufptr, msglen - strlen(eq->sender) - 1);
			if (emu->string_id == 1032)
				eq->chan_num = 8;
			else
				eq->chan_num = 3;
			eq->skill_in_language = 100;
			return outapp;
		}
		else {
			switch (emu->string_id) {
			case 100:
				message = "Your target is out of range, get closer!";
				break;
			case 101:
				message = "Target player not found.";
				break;
			case 104:
				message = "Trade cancelled, duplicated Lore Items would result.";
				break;
			case 105:
				message = "You cannot form an affinity with this area. Try a city.";
				break;
			case 106:
				message = "This spell does not work here.";
				break;
			case 107:
				message = "This spell does not work on this plane.";
				break;
			case 108:
				message = "You cannot see your target.";
				break;
			case 113:
				message = "The next group buff you cast will hit all targets in range.";
				break;
			case 116:
				message = "Your ability failed. Timer has been reset.";
				break;
			case 114:
				message = "You escape from combat, hiding yourself from view.";
				break;
			case 119:
				message = "Alternate Experience is *OFF*.";
				break;
			case 121:
				message = "Alternate Experience is *ON*.";
				break;
			case 124:
				message = "Your target is too far away, get closer!";
				break;
			case 126:
				message = "Your will is not sufficient to command this weapon.";
				break;
			case 127:
				message = "Your pet's will is not sufficient to command its weapon.";
				break;
			case 128:
				message = "You unleash a flurry of attacks.";
				break;
			case 130:
				message = "It's locked and you're not holding the key.";
				break;
			case 131:
				message = "This lock cannot be picked.";
				break;
			case 132:
				message = "You are not sufficiently skilled to pick this lock.";
				break;
			case 133:
				message = "You opened the locked door with your magic GM key.";
				break;
			case 136:
				message = "You are not sufficient level to use this item.";
				break;
			case 138:
				message = "You gain experience!!";
				break;
			case 139:
				message = "You gain party experience!!";
				break;
			case 143:
				message = "Your bow shot did double dmg.";
				break;
			case 146:
				message = "You can't bandage without bandages, go buy some.";
				break;
			case 147:
				message = "You are being bandaged. Stay relatively still.";
				break;
			case 148:
				message = "You can't try to forage while attacking.";
				break;
			case 149:
				message = "You must be standing to forage.";
				break;
			case 150:
				message = "You have scrounged up some fishing grubs.";
				break;
			case 151:
				message = "You have scrounged up some water.";
				break;
			case 152:
				message = "You have scrounged up some food.";
				break;
			case 153:
				message = "You have scrounged up some drink.";
				break;
			case 154:
				message = "You have scrounged up something that doesn't look edible.";
				break;
			case 155:
				message = "You fail to locate any food nearby.";
				break;
			case 156:
				message = "You are already fishing!";
				break;
			case 158:
				message = "You can't fish while holding something.";
				break;
			case 160:
				message = "You can't fish without a fishing pole, go buy one.";
				break;
			case 161:
				message = "You need to put your fishing pole in your primary hand.";
				break;
			case 162:
				message = "You can't fish without fishing bait, go buy some.";
				break;
			case 163:
				message = "You cast your line.";
				break;
			case 164:
				message = "You're not scaring anyone.";
				break;
			case 165:
				message = "You stop fishing and go on your way.";
				break;
			case 166:
				message = "Trying to catch land sharks perhaps?";
				break;
			case 167:
				message = "Trying to catch a fire elemental or something?";
				break;
			case 168:
				message = "You didn't catch anything.";
				break;
			case 169:
				message = "Your fishing pole broke!";
				break;
			case 170:
				message = "You caught, something...";
				break;
			case 171:
				message = "You spill your beer while bringing in your line.";
				break;
			case 172:
				message = "You lost your bait!";
				break;
			case 173:
				message = "Your spell fizzles!";
				break;
			case 179:
				message = "You cannot use this item unless it is equipped.";
				break;
			case 180:
				message = "You miss a note, bringing your song to a close!";
				break;
			case 181:
				message = "Your race, class, or deity cannot use this item.";
				break;
			case 182:
				message = "Item is out of charges.";
				break;
			case 191:
				message = "Your target has no mana to affect.";
				break;
			case 196:
				message = "You must first target a group member.";
				break;
			case 197:
				message = "Your spell is too powerful for your intended target.";
				break;
			case 199:
				message = "Insufficient Mana to cast this spell!";
				break;
			case 203:
				message = "This being is not a worthy sacrifice.";
				break;
			case 204:
				message = "This being is too powerful to be a sacrifice.";
				break;
			case 205:
				message = "You cannot sacrifice yourself.";
				break;
			case 207:
				message = "You *CANNOT* cast spells, you have been silenced!";
				break;
			case 208:
				message = "Spell can only be cast during the day.";
				break;
			case 209:
				message = "Spell can only be cast during the night.";
				break;
			case 210:
				message = "That spell can not affect this target PC.";
				break;
			case 214:
				message = "You must first select a target for this spell!";
				break;
			case 215:
				message = "You must first target a living group member whose corpse you wish to summon.";
				break;
			case 221:
				message = "This spell only works on corpses.";
				break;
			case 224:
				message = "You can't drain yourself!";
				break;
			case 230:
				message = "This corpse is not valid.";
				break;
			case 231:
				message = "This player cannot be resurrected. The corpse is too old.";
				break;
			case 234:
				message = "You can only cast this spell in the outdoors.";
				break;
			case 236:
				message = "Spell recast time not yet met.";
				break;
			case 237:
				message = "Spell recovery time not yet met.";
				break;
			case 238:
				message = "Your Portal fails to open.";
				break;
			case 239:
				message = "Your target cannot be mesmerized.";
				break;
			case 240:
				message = "Your target cannot be mesmerized (with this spell).";
				break;
			case 241:
				message = "Your target is immune to the stun portion of this effect.";
				break;
			case 242:
				message = "Your target is immune to changes in its attack speed.";
				break;
			case 243:
				message = "Your target is immune to fear spells.";
				break;
			case 244:
				message = "Your target is immune to changes in its run speed.";
				break;
			case 246:
				message = "You cannot have more than one pet at a time.";
				break;
			case 248:
				message = "Your target is too high of a level for your charm spell.";
				break;
			case 251:
				message = "That spell can not affect this target NPC.";
				break;
			case 256:
				message = "Your pet is the focus of something's attention.";
				break;
			case 255:
				message = "You do not have a pet.";
				break;
			case 260:
				message = "Your gate is too unstable, and collapses.";
				break;
			case 262:
				message = "You cannot sense any corpses for this PC in this zone.";
				break;
			case 263:
				message = "Your spell did not take hold.";
				break;
			case 267:
				message = "This NPC cannot be charmed.";
				break;
			case 268:
				message = "Your target looks unaffected.";
				break;
			case 269:
				message = "Stick to singing until you learn to play this instrument.";
				break;
			case 270:
				message = "You regain your concentration and continue your casting.";
				break;
			case 271:
				message = "Your spell would not have taken hold on your target.";
				break;
			case 272:
				message = "You are missing some required spell components.";
				break;
			case 275:
				message = "You feel yourself starting to appear.";
				break;
			case 278:
				message = "You lose the concentration to remain in your fighting discipline.";
				break;
			case 289:
				message = "You regain some experience from resurrection.";
				break;
			case 290:
				message = "Duplicate lore items are not allowed.";
				break;
			case 293:
				message = "Target other group buff is *ON*.";
				break;
			case 294:
				message = "Target other group buff is *OFF*.";
				break;
			case 303:
				message = "I don't see anyone by that name around here...";
				break;
			case 334:
				message = "You cannot combine these items in this container type!";
				break;
			case 336:
				message = "You lacked the skills to fashion the items together.";
				break;
			case 338:
				message = "You can no longer advance your skill from making this item.";
				break;
			case 339:
				message = "You have fashioned the items together to create something new!";
				break;
			case 343:
				message = "You have momentarily ducked away from the main combat.";
				break;
			case 344:
				message = "Your attempts at ducking clear of combat fail.";
				break;
			case 345:
				message = "You failed to hide yourself.";
				break;
			case 346:
				message = "You have hidden yourself from view.";
				break;
			case 347:
				message = "You are as quiet as a cat stalking its prey.";
				break;
			case 348:
				message = "You are as quiet as a herd of running elephants.";
				break;
			case 349:
				message = "You magically mend your wounds and heal considerable damage.";
				break;
			case 350:
				message = "You mend your wounds and heal some damage.";
				break;
			case 351:
				message = "You have worsened your wounds!";
				break;
			case 352:
				message = "You have failed to mend your wounds.";
				break;
			case 371:
				message = "You cannot loot this Lore Item. You already have one.";
				break;
			case 379:
				message = "You cannot pick up a lore item you already possess.";
				break;
			case 389:
				message = "The corpse is too far away to summon.";
				break;
			case 390:
				message = "You do not have consent to summon that corpse.";
				break;
			case 393:
				message = "You are ready to use a new discipline now.";
				break;
			case 397:
				message = "Not a valid consent name.";
				break;
			case 398:
				message = "You cannot consent NPC\'s.";
				break;
			case 399:
				message = "You cannot consent yourself.";
				break;
			case 405:
				message = "You need to play a percussion instrument for this song.";
				break;
			case 406:
				message = "You need to play a wind instrument for this song.";
				break;
			case 407:
				message = "You need to play a stringed instrument for this song.";
				break;
			case 408:
				message = "You need to play a brass instrument for this song.";
				break;
			case 419:
				message = "You have been healed for ";
				message += emu->message;
				message += " points of damage.";
				break;
			case 422:
				message = "Your ";
				message += emu->message;
				message += " begins to glow.";
				break;
			case 423:
				message = emu->message;
				message += " tries to cast an invisibility spell on you, but you are already invisible.";
				break;
			case 424:
				message = emu->message;
				message += " tries to cast a spell on you, but you are protected.";
				break;
			case 425:
				message = "Your target resisted the ";
				message += emu->message;
				message += " spell.";
				break;
			case 426:
				message = "You resist the ";
				message += emu->message;
				message += " spell!";
				break;
			case 427:
				message = "You perform an exceptional heal! (";
				message += emu->message;
				message += ")";
				break;
			case 428:
				message = "You deliver a critical blast! (";
				message += emu->message;
				message += ")";
				break;
			case 429:
				message = "Summoning your corpse.";
				break;
			case 430:
				message = "Summoning ";
				message += emu->message;
				message += "'s corpse.";
				break;
			case 433:
				message = "You are missing ";
				message += emu->message;
				break;
			case 469:
				message = "Your faction standing with ";
				message += emu->message;
				message += " could not possibly get any worse.";
				break;
			case 470:
				message = "Your faction standing with ";
				message += emu->message;
				message += " got worse.";
				break;
			case 471:
				message = "Your faction standing with ";
				message += emu->message;
				message += " could not possibly get any better.";
				break;
			case 472:
				message = "Your faction standing with ";
				message += emu->message;
				message += " got better.";
				break;
			case 12268:
				message = "You must target a player or use /invite <name> to invite someone to your group.";
				break;
			case 12270:
				message = "You cannot invite yourself.";
				break;
			case 12290:
				message = "You abandon your preparations to camp.";
				break;
			case 12323:
				message = "Talking to yourself again?";
				break;
			case 12368:
				message = "You do not have control of yourself right now.";
				break;
			case 12409:
				message = "You can't seem to steal from yourself for some reason...";
				break;
			case 12406:
				message = "You must target a player to steal from first.  You may not steal from corpses.";
				break;
			case 12410:
				message = "You may not steal from a person who does not follow the ways of chaos....";
				break;
			case 12413:
				message = "You can only steal from others in your level range.";
				break;
			case 12440:
				message = "You are too distracted to cast a spell now!";
				break;
			case 12442:
				message = "You are already casting a spell!";
				break;
			case 12444:
				message = "Your ";
				message += emu->message;
				message += " shimmers briefly.";
				break;
			case 12446:
				message = "You don't sense any corpses of that name.";
				break;
			case 12447:
				message = "You don't sense any corpses.";
				break;
			case 12448:
				message = "Your immunity buff protected you from the spell ";
				message += emu->message;
				message += " !";
				break;
			case 12452:
				message = "You are not holding an item!";
				break;
			case 12471:
				message = "You sense undead in this direction.";
				break;
			case 12472:
				message = "You sense an animal in this direction.";
				break;
			case 12473:
				message = "You sense a summoned being in this direction.";
				break;
			case 12474:
				message = "You don't sense anything.";
				break;
			case 12478:
				message = emu->message;
				message += "'s casting is interrupted!";
				break;
			case 12481:
				message = "You were hit by non-melee for ";
				message += emu->message;
				message += " damage.";
				break;
			default:
				message = "Unhandled string_id: ";
				message += itoa(emu->string_id);
				break;
			}
		}
		if (message.length() > 0) {
			int msglen = message.length() + 1;
			int len = msglen + sizeof(structs::SpecialMesg_Struct) + 4;
			EQApplicationPacket* outapp = new EQApplicationPacket(OP_SpecialMesg, len);
			structs::SpecialMesg_Struct *eq = (structs::SpecialMesg_Struct *) outapp->pBuffer;
			memset(outapp->pBuffer, 0, len);
			eq->msg_type = emu->type;
			strncpy(eq->message, message.c_str(), msglen);
			return outapp;
		}
		return nullptr;
	}
	DECODE(OP_InterruptCast)
	{		
		unsigned char *__emu_buffer = __packet->pBuffer;
		InterruptCast_Struct *emu = (InterruptCast_Struct *)__emu_buffer;
		std::string message = "";
		switch (emu->messageid) {
		case 12687:
			message = "Your song ends.";
			break;
		case 439:
			message = "Your spell is interrupted.";
			break;
		case 12686:
			message = "Your song ends abruptly.";
			break;
		case 173:
			message = "Your spell fizzles!";
			break;
		default:
			break;
		}
		int msglen = message.length() + 1;
		int len = msglen + sizeof(structs::InterruptCast_Struct);
		EQApplicationPacket* outapp = new EQApplicationPacket(OP_InterruptCast, len);
		structs::InterruptCast_Struct *eq = (structs::InterruptCast_Struct *) outapp->pBuffer;
		memset(outapp->pBuffer, 0, len);
		strncpy(eq->message, emu->message, msglen);
		return outapp;
	}

	DECODE(OP_SpecialMesg)
	{
		unsigned char *__emu_buffer = __packet->pBuffer;
		SpecialMesg_Struct *emu = (SpecialMesg_Struct *)__emu_buffer;
		uint32 __i = 0;
		__i++;
		int msglen = __packet->size - sizeof(SpecialMesg_Struct);
		int len = msglen + sizeof(structs::SpecialMesg_Struct) + 4;
		EQApplicationPacket* outapp = new EQApplicationPacket(OP_SpecialMesg, len);
		structs::SpecialMesg_Struct *eq = (structs::SpecialMesg_Struct *) outapp->pBuffer;
		memset(outapp->pBuffer, 0, len);
		eq->msg_type = emu->msg_type;
		strncpy(eq->message, emu->message, msglen);
		return outapp;
	}

	ENCODE(OP_ChannelMessage)
	{
		unsigned char *__eq_buffer = p->pBuffer;
		structs::ChannelMessage_Struct *eq = (structs::ChannelMessage_Struct *) __eq_buffer;
		int msglen = p->size - sizeof(structs::ChannelMessage_Struct) - 4;
		int len = msglen + sizeof(ChannelMessage_Struct);
		EQApplicationPacket* outapp = new EQApplicationPacket(OP_ChannelMessage, len);
		ChannelMessage_Struct *emu = (ChannelMessage_Struct *) outapp->pBuffer;
		memset(outapp->pBuffer, 0, len);

		strn0cpy(emu->targetname, eq->targetname, 32);
		strn0cpy(emu->sender, eq->targetname, 32);
		emu->language = eq->language;
		emu->chan_num = eq->chan_num;
		emu->skill_in_language = eq->skill_in_language;
		strcpy(emu->message, eq->message);
		return outapp;
	}
	/*

	DECODE(OP_TargetMouse)
	{
		SETUP_DIRECT_DECODE(ClientTarget_Struct, structs::ClientTarget_Struct);
		IN(new_target);
		FINISH_DIRECT_DECODE();
	}

	DECODE(OP_TargetCommand)
	{
		SETUP_DIRECT_DECODE(ClientTarget_Struct, structs::ClientTarget_Struct);
		IN(new_target);
		FINISH_DIRECT_DECODE();
	}

	DECODE(OP_Surname)
	{
		SETUP_DIRECT_DECODE(Surname_Struct, structs::Surname_Struct);
		strn0cpy(emu->name, eq->name, 32);
		emu->unknown0064 = eq->unknown032;
		strn0cpy(emu->lastname, eq->lastname, 20);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_Surname)
	{
		ENCODE_LENGTH_EXACT(Surname_Struct);
		SETUP_DIRECT_ENCODE(Surname_Struct, structs::Surname_Struct);
		strn0cpy(eq->name, emu->name, 32);
		eq->unknown032 = emu->unknown0064;
		strn0cpy(eq->lastname, emu->lastname, 20);
		FINISH_ENCODE();
	}

	DECODE(OP_Taunt)
	{
		SETUP_DIRECT_DECODE(ClientTarget_Struct, structs::ClientTarget_Struct);
		IN(new_target);
		FINISH_DIRECT_DECODE();
	}

	DECODE(OP_SetServerFilter)
	{
		DECODE_LENGTH_EXACT(structs::SetServerFilter_Struct);
		SETUP_DIRECT_DECODE(SetServerFilter_Struct, structs::SetServerFilter_Struct);
		emu->filters[0] = eq->filters[5]; //GuildChat
		emu->filters[1] = eq->filters[6]; //Socials
		emu->filters[2] = eq->filters[7]; //GroupChat
		emu->filters[3] = eq->filters[8]; //Shouts
		emu->filters[4] = eq->filters[9]; //Auctions
		emu->filters[5] = eq->filters[10];//OOC
		emu->filters[6] = 1;				//BadWords (Handled by LogServer?)
		emu->filters[7] = eq->filters[2]; //PC Spells 0 is on
		emu->filters[8] = 0;				//NPC Spells Client has it but it doesn't work. 0 is on.
		emu->filters[9] = eq->filters[3]; //Bard Songs 0 is on
		emu->filters[10] = eq->filters[15]; //Spell Crits 0 is on
		int critm = eq->filters[16];
		if (critm > 0){ critm = critm - 1; }
		emu->filters[11] = critm;			//Melee Crits 0 is on. EQMac has 3 options, Emu only 2.
		emu->filters[12] = eq->filters[0]; //Spell Damage 0 is on
		emu->filters[13] = eq->filters[11]; //My Misses
		emu->filters[14] = eq->filters[12]; //Others Misses
		emu->filters[15] = eq->filters[13]; //Others Hit
		emu->filters[16] = eq->filters[14]; //Missed Me
		emu->filters[17] = 0;			  //Damage Shields
		emu->filters[18] = 0;			  //DOT
		emu->filters[19] = 0;			  //Pet Hits
		emu->filters[20] = 0;			  //Pet Misses
		emu->filters[21] = 0;			  //Focus Effects
		emu->filters[22] = 0;			  //Pet Spells
		emu->filters[23] = 0;			  //HoT	
		emu->filters[24] = 0;			  //Unknowns
		emu->filters[25] = 0;
		emu->filters[26] = 0;
		emu->filters[27] = 0;
		FINISH_DIRECT_DECODE();
	}
	*/


structs::Spawn_Struct* TrilogySpawns(struct Spawn_Struct* emu, int type)
{

	if (sizeof(emu) == 0)
		return 0;

	structs::Spawn_Struct *eq = new structs::Spawn_Struct;
	memset(eq, 0, sizeof(structs::Spawn_Struct));
	eq->deltaHeading = emu->deltaHeading;

	strn0cpy(eq->name, emu->name, 30);
	eq->deity = emu->deity;
	if ((emu->race == 42 || emu->race == 120) && emu->gender == 2)
		eq->size = emu->size + 4.0f;
	else
		eq->size = emu->size;
	eq->NPC = emu->NPC;

	eq->cur_hp = emu->cur_hp;
	eq->x_pos = (int16)emu->x_pos;
	eq->y_pos = (int16)emu->y_pos;
	eq->z_pos = (int16)emu->z_pos;
	eq->deltaY = 0;
	eq->deltaX = 0;
	eq->heading = (uint8)emu->heading;
	eq->deltaZ = 0;
	eq->level = emu->level;
	eq->petOwnerId = emu->petOwnerId;
	eq->guildrank = emu->guildrank;
	eq->bodytexture = emu->bodytexture;
	for (int k = 0; k < 9; k++)
	{
		eq->equipment[k] = emu->equipment[k];
		eq->equipcolors[k].color = emu->equipcolors[k].color;
	}
	eq->runspeed = emu->runspeed;
	eq->LD = 0;					// 0=NotLD, 1=LD
	eq->GuildID = emu->GuildID;
	if (emu->NPC == 1)
	{
		eq->guildrank = 0;
		eq->LD = 1;
		eq->GuildID = 0XFFFF;
	}
	if (eq->GuildID == 0)
		eq->GuildID = 0xFFFF;
	eq->helm = emu->helm;
	eq->face = emu->face;
	eq->gender = emu->gender;
	eq->bodytype = emu->bodytype;

	if (emu->race >= 209 && emu->race <= 212)
	{
		eq->race = 75;
		if (emu->race == 210)
			eq->bodytexture = 3;
		else if (emu->race == 211)
			eq->bodytexture = 2;
		else if (emu->race == 212)
			eq->bodytexture = 1;
		else
			eq->bodytexture = 0;
	}
	else
		eq->race = (int8)emu->race;

	strn0cpy(eq->Surname, emu->Surname, 20);
	eq->walkspeed = emu->walkspeed;

	if (emu->class_ > 19 && emu->class_ < 35)
		eq->class_ = emu->class_ - 3;
	else if (emu->class_ == 40)
		eq->class_ = 16;
	else if (emu->class_ == 41)
		eq->class_ = 32;
	else
		eq->class_ = emu->class_;
	eq->anon = 0;
	eq->spawn_id = emu->spawn_id;

	eq->invis = emu->invis;			// 0=visable, 1=invisable
	eq->sneaking = 0;
	eq->anim_type = emu->anim_type;
	eq->light = emu->light;
	eq->anon = emu->anon;			// 0=normal, 1=anon, 2=RP
	eq->AFK = emu->AFK;				// 0=off, 1=on
	eq->GM = emu->GM;

	eq->flymode = emu->flymode;
	eq->pvp = emu->pvp;
	return eq;
}

	DECODE(OP_ZoneSpawns)
	{

		//consume the packet
		DecryptZoneSpawnPacket(__packet->pBuffer, __packet->size);
		EQApplicationPacket* outapp = new EQApplicationPacket();
		outapp->SetOpcode(OP_ZoneSpawns);
		outapp->pBuffer = new uchar[100000];
		outapp->size = InflatePacket((unsigned char*)__packet->pBuffer, __packet->size, outapp->pBuffer, 100000);
		Log.Out(Logs::Detail, Logs::Netcode, "Got %i", sizeof(structs::Spawn_Struct));
		int entrycount = outapp->size / sizeof(Spawn_Struct);
		if (entrycount == 0 || (outapp->size % sizeof(Spawn_Struct)) != 0)
		{
			Log.Out(Logs::General, Logs::Netcode, "[STRUCTS] Wrong size on inbound %s: Got %d, expected multiple of %d", opcodes->EmuToName(outapp->GetOpcode()), outapp->size, sizeof(Spawn_Struct));
			safe_delete(outapp);
			return nullptr;
		}
		//do the transform...
		Spawn_Struct *eq = (Spawn_Struct *)outapp->pBuffer;
		EQApplicationPacket* outapp2 = new EQApplicationPacket(OP_ZoneSpawns, sizeof(structs::Spawn_Struct)*entrycount);
		structs::Spawn_Struct *emu = (structs::Spawn_Struct *)outapp2->pBuffer;

		//zero out the packet. We could avoid this memset by setting all fields (including unknowns)
		memset(outapp2->pBuffer, 0, outapp2->size);

		for (int r = 0; r < entrycount; r++, eq++, emu++)
		{
			struct structs::Spawn_Struct* spawns = TrilogySpawns(eq, 0);
			memcpy(emu, spawns, sizeof(structs::Spawn_Struct));
			safe_delete(spawns);
		}

		EQApplicationPacket* outapp3 = new EQApplicationPacket(OP_ZoneSpawns, sizeof(structs::Spawn_Struct)*entrycount);
		outapp3->size = DeflatePacket((unsigned char*)outapp2->pBuffer, outapp2->size, outapp3->pBuffer, sizeof(structs::Spawn_Struct)*entrycount);
		EncryptTrilogyZoneSpawnPacket(outapp3->pBuffer, outapp3->size);
		safe_delete(__packet);
		safe_delete(outapp);
		safe_delete(outapp2);
		return outapp3;

	}




	/*

	ENCODE(OP_GuildsList)
	{
		//consume the packet
		EQApplicationPacket *in = p;
		

		//store away the emu struct
		unsigned char *__emu_buffer = in->pBuffer;
		OldGuildsList_Struct *old_guildlist_pkt = (OldGuildsList_Struct *)__emu_buffer;
		int num_guilds = (in->size - 4) / sizeof(OldGuildsListEntry_Struct);
		Log.Out(Logs::Detail, Logs::Zone_Server, "GuildList size %i", num_guilds);

		if (num_guilds == 0) {
			delete in;
			return nullptr;
		}
		if (num_guilds > 511)
			num_guilds = 511;
		EQApplicationPacket* outapp = new EQApplicationPacket(OP_GuildsList, 4 + sizeof(structs::GuildsListEntry_Struct) * num_guilds);
		structs::GuildsList_Struct *new_list = (structs::GuildsList_Struct *)outapp->pBuffer;
		memset(new_list, 0, outapp->size);
		memcpy(new_list->head, old_guildlist_pkt->head, 4);
		for (int i = 0; i < num_guilds; i++)
		{
			new_list->Guilds[i].guildID = old_guildlist_pkt->Guilds[i].guildID;
			strn0cpy(new_list->Guilds[i].name, old_guildlist_pkt->Guilds[i].name, 32);
			new_list->Guilds[i].unknown1 = 0xFFFFFFFF;
			new_list->Guilds[i].unknown3 = 0xFFFFFFFF;
		};
		delete[] __emu_buffer;
		return outapp;

	}

	ENCODE(OP_GuildAdded)
	{
		SETUP_DIRECT_ENCODE(OldGuildUpdate_Struct, structs::GuildUpdate_Struct);
		OUT(guildID);
		eq->entry.exists = emu->entry.exists;
		eq->entry.guildID = emu->entry.guildID;
		strn0cpy(eq->entry.name, emu->entry.name, 32);
		eq->entry.unknown1 = 0xFFFFFFFF;
		eq->entry.unknown3 = 0xFFFFFFFF;
		FINISH_ENCODE();
	}
	*/

	DECODE(OP_NewSpawn)
	{
		//consume the packet
		DecryptZoneSpawnPacket(__packet->pBuffer, __packet->size);
		EQApplicationPacket* outapp = new EQApplicationPacket();
		outapp->SetOpcode(OP_NewSpawn);
		outapp->pBuffer = new uchar[100000];
		outapp->size = InflatePacket((unsigned char*)__packet->pBuffer, __packet->size, outapp->pBuffer, 100000);
		Log.Out(Logs::Detail, Logs::Netcode, "Got %i", sizeof(structs::Spawn_Struct));

		//do the transform...
		Spawn_Struct *eq = (Spawn_Struct *)outapp->pBuffer;
		EQApplicationPacket* outapp2 = new EQApplicationPacket(OP_NewSpawn, sizeof(structs::NewSpawn_Struct));
		structs::Spawn_Struct *emu = (structs::Spawn_Struct *)outapp2->pBuffer;
		struct structs::Spawn_Struct* spawns = TrilogySpawns(eq, 0);
		memcpy(emu, spawns, sizeof(structs::Spawn_Struct));
		safe_delete(spawns);
		EncryptTrilogyZoneSpawnPacket(outapp2->pBuffer, outapp2->size);
		safe_delete(outapp);
		return outapp2;

	}

	DECODE(OP_SpawnAppearance)
	{
		SETUP_DIRECT_DECODE(structs::SpawnAppearance_Struct, SpawnAppearance_Struct);
		emu->spawn_id = eq->spawn_id;
		if (eq->type == 16)
		{
			DWORD LocalPC = *(DWORD*)(0x6E4D8C);
			Log.Out(Logs::General, Logs::General, "Spawnid was: %i", *(uint16_t*)(LocalPC + 128));
			*(uint16_t*)(LocalPC + 128) = (uint16_t)eq->parameter;
			Log.Out(Logs::General, Logs::General, "Spawnid is now: %i", *(uint16_t*)(LocalPC + 128));
		}
		emu->type = eq->type;
		emu->parameter = eq->parameter;
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_SpawnAppearance)
	{
		SETUP_DIRECT_ENCODE(structs::SpawnAppearance_Struct, SpawnAppearance_Struct);
		eq->spawn_id = emu->spawn_id;
		eq->parameter = emu->parameter;
		eq->type = emu->type;
		FINISH_ENCODE();
	}

	/*

	DECODE(OP_ZoneChange)
	{
		DECODE_LENGTH_EXACT(structs::ZoneChange_Struct);
		SETUP_DIRECT_DECODE(ZoneChange_Struct, structs::ZoneChange_Struct);
		memcpy(emu->char_name, eq->char_name, sizeof(emu->char_name));
		char shortname[16];
		strncpy(shortname, eq->short_name, 16);
		int i = 179;
		emu->zoneID = 0;
		if (strlen(eq->short_name) > 0)
		{
			while (emu->zoneID == 0 && i > 0)
			{
				if (!strcasecmp(eq->short_name, StaticGetZoneName(i)))
					emu->zoneID = i;
				i--;
			}
		}
		IN(zone_reason);
		IN(success);

		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_ZoneChange)
	{
		ENCODE_LENGTH_EXACT(ZoneChange_Struct);
		SETUP_DIRECT_ENCODE(ZoneChange_Struct, structs::ZoneChange_Struct);

		strn0cpy(eq->char_name, emu->char_name, 30);
		strn0cpy(eq->short_name, StaticGetZoneName(emu->zoneID), 15);
		OUT(zone_reason);
		OUT(success);
		FINISH_ENCODE();
	}

	ENCODE(OP_CancelTrade)
	{
		ENCODE_LENGTH_EXACT(CancelTrade_Struct);
		SETUP_DIRECT_ENCODE(CancelTrade_Struct, structs::CancelTrade_Struct);
		OUT(fromid);
		eq->action = 1665;
		FINISH_ENCODE();
	}

	DECODE(OP_CancelTrade)
	{
		DECODE_LENGTH_EXACT(structs::TradeRequest_Struct);
		SETUP_DIRECT_DECODE(TradeRequest_Struct, structs::TradeRequest_Struct);
		IN(from_mob_id);
		IN(to_mob_id);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_TradeMoneyUpdate)
	{
		ENCODE_LENGTH_EXACT(TradeMoneyUpdate_Struct);
		SETUP_DIRECT_ENCODE(TradeMoneyUpdate_Struct, structs::TradeMoneyUpdate_Struct);
		OUT(trader);
		OUT(type);
		OUT(amount);
		FINISH_ENCODE();
	}

	DECODE(OP_TradeMoneyUpdate)
	{
		DECODE_LENGTH_EXACT(structs::TradeMoneyUpdate_Struct);
		SETUP_DIRECT_DECODE(TradeMoneyUpdate_Struct, structs::TradeMoneyUpdate_Struct);
		IN(trader);
		IN(type);
		IN(amount);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_MemorizeSpell)
	{
		SETUP_DIRECT_ENCODE(MemorizeSpell_Struct, structs::MemorizeSpell_Struct);
		OUT(slot);
		OUT(spell_id);
		OUT(scribing);
		FINISH_ENCODE();
	}

	DECODE(OP_MemorizeSpell)
	{
		DECODE_LENGTH_EXACT(structs::MemorizeSpell_Struct);
		SETUP_DIRECT_DECODE(MemorizeSpell_Struct, structs::MemorizeSpell_Struct);
		IN(slot);
		IN(spell_id);
		IN(scribing);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_Buff)
	{
		SETUP_DIRECT_ENCODE(SpellBuffFade_Struct, structs::SpellBuffFade_Struct);
		OUT(entityid);
		OUT(spellid);
		OUT(slotid);
		OUT(bufffade);
		OUT(duration);
		OUT(slot);
		OUT(level);
		OUT(effect);
		FINISH_ENCODE();
	}

	DECODE(OP_Buff)
	{
		DECODE_LENGTH_EXACT(structs::Buff_Struct);
		SETUP_DIRECT_DECODE(SpellBuffFade_Struct, structs::SpellBuffFade_Struct);
		IN(entityid);
		IN(spellid);
		IN(slotid);
		IN(bufffade);
		IN(duration);
		IN(slot);
		IN(level);
		IN(effect);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_BeginCast)
	{
		SETUP_DIRECT_ENCODE(BeginCast_Struct, structs::BeginCast_Struct);
		if (emu->spell_id >= 2999)
			eq->spell_id = 0xFFFF;
		else
			eq->spell_id = emu->spell_id;
		OUT(caster_id);
		OUT(cast_time);
		FINISH_ENCODE();
	}

	DECODE(OP_BeginCast)
	{
		DECODE_LENGTH_EXACT(structs::BeginCast_Struct);
		SETUP_DIRECT_DECODE(BeginCast_Struct, structs::BeginCast_Struct);
		OUT(spell_id);
		OUT(caster_id);
		OUT(cast_time);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_CastSpell)
	{
		SETUP_DIRECT_ENCODE(CastSpell_Struct, structs::CastSpell_Struct);
		OUT(slot);
		OUT(spell_id);
		eq->inventoryslot = ServerToTrilogySlot(emu->inventoryslot);
		OUT(target_id);
		FINISH_ENCODE();
	}

	DECODE(OP_CastSpell) {
		DECODE_LENGTH_EXACT(structs::CastSpell_Struct);
		SETUP_DIRECT_DECODE(CastSpell_Struct, structs::CastSpell_Struct);
		IN(slot);
		IN(spell_id);
		emu->inventoryslot = TrilogyToServerSlot(eq->inventoryslot);
		IN(target_id);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_Damage)
	{
		ENCODE_LENGTH_EXACT(CombatDamage_Struct);
		SETUP_DIRECT_ENCODE(CombatDamage_Struct, structs::CombatDamage_Struct);
		OUT(target);
		OUT(source);
		OUT(type);
		OUT(spellid);
		OUT(damage);
		OUT(force);
		OUT(sequence);
		FINISH_ENCODE();
	}

	ENCODE(OP_Action2) { ENCODE_FORWARD(OP_Action); }
	ENCODE(OP_Action)
	{
		ENCODE_LENGTH_EXACT(Action_Struct);
		SETUP_DIRECT_ENCODE(Action_Struct, structs::Action_Struct);
		eq->target = emu->target;
		eq->source = emu->source;
		if (emu->level > 65)
			eq->level = 0x41;
		else
			eq->level = static_cast<uint8>(emu->level);
		eq->spell_level = 0x41;
		eq->instrument_mod = static_cast<uint8>(emu->instrument_mod);
		eq->force = emu->force;

		OUT(pushup_angle);
		eq->type = emu->type;
		if (emu->spell > 2999)
			eq->spell = 0xFFFF;
		else
			eq->spell = emu->spell;
		eq->buff_unknown = emu->buff_unknown;
		eq->sequence = emu->sequence;
		FINISH_ENCODE();
	}

	DECODE(OP_Damage) { DECODE_FORWARD(OP_EnvDamage); }
	DECODE(OP_EnvDamage)
	{
		DECODE_LENGTH_EXACT(structs::EnvDamage2_Struct);
		SETUP_DIRECT_DECODE(EnvDamage2_Struct, structs::EnvDamage2_Struct);
		IN(id);
		IN(dmgtype);
		IN(damage);
		IN(constant);
		FINISH_DIRECT_DECODE();
	}
	*/
	ENCODE(OP_ConsiderCorpse) { ENCODE_FORWARD(OP_Consider); }
	ENCODE(OP_Consider)
	{
		ENCODE_LENGTH_EXACT(structs::Consider_Struct);
		SETUP_DIRECT_ENCODE(structs::Consider_Struct, Consider_Struct);
		OUT(playerid);
		OUT(targetid);
		OUT(faction);
		OUT(level);
		OUT(cur_hp);
		OUT(max_hp);
		OUT(pvpcon);
		FINISH_ENCODE();
	}

	DECODE(OP_Consider)
	{
		DECODE_LENGTH_EXACT(Consider_Struct);
		SETUP_DIRECT_DECODE(structs::Consider_Struct, Consider_Struct);
		IN(playerid);
		IN(targetid);
		IN(faction);
		IN(level);
		IN(cur_hp);
		IN(max_hp);
		IN(pvpcon);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_ClientUpdate)
	{
		SETUP_DIRECT_ENCODE(structs::SpawnPositionUpdate_Struct, structs::SpawnPositionUpdate_Struct);
		eq->anim_type = emu->anim_type;
		eq->delta_heading = emu->delta_heading;
		eq->delta_x = emu->delta_x;
		eq->delta_y = emu->delta_y;
		eq->delta_z = emu->delta_z;
		eq->heading = emu->heading;
		eq->x_pos = emu->x_pos;
		eq->y_pos = emu->y_pos;
		eq->z_pos = emu->z_pos;
		eq->spawn_id = emu->spawn_id;
		FINISH_ENCODE();
	}

	/*
	DECODE(OP_ClickDoor)
	{
		DECODE_LENGTH_EXACT(structs::ClickDoor_Struct);
		SETUP_DIRECT_DECODE(ClickDoor_Struct, structs::ClickDoor_Struct);
		IN(doorid);
		IN(item_id);
		IN(player_id);
		FINISH_DIRECT_DECODE();
	}

	DECODE(OP_GMEndTraining)
	{
		DECODE_LENGTH_EXACT(structs::GMTrainEnd_Struct);
		SETUP_DIRECT_DECODE(GMTrainEnd_Struct, structs::GMTrainEnd_Struct);
		IN(npcid);
		IN(playerid);
		FINISH_DIRECT_DECODE();
	}
	*/

	structs::Item_Struct* TrilogyItem(const Item_Struct *item, int16 slot_id_in, int type)
	{
		unsigned char *buffer = new unsigned char[sizeof(structs::Item_Struct)];
		structs::Item_Struct *trilogy_pop_item = (structs::Item_Struct *)buffer;
		memset(trilogy_pop_item, 0, sizeof(structs::Item_Struct));
		trilogy_pop_item->equipSlot = item->equipSlot;
		trilogy_pop_item->common.Charges = item->common.MaxCharges;
		trilogy_pop_item->Price = item->Price;
		trilogy_pop_item->common.SellRate = item->SellRate;

		// Comment: Flag value indicating type of item:
		// Comment: 0x0000 - Readable scroll? few i've seen say "rolled up", i think they're like books
		// 	#define ITEM_NORMAL1                    0x0031
		// 	#define ITEM_NORMAL2                    0x0036
		// 	#define ITEM_NORMAL3                    0x315f
		// 	#define ITEM_NORMAL4                    0x3336
		// Comment: 0x0031 - Normal Item - Only seen once on GM summoned food
		// Comment: 0x0036 - Normal Item (all scribed spells, Velium proc weapons, and misc.)
		// Comment: 0x315f - Normal Item
		// Comment: 0x3336 - Normal Item
		// Comment: 0x3d00 - Container, racial tradeskills? or maybe non-consuming? i dunnno, something weirdo =p
		// Comment: 0x5400 - Container (Combine, Player made, Weight Reducing, etc...)
		// Comment: 0x5450 - Container, plain ordinary newbie containers
		// Comment: 0x7669 - Book item
		if (item->ItemClass == 2) {
			// books
			trilogy_pop_item->flag = 0x7669;
		}
		else if (item->ItemClass == 1) {
			if (item->container.BagType > 8)
				trilogy_pop_item->flag = 0x3d00;
			else
				trilogy_pop_item->flag = 0x5450;
		}
		else {
			if ((item->Effect2 > 0 && item->Effect2 < 3000) || (item->Effect2 > 0 && item->Effect2 < 3000) || (item->Effect2 > 0 && item->Effect2 < 3000) || (item->Effect2 > 0 && item->Effect2 < 3000))
				trilogy_pop_item->flag = 0x0036;
			else
				trilogy_pop_item->flag = 0x315f;
		}
		//trilogy_pop_item->flag = 0x0036;
		trilogy_pop_item->ItemClass = item->ItemClass;
		strn0cpy(trilogy_pop_item->Name, item->Name, 35);
		strn0cpy(trilogy_pop_item->Lore, item->Lore, 60);
		strn0cpy(trilogy_pop_item->IDFile, item->IDFile, 6);
		trilogy_pop_item->Weight = item->Weight;
		trilogy_pop_item->NoRent = item->NoRent;
		trilogy_pop_item->NoDrop = item->NoDrop;
		trilogy_pop_item->Size = item->Size;
		trilogy_pop_item->ID = item->ID;
		trilogy_pop_item->Icon = item->Icon;
		trilogy_pop_item->Slots = item->Slots;

		if (item->ItemClass == 2)
		{
			strncpy(trilogy_pop_item->book.Filename, item->IDFile, 15);
			trilogy_pop_item->book.Book = item->book.Book;
			trilogy_pop_item->book.BookType = item->book.BookType;
		}
		else
		{
			trilogy_pop_item->common.unknown0282 = 0xFF;
			trilogy_pop_item->common.unknown0283 = 0XFF;
			trilogy_pop_item->common.CastTime = item->CastTime;
			trilogy_pop_item->common.SkillModType = item->SkillModType;
			trilogy_pop_item->common.SkillModValue = item->SkillModValue;
			trilogy_pop_item->common.BaneDmgRace = item->BaneDmgRace;
			trilogy_pop_item->common.BaneDmgBody = item->BaneDmgBody;
			trilogy_pop_item->common.BaneDmgAmt = item->BaneDmgAmt;
			trilogy_pop_item->common.RecLevel = item->RecLevel;
			trilogy_pop_item->common.RecSkill = item->RecSkill;
			trilogy_pop_item->common.ProcRate = item->ProcRate;
			trilogy_pop_item->common.ElemDmgType = item->ElemDmgType;
			trilogy_pop_item->common.ElemDmgAmt = item->ElemDmgAmt;
			trilogy_pop_item->common.FactionMod1 = item->FactionMod1;
			trilogy_pop_item->common.FactionMod2 = item->FactionMod2;
			trilogy_pop_item->common.FactionMod3 = item->FactionMod3;
			trilogy_pop_item->common.FactionMod4 = item->FactionMod4;
			trilogy_pop_item->common.FactionAmt1 = item->FactionAmt1;
			trilogy_pop_item->common.FactionAmt2 = item->FactionAmt2;
			trilogy_pop_item->common.FactionAmt3 = item->FactionAmt3;
			trilogy_pop_item->common.FactionAmt4 = item->FactionAmt4;
			trilogy_pop_item->common.Deity = item->Deity;

			if (item->ItemClass == 1)
			{
				trilogy_pop_item->common.container.BagType = item->container.BagType;
				trilogy_pop_item->common.container.BagSlots = item->container.BagSlots;
				trilogy_pop_item->common.container.IsBagOpen = item->container.IsBagOpen;
				trilogy_pop_item->common.container.BagSize = item->container.BagSize;
				trilogy_pop_item->common.container.BagWR = item->container.BagWR;
			}
			else {
				trilogy_pop_item->common.normal.Races = item->common.Races;
				if (item->common.EffectType1 == 1 || (item->common.EffectType1 == 3 || (item->common.EffectType1 == 5))) {
					if (item->common.EffectType1 == 5) {
						trilogy_pop_item->common.normal.click_effect_type = 3;
					}
					else {
						trilogy_pop_item->common.normal.click_effect_type = item->common.EffectType1;
					}
				}
				else if (item->common.EffectType1 == 2) {
					trilogy_pop_item->common.normal.click_effect_type = item->common.EffectType1;
				}
				else if (item->common.EffectType1 == 7) {
					trilogy_pop_item->common.normal.click_effect_type = item->common.EffectType1;
				}
				else
				{ 
					trilogy_pop_item->common.normal.click_effect_type = item->common.EffectType1;
				}
			}
			trilogy_pop_item->common.AStr = item->common.AStr;
			trilogy_pop_item->common.ASta = item->common.ASta;
			trilogy_pop_item->common.ACha = item->common.ACha;
			trilogy_pop_item->common.ADex = item->common.ADex;
			trilogy_pop_item->common.AInt = item->common.AInt;
			trilogy_pop_item->common.AAgi = item->common.AAgi;
			trilogy_pop_item->common.AWis = item->common.AWis;
			trilogy_pop_item->common.MR = item->common.MR;
			trilogy_pop_item->common.FR = item->common.FR;
			trilogy_pop_item->common.CR = item->common.CR;
			trilogy_pop_item->common.DR = item->common.DR;
			trilogy_pop_item->common.PR = item->common.PR;
			trilogy_pop_item->common.HP = item->common.HP;
			trilogy_pop_item->common.Mana = item->common.Mana;
			trilogy_pop_item->common.AC = item->common.AC;
			//trilogy_pop_item->MaxCharges = item->MaxCharges;
			trilogy_pop_item->common.Light = item->common.Light;
			trilogy_pop_item->common.Delay = item->common.Delay;
			trilogy_pop_item->common.Damage = item->common.Damage;
			trilogy_pop_item->common.Range = item->common.Range;
			trilogy_pop_item->common.ItemType = item->common.ItemType;
			trilogy_pop_item->common.Magic = item->common.Magic;
			trilogy_pop_item->common.Material = item->common.Material;
			trilogy_pop_item->common.Color = item->common.Color;
			//trilogy_pop_item->common.Faction = item->Faction;
			trilogy_pop_item->common.Classes = item->common.Classes;
			trilogy_pop_item->common.Stackable = (item->common.Stackable == 1 ? 1 : 0);
		}

		//FocusEffect and BardEffect is already handled above. Now figure out click, scroll, proc, and worn.
			trilogy_pop_item->common.Effect1 = item->common.Effect1;
			trilogy_pop_item->common.Effect2 = item->Effect2;
			trilogy_pop_item->common.EffectType2 = item->common.EffectType1;
			trilogy_pop_item->common.EffectType1 = item->EffectType2;
			trilogy_pop_item->common.EffectLevel1 = item->common.EffectLevel1;
			trilogy_pop_item->common.EffectLevel2 = item->EffectLevel2;

		return trilogy_pop_item;
	}

	DECODE(OP_SummonedItem) { DECODE_FORWARD(OP_ItemPacket); }
	DECODE(OP_ItemLinkResponse) { DECODE_FORWARD(OP_ItemPacket); }
	DECODE(OP_MerchantItemPacket) { DECODE_FORWARD(OP_ItemPacket); }
	DECODE(OP_LootItemPacket) { DECODE_FORWARD(OP_ItemPacket) }
	DECODE(OP_ContainerPacket) { DECODE_FORWARD(OP_ItemPacket) }
	DECODE(OP_BookPacket) { DECODE_FORWARD(OP_ItemPacket) }
	DECODE(OP_ItemPacket)
	{	

		//store away the emu struct
		unsigned char *__emu_buffer = __packet->pBuffer;
		Item_Struct *old_item_pkt = (Item_Struct *)__emu_buffer;

		if (old_item_pkt)
		{
			structs::Item_Struct* trilogy_item = TrilogyItem(old_item_pkt, old_item_pkt->equipSlot, 0);

			EQApplicationPacket* outapp = new EQApplicationPacket(__packet->GetOpcode(), sizeof(structs::Item_Struct));
			memcpy(outapp->pBuffer, trilogy_item, sizeof(structs::Item_Struct));

			if (outapp->size != sizeof(structs::Item_Struct))
				Log.Out(Logs::Detail, Logs::Zone_Server, "Invalid size on OP_ItemPacket packet. Expected: %i, Got: %i", sizeof(structs::Item_Struct), outapp->size);

			safe_delete_array(trilogy_item);
			safe_delete_array(__emu_buffer);
			safe_delete(__packet);
			return outapp;
		}
	}

	DECODE(OP_TradeItemPacket)
	{
		//consume the packet
		EQApplicationPacket *in = __packet;
		
		//store away the emu struct
		unsigned char *__emu_buffer = __packet->pBuffer;
		TradeItemsPacket_Struct *old_item_pkt = (TradeItemsPacket_Struct *)__emu_buffer;

		if (old_item_pkt)
		{
			structs::Item_Struct* trilogy_item = TrilogyItem(&old_item_pkt->item, old_item_pkt->item.equipSlot, 0);

			EQApplicationPacket* outapp = new EQApplicationPacket(OP_TradeItemPacket, sizeof(structs::TradeItemsPacket_Struct));
			structs::TradeItemsPacket_Struct* myitem = (structs::TradeItemsPacket_Struct*) outapp->pBuffer;
			myitem->fromid = old_item_pkt->fromid;
			myitem->slotid = old_item_pkt->slotid;
			memcpy(&myitem->item, trilogy_item, sizeof(structs::Item_Struct));

			if (outapp->size != sizeof(structs::TradeItemsPacket_Struct))
				Log.Out(Logs::Detail, Logs::Zone_Server, "Invalid size on OP_TradeItemPacket packet. Expected: %i, Got: %i", sizeof(structs::TradeItemsPacket_Struct), outapp->size);

			safe_delete(__packet);
			return outapp;
		}
	}

	DECODE(OP_CharInventory)
	{
		//consume the packet
		EQApplicationPacket* outapp = new EQApplicationPacket();
		outapp->SetOpcode(OP_CharInventory);
		outapp->pBuffer = new uchar[100000];
		outapp->size = InflatePacket((unsigned char*)__packet->pBuffer+2, __packet->size-2, outapp->pBuffer, 100000);

		//store away the emu struct
		unsigned char *__emu_buffer = outapp->pBuffer;

		int16 itemcount = outapp->size / sizeof(PlayerItemsPacket_Struct);
		if (itemcount == 0 || (outapp->size % sizeof(PlayerItemsPacket_Struct)) != 0)
		{
			Log.Out(Logs::General, Logs::Netcode, "[STRUCTS] Wrong size on outbound %s: Got %d, expected multiple of %d", opcodes->EmuToName(outapp->GetOpcode()), outapp->size, sizeof(InternalSerializedItem_Struct));
			safe_delete(outapp);
			return nullptr;
		}

		int pisize = sizeof(structs::PlayerItems_Struct) + (250 * sizeof(structs::PlayerItemsPacket_Struct));
		structs::PlayerItems_Struct* pi = (structs::PlayerItems_Struct*) new uchar[pisize];
		memset(pi, 0, pisize);

		PlayerItemsPacket_Struct *eq = (PlayerItemsPacket_Struct *)outapp->pBuffer;
		//do the transform...
		std::string trilogy_item_string;
		int r;
		//std::string trilogy_item_string;
		for (r = 0; r < itemcount; r++, eq++)
		{
			structs::Item_Struct* trilogy_item = TrilogyItem(&eq->item, eq->item.equipSlot, 0);

			structs::PlayerItemsPacket_Struct* trilogy_player_item = new structs::PlayerItemsPacket_Struct;
			memset(trilogy_player_item, 0, sizeof(structs::PlayerItemsPacket_Struct));
			trilogy_player_item->item = *trilogy_item;
			switch (trilogy_player_item->item.ItemClass)
			{
			case 0x01:
				trilogy_player_item->opcode = 0x2166;
				break;
			case 0x02:
				trilogy_player_item->opcode = 0x2165;
			default:
				trilogy_player_item->opcode = 0x2164;
				break;
			}
			char *trilogy_item_char = reinterpret_cast<char*>(trilogy_player_item);
			trilogy_item_string.append(trilogy_item_char, sizeof(structs::PlayerItemsPacket_Struct));
			safe_delete(trilogy_item);
			safe_delete(trilogy_player_item);
		}
		int32 length = 5000;
		int buffer = 2;

		memcpy(pi->packets, trilogy_item_string.c_str(), trilogy_item_string.length());
		EQApplicationPacket* outapp2 = new EQApplicationPacket(OP_CharInventory, length);
		outapp2->size = buffer + DeflatePacket((uchar*)pi->packets, itemcount * sizeof(structs::PlayerItemsPacket_Struct), &outapp2->pBuffer[buffer], length - buffer);
		outapp2->pBuffer[0] = itemcount;
		safe_delete_array(pi);
		safe_delete(outapp);
		return outapp2;
	}

	DECODE(OP_ShopInventoryPacket)
	{
		//consume the packet
		EQApplicationPacket* outapp = new EQApplicationPacket();
		outapp->SetOpcode(OP_ShopInventoryPacket);
		outapp->pBuffer = new uchar[100000];
		outapp->size = InflatePacket((unsigned char*)__packet->pBuffer, __packet->size, outapp->pBuffer, 100000);		

		//store away the emu struct
		unsigned char *__emu_buffer = outapp->pBuffer;

		int16 itemcount = outapp->size / sizeof(MerchantItemsPacket_Struct);
		if (itemcount == 0 || (outapp->size % sizeof(MerchantItemsPacket_Struct)) != 0)
		{
			Log.Out(Logs::Detail, Logs::Zone_Server, "Wrong size on outbound %s: Got %d, expected multiple of %d", opcodes->EmuToName(outapp->GetOpcode()), outapp->size, sizeof(InternalSerializedItem_Struct));
			safe_delete(outapp);
			return nullptr;
		}
		if (itemcount > 40)
			itemcount = 40;

		MerchantItemsPacket_Struct *eq = (MerchantItemsPacket_Struct *)outapp->pBuffer;
		//do the transform...
		std::string trilogy_item_string;
		int r = 0;
		for (r = 0; r < itemcount; r++, eq++)
		{
			structs::Item_Struct* trilogy_item = TrilogyItem(&eq->item,  eq->item.equipSlot, 0);

			if (trilogy_item != 0)
			{
				int pisize = ITEM_STRUCT_SIZE;
				if (trilogy_item->ItemClass == ItemClassBook)
					pisize = SHORT_BOOK_ITEM_STRUCT_SIZE;
				else if (trilogy_item->ItemClass == ItemClassContainer)
					pisize = SHORT_CONTAINER_ITEM_STRUCT_SIZE;

				EQApplicationPacket* outapp2 = new EQApplicationPacket();
				outapp2->SetOpcode(OP_ShopInventoryPacket);
				outapp2->size = pisize + 5;
				outapp2->pBuffer = new unsigned char[outapp2->size];
				structs::MerchantItems_Struct* pi = (structs::MerchantItems_Struct*) outapp2->pBuffer;
				memcpy(&pi->item, trilogy_item, pisize);
				pi->itemtype = trilogy_item->ItemClass;
				CEverQuest__HandleWorldMessage_Trampoline(conn_ptr, 0x0c20, (char*)pi, outapp2->size);
				safe_delete(trilogy_item);
				safe_delete(outapp2);
			}
		}
		safe_delete(outapp);
		return nullptr;
	}

	/*
	
	ENCODE(OP_HPUpdate)
	{
		ENCODE_LENGTH_EXACT(SpawnHPUpdate_Struct);
		SETUP_DIRECT_ENCODE(SpawnHPUpdate_Struct, structs::SpawnHPUpdate_Struct);
		OUT(spawn_id);
		OUT(cur_hp);
		OUT(max_hp);
		FINISH_ENCODE();
	}

	ENCODE(OP_MobHealth)
	{
		ENCODE_LENGTH_EXACT(SpawnHPUpdate_Struct2);
		SETUP_DIRECT_ENCODE(SpawnHPUpdate_Struct2, structs::SpawnHPUpdate_Struct);
		OUT(spawn_id);
		eq->cur_hp = emu->hp;
		eq->max_hp = 100;
		FINISH_ENCODE();
	}

	DECODE(OP_Consume)
	{
		DECODE_LENGTH_EXACT(structs::Consume_Struct);
		SETUP_DIRECT_DECODE(Consume_Struct, structs::Consume_Struct);

		emu->slot = TrilogyToServerSlot(eq->slot);
		IN(type);
		IN(auto_consumed);

		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_ReadBook)
	{
		// no apparent slot translation needed -U
		EQApplicationPacket *in = p;
		

		unsigned char *__emu_buffer = in->pBuffer;
		BookText_Struct *emu_BookText_Struct = (BookText_Struct *)__emu_buffer;
		in->size = sizeof(structs::BookText_Struct) + strlen(emu_BookText_Struct->booktext);
		in->pBuffer = new unsigned char[in->size];
		structs::BookText_Struct *eq_BookText_Struct = (structs::BookText_Struct*)in->pBuffer;

		eq_BookText_Struct->type = emu_BookText_Struct->type;
		strcpy(eq_BookText_Struct->booktext, emu_BookText_Struct->booktext);

		delete[] __emu_buffer;
		return in;

	}

	DECODE(OP_ReadBook)
	{
		DECODE_LENGTH_ATLEAST(structs::BookRequest_Struct);
		SETUP_DIRECT_DECODE(BookRequest_Struct, structs::BookRequest_Struct);

		IN(type);
		strn0cpy(emu->txtfile, eq->txtfile, sizeof(emu->txtfile));

		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_Illusion)
	{
		ENCODE_LENGTH_EXACT(Illusion_Struct);
		SETUP_DIRECT_ENCODE(Illusion_Struct, structs::Illusion_Struct);
		OUT(spawnid);
		OUT(race);
		OUT(gender);
		OUT(texture);
		OUT(helmtexture);
		OUT(face);
		OUT(hairstyle);
		OUT(haircolor);
		OUT(beard);
		OUT(beardcolor);
		eq->size = (int16)(emu->size + 0.5);
		eq->unknown007 = 0xFF;
		eq->unknown_void = 0xFFFFFFFF;

		FINISH_ENCODE();
	}

	DECODE(OP_Illusion)
	{
		DECODE_LENGTH_EXACT(structs::Illusion_Struct);
		SETUP_DIRECT_DECODE(Illusion_Struct, structs::Illusion_Struct);
		IN(spawnid);
		IN(race);
		IN(gender);
		IN(texture);
		IN(helmtexture);
		IN(face);
		IN(hairstyle);
		IN(haircolor);
		IN(beard);
		IN(beardcolor);
		IN(size);

		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_ShopRequest)
	{
		ENCODE_LENGTH_EXACT(Merchant_Click_Struct);
		SETUP_DIRECT_ENCODE(Merchant_Click_Struct, structs::Merchant_Click_Struct);
		eq->npcid = emu->npcid;
		OUT(playerid);
		OUT(command);
		eq->unknown[0] = 0x71;
		eq->unknown[1] = 0x54;
		eq->unknown[2] = 0x00;
		OUT(rate);
		FINISH_ENCODE();
	}

	DECODE(OP_ShopRequest)
	{
		DECODE_LENGTH_EXACT(structs::Merchant_Click_Struct);
		SETUP_DIRECT_DECODE(Merchant_Click_Struct, structs::Merchant_Click_Struct);
		emu->npcid = eq->npcid;
		IN(playerid);
		IN(command);
		IN(rate);
		FINISH_DIRECT_DECODE();
	}

	DECODE(OP_ShopPlayerBuy)
	{
		DECODE_LENGTH_EXACT(structs::Merchant_Sell_Struct);
		SETUP_DIRECT_DECODE(Merchant_Sell_Struct, structs::Merchant_Sell_Struct);
		emu->npcid = eq->npcid;
		IN(playerid);
		emu->itemslot = TrilogyToServerSlot(eq->itemslot);
		IN(quantity);
		IN(price);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_ShopPlayerBuy)
	{
		ENCODE_LENGTH_EXACT(Merchant_Sell_Struct);
		SETUP_DIRECT_ENCODE(Merchant_Sell_Struct, structs::Merchant_Sell_Struct);
		eq->npcid = emu->npcid;
		eq->playerid = emu->playerid;
		eq->itemslot = ServerToTrilogySlot(emu->itemslot);
		OUT(quantity);
		OUT(price);
		FINISH_ENCODE();
	}

	DECODE(OP_ShopPlayerSell)
	{
		DECODE_LENGTH_EXACT(structs::Merchant_Purchase_Struct);
		SETUP_DIRECT_DECODE(Merchant_Purchase_Struct, structs::Merchant_Purchase_Struct);
		emu->npcid = eq->npcid;
		//IN(playerid);
		emu->itemslot = TrilogyToServerSlot(eq->itemslot);
		IN(quantity);
		IN(price);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_ShopPlayerSell)
	{
		ENCODE_LENGTH_EXACT(Merchant_Purchase_Struct);
		SETUP_DIRECT_ENCODE(Merchant_Purchase_Struct, structs::Merchant_Purchase_Struct);
		eq->npcid = emu->npcid;
		//eq->playerid=emu->playerid;
		eq->itemslot = ServerToTrilogySlot(emu->itemslot);
		OUT(quantity);
		OUT(price);
		FINISH_ENCODE();
	}

	ENCODE(OP_ShopDelItem)
	{
		ENCODE_LENGTH_EXACT(Merchant_DelItem_Struct);
		SETUP_DIRECT_ENCODE(Merchant_DelItem_Struct, structs::Merchant_DelItem_Struct);
		eq->npcid = emu->npcid;
		OUT(playerid);
		eq->itemslot = ServerToTrilogySlot(emu->itemslot);
		if (emu->type == 0)
			eq->type = 64;
		else
			OUT(type);
		FINISH_ENCODE();
	}

	ENCODE(OP_Animation)
	{
		ENCODE_LENGTH_EXACT(Animation_Struct);
		SETUP_DIRECT_ENCODE(Animation_Struct, structs::Animation_Struct);
		OUT(spawnid);
		OUT(action);
		OUT(value);
		OUT(unknown10);
		FINISH_ENCODE();
	}

	DECODE(OP_Animation)
	{
		DECODE_LENGTH_EXACT(structs::Animation_Struct);
		SETUP_DIRECT_DECODE(Animation_Struct, structs::Animation_Struct);
		IN(spawnid);
		IN(action);
		IN(value);
		IN(unknown10);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_LootItem)
	{
		ENCODE_LENGTH_EXACT(LootingItem_Struct);
		SETUP_DIRECT_ENCODE(LootingItem_Struct, structs::LootingItem_Struct);
		OUT(lootee);
		OUT(looter);
		eq->slot_id = ServerToTrilogyCorpseSlot(emu->slot_id);
		OUT(auto_loot);

		FINISH_ENCODE();
	}

	DECODE(OP_LootItem)
	{
		DECODE_LENGTH_EXACT(structs::LootingItem_Struct);
		SETUP_DIRECT_DECODE(LootingItem_Struct, structs::LootingItem_Struct);
		IN(lootee);
		IN(looter);
		emu->slot_id = TrilogyToServerCorpseSlot(eq->slot_id);
		IN(auto_loot);

		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_AAExpUpdate)
	{
		ENCODE_LENGTH_EXACT(AltAdvStats_Struct);
		SETUP_DIRECT_ENCODE(AltAdvStats_Struct, structs::AltAdvStats_Struct);
		OUT(experience);
		OUT(unspent);
		OUT(percentage);
		FINISH_ENCODE();
	}

	ENCODE(OP_AAAction)
	{
		ENCODE_LENGTH_EXACT(UseAA_Struct);
		SETUP_DIRECT_ENCODE(UseAA_Struct, structs::UseAA_Struct);
		OUT(end);
		OUT(ability);
		OUT(begin);
		eq->unknown_void = 2154;

		FINISH_ENCODE();
	}

	ENCODE(OP_GroundSpawn)
	{

		ENCODE_LENGTH_EXACT(Object_Struct);
		SETUP_DIRECT_ENCODE(Object_Struct, structs::Object_Struct);
		OUT(drop_id);
		OUT(zone_id);
		OUT(heading);
		OUT(z);
		OUT(y);
		OUT(x);
		strncpy(eq->object_name, emu->object_name, 16);
		OUT(object_type);
		OUT(charges);
		OUT(maxcharges);
		int g;
		for (g = 0; g<10; g++)
		{
			if (eq->itemsinbag[g] > 0)
			{
				eq->itemsinbag[g] = emu->itemsinbag[g];
				Log.Out(Logs::Detail, Logs::Inventory, "Found a container item %i in slot: %i", emu->itemsinbag[g], g);
			}
			else
				eq->itemsinbag[g] = 0xFFFF;
		}
		eq->unknown208 = 0xFFFFFFFF;
		eq->unknown216[0] = 0xFFFF;
		FINISH_ENCODE();
	}

	DECODE(OP_GroundSpawn)
	{
		DECODE_LENGTH_EXACT(structs::Object_Struct);
		SETUP_DIRECT_DECODE(Object_Struct, structs::Object_Struct);
		IN(drop_id);
		IN(zone_id);
		IN(heading);
		IN(z);
		IN(y);
		IN(x);
		strncpy(emu->object_name, eq->object_name, 16);
		IN(object_type);

		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_ClickObjectAction)
	{

		ENCODE_LENGTH_EXACT(ClickObjectAction_Struct);
		SETUP_DIRECT_ENCODE(ClickObjectAction_Struct, structs::ClickObjectAction_Struct);
		OUT(player_id);
		OUT(drop_id);
		OUT(open);
		OUT(type);
		OUT(icon);
		eq->slot = emu->unknown16;
		FINISH_ENCODE();
	}

	DECODE(OP_ClickObjectAction)
	{
		DECODE_LENGTH_EXACT(structs::ClickObjectAction_Struct);
		SETUP_DIRECT_DECODE(ClickObjectAction_Struct, structs::ClickObjectAction_Struct);
		IN(player_id);
		IN(drop_id);
		IN(open);
		IN(type);
		IN(icon);
		emu->unknown16 = eq->slot;
		FINISH_DIRECT_DECODE();
	}

	DECODE(OP_TradeSkillCombine)
	{
		DECODE_LENGTH_EXACT(structs::Combine_Struct);
		SETUP_DIRECT_DECODE(NewCombine_Struct, structs::Combine_Struct);
		emu->container_slot = TrilogyToServerSlot(eq->container_slot);
		FINISH_DIRECT_DECODE();
	}

	DECODE(OP_TradeRequest)
	{
		DECODE_LENGTH_EXACT(structs::TradeRequest_Struct);
		SETUP_DIRECT_DECODE(TradeRequest_Struct, structs::TradeRequest_Struct);
		IN(from_mob_id);
		IN(to_mob_id);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_TradeRequest)
	{

		ENCODE_LENGTH_EXACT(TradeRequest_Struct);
		SETUP_DIRECT_ENCODE(TradeRequest_Struct, structs::TradeRequest_Struct);
		OUT(from_mob_id);
		OUT(to_mob_id);
		FINISH_ENCODE();
	}

	DECODE(OP_TradeRequestAck)
	{
		DECODE_LENGTH_EXACT(structs::TradeRequest_Struct);
		SETUP_DIRECT_DECODE(TradeRequest_Struct, structs::TradeRequest_Struct);
		IN(from_mob_id);
		IN(to_mob_id);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_TradeRequestAck)
	{

		ENCODE_LENGTH_EXACT(TradeRequest_Struct);
		SETUP_DIRECT_ENCODE(TradeRequest_Struct, structs::TradeRequest_Struct);
		OUT(from_mob_id);
		OUT(to_mob_id);
		FINISH_ENCODE();
	}

	ENCODE(OP_ManaChange)
	{
		EQApplicationPacket *__packet = p;
		
		unsigned char *__emu_buffer = __packet->pBuffer;
		ManaChange_Struct *emu = (ManaChange_Struct *)__emu_buffer;
		EQApplicationPacket* outapp = new EQApplicationPacket(OP_ManaChange);
		if (emu->spell_id > 0 && emu->spell_id < 3000)
		{
			outapp->size = sizeof(structs::ManaChange_Struct);
			outapp->pBuffer = new uchar[outapp->size];
			structs::ManaChange_Struct *eq = (structs::ManaChange_Struct *)outapp->pBuffer;
			eq->new_mana = emu->new_mana;
			eq->spell_id = emu->spell_id;

		}
		else {
			outapp->size = sizeof(structs::ManaChange_Struct2);
			outapp->pBuffer = new uchar[outapp->size];
			structs::ManaChange_Struct2 *eq = (structs::ManaChange_Struct2 *)outapp->pBuffer;
			eq->new_mana = emu->new_mana;
		}
		delete[] __emu_buffer;
		return outapp;
	}

	ENCODE(OP_DeleteSpawn)
	{
		SETUP_DIRECT_ENCODE(DeleteSpawn_Struct, structs::DeleteSpawn_Struct);
		OUT(spawn_id);
		FINISH_ENCODE();
	}

	ENCODE(OP_TimeOfDay)
	{
		SETUP_DIRECT_ENCODE(TimeOfDay_Struct, structs::TimeOfDay_Struct);
		OUT(hour);
		OUT(minute);
		OUT(day);
		OUT(month);
		OUT(year);
		FINISH_ENCODE();
	}

	DECODE(OP_WhoAllRequest)
	{
		DECODE_LENGTH_EXACT(structs::Who_All_Struct);
		SETUP_DIRECT_DECODE(Who_All_Struct, structs::Who_All_Struct);
		strcpy(emu->whom, eq->whom);
		IN(wrace);
		IN(wclass);
		IN(lvllow);
		IN(lvlhigh);
		IN(gmlookup);
		IN(guildid);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_GroupInvite2) { ENCODE_FORWARD(OP_GroupInvite); }
	ENCODE(OP_GroupInvite)
	{
		ENCODE_LENGTH_EXACT(GroupInvite_Struct);
		SETUP_DIRECT_ENCODE(GroupInvite_Struct, structs::GroupInvite_Struct);
		strn0cpy(eq->invitee_name, emu->invitee_name, 30);
		strn0cpy(eq->inviter_name, emu->inviter_name, 30);
		FINISH_ENCODE();
	}

	DECODE(OP_GroupInvite2) { DECODE_FORWARD(OP_GroupInvite); }
	DECODE(OP_GroupInvite)
	{
		DECODE_LENGTH_EXACT(structs::GroupInvite_Struct);
		SETUP_DIRECT_DECODE(GroupInvite_Struct, structs::GroupInvite_Struct);
		strcpy(emu->invitee_name, eq->invitee_name);
		strcpy(emu->inviter_name, eq->inviter_name);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_GroupFollow)
	{
		ENCODE_LENGTH_EXACT(GroupGeneric_Struct);
		SETUP_DIRECT_ENCODE(GroupGeneric_Struct, structs::GroupGeneric_Struct);
		strn0cpy(eq->name1, emu->name1, 30);
		strn0cpy(eq->name2, emu->name2, 30);
		FINISH_ENCODE();
	}

	DECODE(OP_GroupFollow)
	{
		DECODE_LENGTH_EXACT(structs::GroupGeneric_Struct);
		SETUP_DIRECT_DECODE(GroupGeneric_Struct, structs::GroupGeneric_Struct);
		strcpy(emu->name1, eq->name1);
		strcpy(emu->name2, eq->name2);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_GroupUpdate)
	{
		ENCODE_LENGTH_ATLEAST(GroupJoin_Struct);
		SETUP_DIRECT_ENCODE(GroupUpdate_Struct, structs::GroupUpdate_Struct);
		strn0cpy(eq->yourname, emu->yourname, 32);

		if (emu->action == 8)
		{
			// groupActMakeLeader = 8
			strn0cpy(eq->othername, emu->leadersname, 32);
			eq->action = 1;
		}
		else if (emu->action == 6) {
			// groupActDisband = 6
			strn0cpy(eq->othername, emu->yourname, 32);
			eq->action = 4;
		}
		else {
			strn0cpy(eq->othername, emu->membername[0], 32);
		}
		// Trilogy
		// #define	ADD_MEMBER		0
		// #define  NEW_LEADER		1
		// #define	REMOVE_MEMBER	3
		// #define	GROUP_QUIT		4
		// Group Full 5

		// groupActLeave = 1
		if (emu->action == 1)
			eq->action = 3;


		FINISH_ENCODE();
	}

	ENCODE(OP_GroupCancelInvite)
	{
		ENCODE_LENGTH_EXACT(GroupCancel_Struct);
		SETUP_DIRECT_ENCODE(GroupCancel_Struct, structs::GroupCancel_Struct);
		strn0cpy(eq->name1, emu->name1, 30);
		strn0cpy(eq->name2, emu->name2, 30);
		OUT(toggle);
		FINISH_ENCODE();
	}

	DECODE(OP_GroupCancelInvite)
	{
		DECODE_LENGTH_EXACT(structs::GroupCancel_Struct);
		SETUP_DIRECT_DECODE(GroupCancel_Struct, structs::GroupCancel_Struct);
		strcpy(emu->name1, eq->name1);
		strcpy(emu->name2, eq->name2);
		IN(toggle);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_GroupDisband)
	{
		ENCODE_LENGTH_EXACT(GroupGeneric_Struct);
		SETUP_DIRECT_ENCODE(GroupGeneric_Struct, structs::GroupGeneric_Struct);
		strn0cpy(eq->name1, emu->name1, 30);
		strn0cpy(eq->name2, emu->name2, 30);
		FINISH_ENCODE();
	}

	DECODE(OP_GroupDisband)
	{
		DECODE_LENGTH_EXACT(structs::GroupGeneric_Struct);
		SETUP_DIRECT_DECODE(GroupGeneric_Struct, structs::GroupGeneric_Struct);
		strcpy(emu->name1, eq->name1);
		strcpy(emu->name2, eq->name2);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_TradeCoins)
	{
		SETUP_DIRECT_ENCODE(TradeCoin_Struct, structs::TradeCoin_Struct);
		OUT(trader);
		OUT(slot);
		OUT(amount);
		eq->unknown005 = 0x4fD2;
		FINISH_ENCODE();
	}

	DECODE(OP_ItemLinkResponse)
	{
		DECODE_LENGTH_EXACT(structs::ItemViewRequest_Struct);
		SETUP_DIRECT_DECODE(ItemViewRequest_Struct, structs::ItemViewRequest_Struct);
		IN(item_id);
		strcpy(emu->item_name, eq->item_name);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_RequestClientZoneChange)
	{
		SETUP_DIRECT_ENCODE(RequestClientZoneChange_Struct, structs::RequestClientZoneChange_Struct);
		strn0cpy(eq->zone_name, StaticGetZoneName(emu->zone_id), 30);
		OUT(x);
		OUT(y);
		eq->z = emu->z * 10;
		OUT(heading);
		FINISH_ENCODE();
	}

	ENCODE(OP_Trader)
	{
		if ((p)->size == sizeof(Trader_Struct))
		{
			ENCODE_LENGTH_EXACT(Trader_Struct);
			SETUP_DIRECT_ENCODE(Trader_Struct, structs::Trader_Struct);
			OUT(Code);
			int k;
			for (k = 0; k < 80; k++)
			{
				eq->Items[k] = emu->Items[k];
			}
			for (k = 0; k < 80; k++)
			{
				eq->ItemCost[k] = emu->ItemCost[k];
			}
			FINISH_ENCODE();
		}
		else if ((p)->size == sizeof(Trader_ShowItems_Struct))
		{
			ENCODE_LENGTH_EXACT(Trader_ShowItems_Struct);
			SETUP_DIRECT_ENCODE(Trader_ShowItems_Struct, structs::Trader_ShowItems_Struct);
			OUT(Code);
			OUT(TraderID);
			if (emu->SubAction == 0)
				eq->SubAction = emu->Code;
			else
				OUT(SubAction);
			eq->Items[0] = emu->Unknown08[0];
			eq->Items[1] = emu->Unknown08[1];
			FINISH_ENCODE();
		}
		else if ((p)->size == sizeof(TraderPriceUpdate_Struct))
		{
			ENCODE_LENGTH_EXACT(TraderPriceUpdate_Struct);
			SETUP_DIRECT_ENCODE(TraderPriceUpdate_Struct, structs::TraderPriceUpdate_Struct);
			OUT(Action);
			OUT(SubAction);
			OUT(SerialNumber);
			OUT(NewPrice);
			FINISH_ENCODE();
		}
		else if ((p)->size == sizeof(TraderBuy_Struct))
		{
			ENCODE_LENGTH_EXACT(TraderBuy_Struct);
			SETUP_DIRECT_ENCODE(TraderBuy_Struct, structs::TraderBuy_Struct);
			OUT(Action);
			OUT(TraderID);
			OUT(ItemID);
			OUT(Price);
			OUT(Quantity);
			//OUT(Slot);
			strcpy(eq->ItemName, emu->ItemName);
			FINISH_ENCODE();
		}
	}

	DECODE(OP_Trader)
	{
		if (__packet->size == sizeof(structs::Trader_Struct))
		{
			DECODE_LENGTH_EXACT(structs::Trader_Struct);
			SETUP_DIRECT_DECODE(Trader_Struct, structs::Trader_Struct);
			IN(Code);
			int k;
			for (k = 0; k < 80; k++)
			{
				emu->Items[k] = eq->Items[k];
			}
			for (k = 0; k < 80; k++)
			{
				emu->ItemCost[k] = eq->ItemCost[k];
			}
			FINISH_DIRECT_DECODE();
		}
		else if (__packet->size == sizeof(structs::TraderStatus_Struct))
		{
			DECODE_LENGTH_EXACT(structs::TraderStatus_Struct);
			SETUP_DIRECT_DECODE(TraderStatus_Struct, structs::TraderStatus_Struct);
			IN(Code);
			IN(TraderID);
			FINISH_DIRECT_DECODE();
		}
		else if (__packet->size == sizeof(structs::TraderPriceUpdate_Struct))
		{
			DECODE_LENGTH_EXACT(structs::TraderPriceUpdate_Struct);
			SETUP_DIRECT_DECODE(TraderPriceUpdate_Struct, structs::TraderPriceUpdate_Struct);
			IN(Action);
			IN(SubAction);
			IN(SerialNumber);
			IN(NewPrice);
			FINISH_DIRECT_DECODE();
		}
	}

	ENCODE(OP_BecomeTrader)
	{
		ENCODE_LENGTH_EXACT(BecomeTrader_Struct);
		SETUP_DIRECT_ENCODE(BecomeTrader_Struct, structs::BecomeTrader_Struct);
		OUT(Code);
		OUT(ID);
		FINISH_ENCODE();
	}

	ENCODE(OP_TraderBuy)
	{
		ENCODE_LENGTH_EXACT(TraderBuy_Struct);
		SETUP_DIRECT_ENCODE(TraderBuy_Struct, structs::TraderBuy_Struct);
		OUT(Action);
		OUT(TraderID);
		OUT(ItemID);
		OUT(Price);
		OUT(Quantity);
		eq->Slot = emu->AlreadySold;
		strcpy(eq->ItemName, emu->ItemName);
		FINISH_ENCODE();
	}

	DECODE(OP_TraderBuy)
	{
		DECODE_LENGTH_EXACT(structs::TraderBuy_Struct);
		SETUP_DIRECT_DECODE(TraderBuy_Struct, structs::TraderBuy_Struct);
		IN(Action);
		IN(TraderID);
		IN(ItemID);
		IN(Price);
		IN(Quantity);
		emu->AlreadySold = eq->Slot;
		strcpy(emu->ItemName, eq->ItemName);
		FINISH_DIRECT_DECODE();
	}

	DECODE(OP_BazaarSearch){
		if (__packet->size == sizeof(structs::BazaarSearch_Struct))
		{
			DECODE_LENGTH_EXACT(structs::BazaarSearch_Struct);
			SETUP_DIRECT_DECODE(BazaarSearch_Struct, structs::BazaarSearch_Struct);
			IN(Beginning.Action);
			IN(TraderID);
			IN(Class_);
			IN(Race);
			IN(ItemStat);
			IN(Slot);
			IN(Type);
			IN(MinPrice);
			IN(MaxPrice);
			strcpy(emu->Name, eq->Name);
			FINISH_DIRECT_DECODE();
		}
		else if (__packet->size == sizeof(structs::BazaarWelcome_Struct))
		{
			DECODE_LENGTH_EXACT(structs::BazaarWelcome_Struct);
			SETUP_DIRECT_DECODE(BazaarWelcome_Struct, structs::BazaarWelcome_Struct);
			emu->Beginning.Action = eq->Action;
			IN(Traders);
			IN(Items);
			FINISH_DIRECT_DECODE();
		}
		else if (__packet->size == sizeof(BazaarInspect_Struct))
		{
			DECODE_LENGTH_EXACT(BazaarInspect_Struct);
		}
	}

	ENCODE(OP_WearChange)
	{
		ENCODE_LENGTH_EXACT(WearChange_Struct);
		SETUP_DIRECT_ENCODE(WearChange_Struct, structs::WearChange_Struct);
		OUT(spawn_id);
		OUT(material);
		eq->color = emu->color.color;
		eq->blue = emu->color.rgb.blue;
		eq->green = emu->color.rgb.green;
		eq->red = emu->color.rgb.red;
		eq->use_tint = emu->color.rgb.use_tint;
		OUT(wear_slot_id);
		FINISH_ENCODE();
	}

	DECODE(OP_WearChange)
	{
		DECODE_LENGTH_EXACT(structs::WearChange_Struct);
		SETUP_DIRECT_DECODE(WearChange_Struct, structs::WearChange_Struct);
		IN(spawn_id);
		IN(material);
		emu->color.color = eq->color;
		emu->color.rgb.blue = eq->blue;
		emu->color.rgb.green = eq->green;
		emu->color.rgb.red = eq->red;
		emu->color.rgb.use_tint = eq->use_tint;
		IN(wear_slot_id);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_ExpUpdate)
	{
		ENCODE_LENGTH_EXACT(ExpUpdate_Struct);
		SETUP_DIRECT_ENCODE(ExpUpdate_Struct, structs::ExpUpdate_Struct);
		OUT(exp);
		FINISH_ENCODE();
	}

	ENCODE(OP_Death)
	{
		ENCODE_LENGTH_EXACT(Death_Struct);
		SETUP_DIRECT_ENCODE(Death_Struct, structs::Death_Struct);
		OUT(spawn_id);
		OUT(killer_id);
		OUT(corpseid);
		OUT(spell_id);
		OUT(attack_skill);
		OUT(damage);
		FINISH_ENCODE();
	}

	DECODE(OP_Bug)
	{
		DECODE_LENGTH_EXACT(structs::BugStruct);
		SETUP_DIRECT_DECODE(BugStruct, structs::BugStruct);
		strcpy(emu->chartype, eq->chartype);
		strn0cpy(emu->bug, eq->bug, sizeof(eq->bug));
		strcpy(emu->name, eq->name);
		strcpy(emu->target_name, eq->target_name);
		IN(x);
		IN(y);
		IN(z);
		IN(heading);
		IN(type);
		FINISH_DIRECT_DECODE();
	}

	DECODE(OP_CombatAbility)
	{
		DECODE_LENGTH_EXACT(structs::CombatAbility_Struct);
		SETUP_DIRECT_DECODE(CombatAbility_Struct, structs::CombatAbility_Struct);
		IN(m_target);
		IN(m_atk);
		IN(m_skill);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_Projectile)
	{
		ENCODE_LENGTH_EXACT(Arrow_Struct);
		SETUP_DIRECT_ENCODE(Arrow_Struct, structs::Arrow_Struct);
		OUT(type);
		OUT(src_x);
		OUT(src_y);
		OUT(src_z);
		OUT(velocity);
		OUT(launch_angle);
		OUT(tilt);
		OUT(arc);
		OUT(source_id);
		OUT(target_id);
		OUT(skill);
		OUT(object_id);
		OUT(effect_type);
		OUT(yaw);
		OUT(pitch);
		OUT(behavior);
		OUT(light);
		strcpy(eq->model_name, emu->model_name);
		FINISH_ENCODE();
	}

	ENCODE(OP_Charm)
	{
		ENCODE_LENGTH_EXACT(Charm_Struct);
		SETUP_DIRECT_ENCODE(Charm_Struct, structs::Charm_Struct);
		OUT(owner_id);
		OUT(pet_id);
		OUT(command);
		FINISH_ENCODE();
	}

	ENCODE(OP_Sound)
	{
		ENCODE_LENGTH_EXACT(QuestReward_Struct);
		SETUP_DIRECT_ENCODE(QuestReward_Struct, structs::QuestReward_Struct);
		OUT(mob_id);
		OUT(copper);
		OUT(silver);
		OUT(gold);
		OUT(platinum);
		FINISH_ENCODE();
	}

	ENCODE(OP_FaceChange)
	{
		ENCODE_LENGTH_EXACT(FaceChange_Struct);
		SETUP_DIRECT_ENCODE(FaceChange_Struct, structs::FaceChange_Struct);
		OUT(haircolor);
		OUT(beardcolor);
		OUT(eyecolor1);
		OUT(eyecolor2);
		OUT(hairstyle);
		OUT(beard);
		OUT(face);
		FINISH_ENCODE();
	}

	DECODE(OP_FaceChange)
	{
		DECODE_LENGTH_EXACT(structs::FaceChange_Struct);
		SETUP_DIRECT_DECODE(FaceChange_Struct, structs::FaceChange_Struct);
		IN(haircolor);
		IN(beardcolor);
		IN(eyecolor1);
		IN(eyecolor2);
		IN(hairstyle);
		IN(beard);
		IN(face);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_Assist)
	{
		ENCODE_LENGTH_EXACT(EntityId_Struct);
		SETUP_DIRECT_ENCODE(EntityId_Struct, structs::EntityId_Struct);
		OUT(entity_id);
		FINISH_ENCODE();
	}

	DECODE(OP_Assist)
	{
		DECODE_LENGTH_EXACT(structs::EntityId_Struct);
		SETUP_DIRECT_DECODE(EntityId_Struct, structs::EntityId_Struct);
		IN(entity_id);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_RaidJoin) { ENCODE_FORWARD(OP_Unknown); }
	ENCODE(OP_RaidUpdate) { ENCODE_FORWARD(OP_Unknown); }
	ENCODE(OP_RaidInvite) { ENCODE_FORWARD(OP_Unknown); }
	ENCODE(OP_SendAAStats) { ENCODE_FORWARD(OP_Unknown); }
	ENCODE(OP_Unknown)
	{
		EQApplicationPacket *in = p;
		

		Log.Out(Logs::Detail, Logs::Client_Server_Packet, "Dropped an invalid packet: %s", opcodes->EmuToName(in->GetOpcode()));

		delete in;
		return nullptr;

	}

	static inline int16 ServerToTrilogySlot(uint32 ServerSlot)
	{
		//int16 TrilogySlot;
		if (ServerSlot == INVALID_INDEX)
			return INVALID_INDEX;

		return ServerSlot; // deprecated
	}

	static inline int16 ServerToTrilogyCorpseSlot(uint32 ServerCorpse)
	{
		return ServerCorpse;
	}

	static inline uint32 TrilogyToServerSlot(int16 TrilogySlot)
	{
		//uint32 ServerSlot;
		if (TrilogySlot == INVALID_INDEX)
			return INVALID_INDEX;

		return TrilogySlot; // deprecated
	}

	static inline uint32 TrilogyToServerCorpseSlot(int16 TrilogyCorpse)
	{
		return TrilogyCorpse;
	}
	*/

} //end namespace Trilogy
