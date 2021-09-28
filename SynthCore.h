#ifndef __SYNTH_CORE_H__
#define __SYNTH_CORE_H__

#include <stdint.h>
#include "EnvelopeTable.h"

#define POLY_NUM 6
#define DECAY_TIME_FACTOR 150
#define ATTACK_TIME_FACTOR 10

enum ENVELOPE_STATUS
{
	STATUS_ATTACK = 0,
	STATUS_DECAY = 1,
	STATUS_IDLE = 2
};

typedef struct _SoundUnit
{
	uint16_t increment;
	uint8_t envelopeStatus;
	uint16_t waveGenAccumulator;
	uint8_t envelopeLevel;
	uint8_t envelopePos;
	uint16_t val;
	uint8_t sampleVal;
} SoundUnit;

typedef struct _SoundUnitSplit
{
	uint8_t increment_frac;
	uint8_t increment_int;
	uint8_t envelopeStatus;
	uint8_t waveGenAccumulator_frac;
	uint8_t waveGenAccumulator_int;
	uint8_t envelopeLevel;
	uint8_t envelopePos;
	uint16_t val;
	uint8_t sampleVal;
} SoundUnitSplit;

typedef union _SoundUnitUnion
{
	SoundUnit combine;
	SoundUnitSplit split;
} SoundUnitUnion;

typedef struct _Synthesizer
{
	SoundUnitUnion SoundUnitUnionList[POLY_NUM];
	uint16_t mixOut;
	uint8_t lastSoundUnit;
} Synthesizer;

extern void SynthInit(Synthesizer *synth);

#ifdef RUN_TEST
extern void NoteOnC(uint8_t note);
extern void SynthC(void);
extern void GenDecayEnvlopeC(void);
#endif

extern void NoteOnAsm(uint8_t note);
extern void GenDecayEnvlopeAsm(void);
extern void SynthAsm(void);

#ifdef RUN_TEST
extern Synthesizer synthForC;
#endif

extern __data Synthesizer synthForAsm;

#endif