
#define ENCODE(x) EQApplicationPacket* Strategy::Encode_##x(EQApplicationPacket *p)
#define DECODE(x) EQApplicationPacket* Strategy::Decode_##x(EQApplicationPacket *__packet)

#define StructDist(in, f1, f2) (uint32(&in->f2)-uint32(&in->f1))
#define CopyBlock(to, field, from, field1, field2) \
	memcpy((void *) &to->field, (const void *) from->field1, StructDist(from, field1, field2));
#define CopyLen(to, field, from, field1, len) \
	memcpy((void *) &to->field, (const void *) from->field1, len);


/*
 *
 * for encoders
 *
 */
//more complex operations and variable length packets
#define FASTQUEUE(packet) dest->FastQueuePacket(&packet, ack_req);
#define TAKE(packet_name) \
	EQApplicationPacket *packet_name = *p; \
	*p = nullptr;

//simple buffer-to-buffer movement for fixed length packets
//the eq packet is mapped into `eq`, the emu packet into `emu`
#define SETUP_DIRECT_ENCODE(emu_struct, eq_struct) \
	SETUP_VAR_ENCODE(emu_struct); \
	ALLOC_VAR_ENCODE(eq_struct, sizeof(eq_struct));

//like a direct encode, but for variable length packets (two stage)
#define SETUP_VAR_ENCODE(emu_struct) \
	EQApplicationPacket *__packet = p; \
	unsigned char *__emu_buffer = __packet->pBuffer; \
	emu_struct *emu = (emu_struct *) __emu_buffer; \
	uint32 __i = 0; \
	__i++; /* to shut up compiler */

#define ALLOC_VAR_ENCODE(eq_struct, len) \
	__packet->pBuffer = new unsigned char[len]; \
	__packet->size = len; \
	memset(__packet->pBuffer, 0, len); \
	eq_struct *eq = (eq_struct *) __packet->pBuffer; \

//a shorter assignment for direct mode
#undef OUT
#define OUT(x) eq->x = emu->x;
#define OUT_str(x) \
	strncpy(eq->x, emu->x, sizeof(eq->x)); \
        eq->x[sizeof(eq->x)-1] = '\0';
#define OUT_array(x, n) \
	for(__i = 0; __i < n; __i++) \
		eq->x[__i] = emu->x[__i];

//call before any premature returns in an encoder using SETUP_DIRECT_ENCODE
#define FAIL_ENCODE() \
	safe_delete_array(__emu_buffer); \
	safe_delete(p); \
	return nullptr;

//call to finish an encoder using SETUP_DIRECT_ENCODE
#define FINISH_ENCODE() \
	return __packet;

//check length of packet before decoding. Call before setup.
#define ENCODE_LENGTH_EXACT(struct_) \
	if((p)->size != sizeof(struct_)) { \
		Log.Out(Logs::Detail, Logs::Netcode, "Wrong size on outbound %s (" #struct_ "): Got %d, expected %d", opcodes->EmuToName((p)->GetOpcode()), (p)->size, sizeof(struct_)); \
		return nullptr; \
	}
#define ENCODE_LENGTH_ATLEAST(struct_) \
	if((p)->size < sizeof(struct_)) { \
		Log.Out(Logs::Detail, Logs::Netcode, "Wrong size on outbound %s (" #struct_ "): Got %d, expected at least %d", opcodes->EmuToName((p)->GetOpcode()), (p)->size, sizeof(struct_)); \
		return nullptr; \
	}

//forward this opcode to another encoder
#define ENCODE_FORWARD(other_op) \
	return Encode_##other_op(p);

//destroy the packet, it is not sent to this client version
#define EAT_ENCODE(op) \
	ENCODE(op) { \
	return nullptr; \
	}



/*
 *
 * for decoders:
 *
 */


//Just checks to make sure the extra data is actually there. EQ clients could previously send 0-sized packets that require a decode and crash the server.
#define CHECK_DECODE_NULLPTR(eq_struct) \
	if(eq_struct == nullptr) { return nullptr; }

//Sanity checking to make sure the sizeof a struct is exactly a struct. This should be done on EVERY decode.
#define DECODE_LENGTH_ATLEAST(struct_) \
	if(__packet->size < sizeof(struct_)) { \
		__packet->SetOpcode(OP_Unknown); /* invalidate the packet */ \
		_log(NET__STRUCTS, "Wrong size on incoming %s (" #struct_ "): Got %d, expected at least %d", opcodes->EmuToName(__packet->GetOpcode()), __packet->size, sizeof(struct_)); \
		_hex(NET__STRUCT_HEX, __packet->pBuffer, __packet->size); \
		return nullptr; \
	}

//simple buffer-to-buffer movement for fixed length packets
//the eq packet is mapped into `eq`, the emu packet into `emu`
#define SETUP_DIRECT_DECODE(emu_struct, eq_struct) \
	CHECK_DECODE_NULLPTR(__packet->pBuffer); \
	DECODE_LENGTH_ATLEAST(eq_struct); \
	unsigned char *__eq_buffer = __packet->pBuffer; 	uint32 __i = 0;\
	__packet->size = sizeof(emu_struct); \
	__packet->pBuffer = new unsigned char[__packet->size]; \
	emu_struct *emu = (emu_struct *) __packet->pBuffer; \
	eq_struct *eq = (eq_struct *) __eq_buffer; 


#define MEMSET_IN(emu_struct) \
	memset(__packet->pBuffer, 0, sizeof(emu_struct));


//a shorter assignment for direct mode
#undef IN
#define IN(x) emu->x = eq->x;

#define IN_str(x) \
	strncpy(emu->x, eq->x, sizeof(emu->x)); \
        emu->x[sizeof(emu->x)-1] = '\0';
#define IN_array(x, n) \
	for(__i = 0; __i < n; __i++) \
		emu->x[__i] = eq->x[__i];

//call before any premature returns in an encoder using SETUP_DIRECT_DECODE
#define FAIL_DIRECT_DECODE() \
	safe_delete_array(__eq_buffer); \
	safe_delete(__packet); \
	safe_delete(p); \
	return nullptr;

//call to finish an encoder using SETUP_DIRECT_DECODE
#define FINISH_DIRECT_DECODE() \
	safe_delete_array(__eq_buffer); \
	return __packet;

//check length of packet before decoding. Call before setup.
#define DECODE_LENGTH_EXACT(struct_) \
	if(__packet->size != sizeof(struct_)) { \
		__packet->SetOpcode(OP_Unknown); /* invalidate the packet */ \
		Log.Out(Logs::Detail, Logs::Netcode, "Wrong size on incoming %s (" #struct_ "): Got %d, expected %d", opcodes->EmuToName(__packet->GetOpcode()), __packet->size, sizeof(struct_)); \
		return nullptr; \
	}

#pragma warning( disable : 4005 )

#define DECODE_LENGTH_ATLEAST(struct_) \
	if(__packet->size < sizeof(struct_)) { \
		__packet->SetOpcode(OP_Unknown); /* invalidate the packet */ \
		Log.Out(Logs::Detail, Logs::Netcode, "Wrong size on incoming %s (" #struct_ "): Got %d, expected at least %d", opcodes->EmuToName(__packet->GetOpcode()), __packet->size, sizeof(struct_)); \
		return nullptr; \
	}

//forward this opcode to another decoder
#define DECODE_FORWARD(other_op) \
	return Decode_##other_op(__packet);











