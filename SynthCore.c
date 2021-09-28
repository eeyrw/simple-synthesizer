#include "SynthCore.h"
#include <stdint.h>
#include <stdio.h>

#ifdef RUN_TEST
Synthesizer synthForC;
#endif

__code uint16_t AccumIncrement[128];

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
				soundUnionList[i].combine.envelopeStatus = STATUS_STOP;
			}
		}
	}
}

void GenDecayEnvlopeAsmP(void)
{
	GenDecayEnvlopeC();
}
#endif