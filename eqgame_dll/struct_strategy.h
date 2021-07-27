#ifndef STRUCTSTRATEGY_H_
#define STRUCTSTRATEGY_H_

class EQApplicationPacket;
class EQStreamInterface;
#include "emu_opcodes.h"
#include "clientversions.h"

#include <string>

class StructStrategy {
public:
	//the encoder takes ownership of the supplied packet, and may enqueue multiple resulting packets into the stream
	typedef EQApplicationPacket* (*Encoder)(EQApplicationPacket *p);
	//the decoder may only edit the supplied packet, producing a single packet for eqemu to consume.
	typedef EQApplicationPacket* (*Decoder)(EQApplicationPacket *p);

	StructStrategy();
	virtual ~StructStrategy() {}

	//this method takes an eqemu struct, and enqueues the produced structs into the stream.
	EQApplicationPacket* Encode(EQApplicationPacket *p) const;
	//this method takes an EQ wire struct, and converts it into an eqemu struct
	EQApplicationPacket* Decode(EQApplicationPacket *p) const;

	virtual std::string Describe() const = 0;
	virtual const EQClientVersion ClientVersion() const = 0;

protected:
	//some common coders:
	//Print an error saying unknown struct/opcode and drop it
	static EQApplicationPacket* ErrorEncoder(EQApplicationPacket *p);
	static EQApplicationPacket* ErrorDecoder(EQApplicationPacket *p);
	//pass the packet through without modification (emu == EQ) (default)
	static EQApplicationPacket* PassEncoder(EQApplicationPacket *p);
	static EQApplicationPacket* PassDecoder(EQApplicationPacket *p);

	Encoder encoders[_maxEmuOpcode];
	Decoder decoders[_maxEmuOpcode];
};

//effectively a singleton, but I decided to do it this way for no apparent reason.
namespace StructStrategyFactory {
	void RegisterPatch(EmuOpcode first_opcode, const StructStrategy *structs);

	//does NOT return ownership of the strategy.
	const StructStrategy *FindPatch(EmuOpcode first_opcode);
};


#endif /*STRUCTSTRATEGY_H_*/
























