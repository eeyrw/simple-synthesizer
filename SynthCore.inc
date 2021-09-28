ENVELOP_LEN=256
POLY_NUM=6


DECAY_TIME_FACTOR=150


; typedef struct _SoundUnit
; {
; 	uint16_t increment;
; 	uint8_t envelopeStatus;
; 	uint16_t waveGenAccumulator;
; 	uint8_t envelopeLevel;
; 	uint8_t envelopePos;
; 	int16_t val;
; 	int8_t sampleVal;
; } SoundUnit;

; typedef struct _SoundUnitSplit
; {
; 	uint8_t increment_frac;
; 	uint8_t increment_int;
; 	uint8_t envelopeStatus;
; 	uint16_t waveGenAccumulator_frac;
; 	uint16_t waveGenAccumulator_int;
; 	uint8_t envelopeLevel;
; 	uint8_t envelopePos;
; 	int16_t val;
; 	int8_t sampleVal;
; } SoundUnitSplit;

unitSz=10

pIncrement_int=1
pIncrement_frac=0
pEnvelopeStatus=2
pWaveGenAccumulator_int=4
pWaveGenAccumulator_frac=3
pEnvelopeLevel=5
pEnvelopePos=6
pVal_l=7
pVal_h=8
pSampleVal=9

pMixOut_l=unitSz*POLY_NUM
pMixOut_h=unitSz*POLY_NUM+1
pLastSoundUnit=unitSz*POLY_NUM+2

SynthAbsAddr = 0x10+11+6
SynthTotalSize = unitSz*POLY_NUM+3

