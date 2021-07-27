
#include "global_define.h"
#include "eqemu_logsys.h"
#include "struct_strategy.h"

#include "eq_stream.h"
#include <map>


//note: all encoders and decoders must be valid functions.
//so if you specify set_defaults=false
StructStrategy::StructStrategy() {
	int r;
	for(r = 0; r < _maxEmuOpcode; r++) {
		encoders[r] = PassEncoder;
		decoders[r] = PassDecoder;
	}
}

EQApplicationPacket* StructStrategy::Encode(EQApplicationPacket *p) const {

	EmuOpcode op = (p)->GetOpcode();
	Encoder proc = encoders[op];
	return proc(p);
}

EQApplicationPacket* StructStrategy::Decode(EQApplicationPacket *p) const {
	EmuOpcode op = p->GetOpcode();
	Decoder proc = decoders[op];
	return proc(p);
}


EQApplicationPacket* StructStrategy::ErrorEncoder(EQApplicationPacket *in_p) {
	return in_p;
}

EQApplicationPacket* StructStrategy::ErrorDecoder(EQApplicationPacket *p) {
	return p;
}

EQApplicationPacket* StructStrategy::PassEncoder(EQApplicationPacket *p) {
	return p;
}

EQApplicationPacket* StructStrategy::PassDecoder(EQApplicationPacket *p) {
	return p;
}




//effectively a singleton, but I decided to do it this way for no apparent reason.
namespace StructStrategyFactory {

	static std::map<EmuOpcode, const StructStrategy *> strategies;

	void RegisterPatch(EmuOpcode first_opcode, const StructStrategy *structs) {
		strategies[first_opcode] = structs;
	}

	const StructStrategy *FindPatch(EmuOpcode first_opcode) {
		std::map<EmuOpcode, const StructStrategy *>::const_iterator res;
		res = strategies.find(first_opcode);
		if(res == strategies.end())
			return(nullptr);
		return(res->second);
	}

};

























