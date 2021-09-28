#include "SynthCore.h"
#include <stdint.h>
#include <stdio.h>

#ifdef RUN_TEST
Synthesizer synthForC;
#endif

__code uint16_t AccumIncrement[128]={
	    16,    17,    18,    19,    21,    22,    23,    25,    26,    28,
    29,    31,    33,    35,    37,    39,    42,    44,    47,    50,
    53,    56,    59,    63,    66,    70,    75,    79,    84,    89,
    94,   100,   106,   112,   119,   126,   133,   141,   150,   159,
   168,   178,   189,   200,   212,   225,   238,   252,   267,   283,
   300,   318,   337,   357,   378,   401,   425,   450,   477,   505,
   535,   567,   601,   637,   675,   715,   757,   802,   850,   901,
   954,  1011,  1071,  1135,  1202,  1274,  1350,  1430,  1515,  1605,
  1701,  1802,  1909,  2022,  2143,  2270,  2405,  2548,  2700,  2860,
  3030,  3211,  3402,  3604,  3818,  4045,  4286,  4541,  4811,  5097,
  5400,  5721,  6061,  6422,  6804,  7208,  7637,  8091,  8572,  9082,
  9622, 10195, 10801, 11443, 12123, 12844, 13608, 14417, 15275, 16183,
 17145, 18165, 19245, 20390, 21602, 22887, 24247, 25689,
};

void SynthInit(Synthesizer *synth)
{
	SoundUnitUnion *soundUnionList = &(synth->SoundUnitUnionList[0]);
	for (uint8_t i = 0; i < POLY_NUM; i++)
	{
		soundUnionList[i].combine.increment = 0;
		soundUnionList[i].combine.envelopeStatus = STATUS_IDLE;
		soundUnionList[i].combine.waveGenAccumulator = 0;
		soundUnionList[i].combine.envelopeLevel = 255;
		soundUnionList[i].combine.envelopePos = 0;
		soundUnionList[i].combine.val = 0;
	}
	synth->lastSoundUnit = 0;
}
#ifdef RUN_TEST
void NoteOnC(uint8_t note)
{
	uint8_t lastSoundUnit = synthForC.lastSoundUnit;

	// disable_interrupts();
	synthForC.SoundUnitUnionList[lastSoundUnit].combine.increment = AccumIncrement[note & 0x7F];
	synthForC.SoundUnitUnionList[lastSoundUnit].combine.envelopeStatus = STATUS_ATTACK;
	synthForC.SoundUnitUnionList[lastSoundUnit].combine.waveGenAccumulator = 0;
	synthForC.SoundUnitUnionList[lastSoundUnit].combine.envelopePos = 0;
	synthForC.SoundUnitUnionList[lastSoundUnit].combine.envelopeLevel = 255;
	// enable_interrupts();

	lastSoundUnit++;

	if (lastSoundUnit == POLY_NUM)
		lastSoundUnit = 0;

	synthForC.lastSoundUnit = lastSoundUnit;
}

void NoteOnAsmP(uint8_t note)
{
	NoteOnC(note);
}

void SynthC(void)
{
	synthForC.mixOut = 0;
	SoundUnitUnion *soundUnionList = &(synthForC.SoundUnitUnionList[0]);
	for (uint8_t i = 0; i < POLY_NUM; i++)
	{
		if (soundUnionList[i].combine.envelopeLevel == 0)
			continue;
		soundUnionList[i].combine.sampleVal = soundUnionList[i].split.waveGenAccumulator_int;
		soundUnionList[i].combine.val = (uint16_t)soundUnionList[i].combine.envelopeLevel * soundUnionList[i].combine.sampleVal;

		soundUnionList[i].combine.waveGenAccumulator += soundUnionList[i].combine.increment;
		synthForC.mixOut += (soundUnionList[i].combine.val >> 8);
	}
}
#endif
void GenDecayEnvlopeC(void)
{
	__data SoundUnitUnion *soundUnionList = &(synthForAsm.SoundUnitUnionList[0]);
	for (uint8_t i = 0; i < POLY_NUM; i++)
	{
		if (soundUnionList[i].combine.envelopeStatus == STATUS_ATTACK)
		{
			if (soundUnionList[i].combine.envelopePos < ATTACK_TIME_FACTOR)
				soundUnionList[i].combine.envelopePos += 1;
			else
			{
				soundUnionList[i].combine.envelopePos = 0;
				soundUnionList[i].combine.envelopeStatus = STATUS_DECAY;
			}
		}
		else if (soundUnionList[i].combine.envelopeStatus == STATUS_DECAY)
		{
			if (soundUnionList[i].combine.envelopePos < (sizeof(EnvelopeTable) - 1))
			{
				soundUnionList[i].combine.envelopeLevel = EnvelopeTable[soundUnionList[i].combine.envelopePos];
				soundUnionList[i].combine.envelopePos += 1;
			}
			else
			{
				soundUnionList[i].combine.envelopeStatus = STATUS_IDLE;
			}
		}
	}
}

void GenDecayEnvlopeAsmP(void)
{
	GenDecayEnvlopeC();
}
