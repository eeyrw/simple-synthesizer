.area CSEG    (CODE)
_SynthAsm:
	pSynth = SynthAbsAddr
	mov (pSynth+pMixOut_l),#0
	mov (pSynth+pMixOut_h),#0	

	; r4,r5,r6,r7 : temporary registers

.irp  Idx,0,1,2,3,4,5

	pSndUnit = pSynth+Idx*unitSz
	mov a, (pSndUnit+pEnvelopeLevel)
	mov b,a
	jz soundUnitProcessEnd'Idx'$

	mov a,(pSndUnit+pWaveGenAccumulator_int)
	mov (pSndUnit+pSampleVal),a

	mul ab
	mov (pSndUnit+pVal_l),a
	mov (pSndUnit+pVal_h),b
	mov a,b			; Div with 0xFF
	mov b,#0

	add a, (pSynth+pMixOut_l)
	mov (pSynth+pMixOut_l),a
	mov a,b
	addc a,(pSynth+pMixOut_h)
	mov (pSynth+pMixOut_h),a


	mov a,(pSndUnit+pIncrement_frac)
	add a,(pSndUnit+pWaveGenAccumulator_frac)
	mov (pSndUnit+pWaveGenAccumulator_frac),a

	mov a,(pSndUnit+pIncrement_int)
	addc a,(pSndUnit+pWaveGenAccumulator_int)
	mov (pSndUnit+pWaveGenAccumulator_int),a
soundUnitProcessEnd'Idx'$:
.endm

	mov r4,(pSynth+pMixOut_l)
	mov r5,(pSynth+pMixOut_h)

	mov a,r5
	mov c,a.7
	rrc a
	mov r5,a
	mov a,r4
	rrc a
	mov r4,a

;	SynthCore.c:110: if(synthForAsm.mixOut>512)
	clr	c
	mov a,#511
	subb	a,r4
	mov	a,#(511>>8)
	subb	a,r5
	jnc	clipBranchEnd$
;	SynthCore.c:111: synthForAsm.mixOut=512;
	mov	r4,#511
	mov	r5,#(511>>8)
	clipBranchEnd$:

	; mixOut = mixOut>>1
	mov a,r5
	mov c,a.7
	rrc a
	mov r5,a
	mov a,r4
	rrc a
	mov r4,a

	mov REG_CCAP0H,a
	cpl a
	mov REG_CCAP1H,a